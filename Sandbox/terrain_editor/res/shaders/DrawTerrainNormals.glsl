R"(
#type vertex
#version 410 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec2 posOffset;
layout(location = 3) in vec2 texOffset;

uniform sampler2D heightMap;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float maxTessLevel;
uniform float detailLevel;

uniform float heightFactor;

out ToTessControl
{
	vec2 texCoord;
	float level;
	bool inScreen;
	vec2 scrCoords;
} outVS; 

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
	outVS.texCoord = texCoord + texOffset;
	
	vec4 pos = vec4( gl_Position.xyz + vec3(0, texture( heightMap, outVS.texCoord).r * heightFactor, 0.0 ), 1.0);
	pos = projectionMatrix * viewMatrix * modelMatrix * pos;
	
	outVS.level = Level( length(pos) );
	outVS.scrCoords = pos.xy / pos.w;
	outVS.inScreen = InScreen( outVS.scrCoords );
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

out ToGeom
{
	vec3 normal;
} outTES;

#define Interpolate( a, p ) \
	( mix( \
		mix( a[0] p, a[1] p, gl_TessCoord.x ), \
		mix( a[3] p, a[2] p, gl_TessCoord.x ), \
		gl_TessCoord.y ) )

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

void main()
{
	vec4 newPosition = Interpolate( gl_in, .gl_Position );
	vec2 newTexCoord = Interpolate( inTES, .texCoord );
	newPosition.y = HeightFromTexture(newTexCoord) ;

	gl_Position = projectionMatrix * viewMatrix *  modelMatrix * newPosition;

	outTES.normal = normalize(texture(normalMap, newTexCoord).xzy);
}

#type geometry
#version 400 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in ToGeom
{
	vec3 normal;
} inGS[];

const float MAGNITUDE = 10.5;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(inGS[index].normal, 0.0) * MAGNITUDE;
	EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // вектор нормали для первой вершины
    GenerateLine(1); // ... для второй
    GenerateLine(2); // ... для третьей
}

#type fragment
#version 400 core

out vec4 fragColor;

void main()
{
	fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}  
)"
