R"(
#type vertex
#version 410 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 posOffset;
layout(location = 3) in vec2 texOffset;

out ToTessControl
{
	vec2 texCoord;
	float level;
	bool inScreen;
	vec2 scrCoords;
} outFromVS; 

uniform sampler2D heightMap;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float maxTessLevel;
uniform float detailLevel;

uniform float heightFactor;

float Level(float dist)
{
	return clamp( detailLevel / dist - 2.0, 1.0, maxTessLevel );
}

bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return ( abs(pos.x) <= size && abs(pos.y) <= size );
}

void main()
{
	gl_Position = vec4( position + posOffset, 0, 1.0 ).xzyw;
	outFromVS.texCoord = texCoord + texOffset;
	
	vec4 pos = vec4( gl_Position.xyz + vec3(0, texture( heightMap, outFromVS.texCoord).r * heightFactor, 0.0 ), 1.0);
	pos = projectionMatrix * viewMatrix * modelMatrix * pos;
	
	outFromVS.level = Level( length(pos) );
	outFromVS.scrCoords = pos.xy / pos.w;
	outFromVS.inScreen = InScreen( outFromVS.scrCoords );
}

#type tesscontrol
#version 410 core

layout(vertices = 4) out;

uniform float maxTessLevel;
uniform float detailLevel;

in ToTessControl
{
	vec2 texCoord;
	float level;
	bool inScreen;
	vec2 scrCoords;
} inTCS[]; 

out ToTessEval
{
	vec2 texCoord;
} outTCS[];

#define Min4( _a, _b, _c, _d )	min( min( _a, _b ), min( _c, _d ) )
#define Max4( _a, _b, _c, _d )	max( max( _a, _b ), max( _c, _d ) )

vec4 Rect(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3)
{
	return vec4( 
		Min4( p0.x, p1.x, p2.x, p3.x ),
		Min4( p0.y, p1.y, p2.y, p3.y ),
		Max4( p0.x, p1.x, p2.x, p3.x ),
		Max4( p0.y, p1.y, p2.y, p3.y ) 
	);
}

bool QuadInScreen()
{
	const float		size = 1.2;
	
	vec4 scrRect	= vec4( -size, -size, size, size );
	vec4 patchRect = Rect( inTCS[0].scrCoords, inTCS[1].scrCoords,
							 inTCS[2].scrCoords, inTCS[3].scrCoords );
	return	( patchRect[0] < scrRect[2] && patchRect[2] > scrRect[0] &&
			  patchRect[1] < scrRect[3] && patchRect[3] > scrRect[1] ) ||
			( scrRect[2] < patchRect[0] && scrRect[0] > patchRect[2] &&
			  scrRect[3] < patchRect[1] && scrRect[1] > patchRect[3] );
}

float Level(in vec2 p0, in vec2 p1)
{
	return clamp( distance( p0, p1 ) * detailLevel * 0.0075, 0.1, maxTessLevel );
}

void main()
{
	if ( gl_InvocationID == 0 )
	{
		bool inScreen = any( bvec4( inTCS[0].inScreen, inTCS[1].inScreen,
										inTCS[2].inScreen, inTCS[3].inScreen ) );
		float k = ( inScreen || QuadInScreen() ) ? 1.0 : 0.0;
		
		float max_level = max( max( Level( inTCS[0].scrCoords, inTCS[3].scrCoords ), Level( inTCS[0].scrCoords, inTCS[1].scrCoords ) ),
								max( Level( inTCS[1].scrCoords, inTCS[2].scrCoords ), Level( inTCS[2].scrCoords, inTCS[3].scrCoords ) ) );
		
		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;
		
		gl_TessLevelOuter[0] = max( inTCS[0].level, inTCS[3].level ) * k;
		gl_TessLevelOuter[1] = max( inTCS[0].level, inTCS[1].level ) * k;
		gl_TessLevelOuter[2] = max( inTCS[1].level, inTCS[2].level ) * k;
		gl_TessLevelOuter[3] = max( inTCS[2].level, inTCS[3].level ) * k;
	}

	gl_out[gl_InvocationID].gl_Position	= gl_in[gl_InvocationID].gl_Position;
	outTCS[gl_InvocationID].texCoord = inTCS[gl_InvocationID].texCoord;
}

#type tessevaluation
#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D normalMap;
uniform sampler2D heightMap;

uniform float heightFactor;

in ToTessEval
{
	vec2 texCoord;
} inTES[];

out ToFrag
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
	vec3 normal0;
} outTES;

#define Interpolate( a, p ) ( mix( mix( a[0] p, a[1] p, gl_TessCoord.x ), mix( a[3] p, a[2] p, gl_TessCoord.x ), gl_TessCoord.y ) )

float HeightFromTexture(in vec2 texCoord)
{
	float	height = 0.0;
	height += textureOffset( heightMap, texCoord, ivec2(-1,-1) ).r;
	height += textureOffset( heightMap, texCoord, ivec2(-1, 0) ).r;
	height += textureOffset( heightMap, texCoord, ivec2(-1, 1) ).r;
	height += textureOffset( heightMap, texCoord, ivec2( 0,-1) ).r;
	height += textureOffset( heightMap, texCoord, ivec2( 0, 0) ).r * 2.0;
	height += textureOffset( heightMap, texCoord, ivec2( 0, 1) ).r;
	height += textureOffset( heightMap, texCoord, ivec2( 1,-1) ).r;
	height += textureOffset( heightMap, texCoord, ivec2( 1, 0) ).r;
	height += textureOffset( heightMap, texCoord, ivec2( 1, 1) ).r;
	return ( height * 0.1 * heightFactor);
}

//uniform sampler2D unit_wave
//noperspective in vec2 tex_coord;
//const vec2 size = vec2(2.0,0.0);
//const ivec3 off = ivec3(-1,0,1);
//vec4 wave = texture(unit_wave, tex_coord);
//float s11 = wave.x;
//float s01 = textureOffset(unit_wave, tex_coord, off.xy).x;
//float s21 = textureOffset(unit_wave, tex_coord, off.zy).x;
//float s10 = textureOffset(unit_wave, tex_coord, off.yx).x;
//float s12 = textureOffset(unit_wave, tex_coord, off.yz).x;
//vec3 va = normalize(vec3(size.xy,s21-s01));
//vec3 vb = normalize(vec3(size.yx,s12-s10));
//vec4 bump = vec4( cross(va,vb), s11 );

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

void main()
{
	vec4 newPosition = Interpolate( gl_in, .gl_Position );
	vec2 newTexCoord = Interpolate( inTES, .texCoord );
	newPosition.y = texture(heightMap, newTexCoord).r * heightFactor;

	gl_Position = projectionMatrix * viewMatrix *  modelMatrix * newPosition;

	outTES.position = newPosition.xyz;
	outTES.texCoord = newTexCoord;
	outTES.normal = normalize(texture(normalMap, newTexCoord).xzy);
	outTES.normal0 = outTES.normal;

	float s11 = newPosition.y;
	float s01 = textureOffset(heightMap, newTexCoord, off.xy).r * heightFactor;
	float s21 = textureOffset(heightMap, newTexCoord, off.zy).r * heightFactor;
	float s10 = textureOffset(heightMap, newTexCoord, off.yx).r * heightFactor;
	float s12 = textureOffset(heightMap, newTexCoord, off.yz).r * heightFactor;

	vec3 va = normalize(vec3(size.xy, s21-s01));
	vec3 vb = normalize(vec3(size.yx, s12-s10));

	outTES.normal0 = cross(va,vb).xzy;
}

#type fragment
#version 410 core

layout(location = 0) out vec4 outColor0;
layout(location = 1) out vec3 outColor1;

//uniform sampler2D normalMap;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float diffInt;

uniform float brushSize;
uniform vec3 brushColor;
uniform vec3 interPoint;

uniform bool drawGrid = true;
uniform float gridSize = 0.05f;
uniform vec4 gridColor = vec4(0, 1.f, 0, 1.f);

in ToFrag
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
	vec3 normal0;
} inFS;

void main()
{
	//vec3 normal = normalize(texture(normalMap, inFS.texCoord).xzy);
	vec3 s = normalize(lightPosition - inFS.position);
	float sDotN = max(dot(s, inFS.normal0), 0.0);
	vec4 resultLightColor = vec4(lightColor * diffInt * sDotN, 1);

	vec4 addColor;
	if(length(inFS.position - interPoint) <= brushSize) addColor = vec4(brushColor, 0);
	else addColor = vec4(0);
	
	//outColor = vec4(normalize(interPoint), 1);
	//outColor = vec4(s, 1);
	outColor0 = resultLightColor + addColor;
	//outColor = vec4(addColor.xyz, 1);
	//outColor = vec4(1, 0, 0, 1);
	//outColor = vec4(inFS.normal0, 1);
	//outColor = vec4(inFS.position.y, 0, 0, 1);
	//outColor = vec4(inFS.texCoord, 0, 1);

	//float equalNull = fract(inFS.position.x) + fract(inFS.position.z);
	// && fract(inFS.position.z) <= 0.1f

	// draw grid
	if(drawGrid)
	{
		if(fract(inFS.position.x) <= gridSize || fract(inFS.position.x) >= (1.f - gridSize) ||
			fract(inFS.position.z) <= gridSize || fract(inFS.position.z) >= (1.f - gridSize)) 
			outColor0 = gridColor;
	}
	//outColor = vec4(vec3(fract(inFS.position.x), fract(inFS.position.z), 0), 1.0);

	outColor1 = inFS.normal0;
}
)"