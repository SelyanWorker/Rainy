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
} output; 

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
	output.texCoord = texCoord + texOffset;
	
	vec4 pos = vec4( gl_Position.xyz + vec3(0, texture( heightMap, output.texCoord).r * heightFactor, 0.0 ), 1.0);
	pos = projectionMatrix * viewMatrix * modelMatrix * pos;
	
	output.level = Level( length(pos) );
	output.scrCoords = pos.xy / pos.w;
	output.inScreen = InScreen( output.scrCoords );
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
} input[]; 

out ToTessEval
{
	vec2 texCoord;
} output[];

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
	vec4 patchRect = Rect( input[0].scrCoords, input[1].scrCoords,
							 input[2].scrCoords, input[3].scrCoords );
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
		bool inScreen = any( bvec4( input[0].inScreen, input[1].inScreen,
										input[2].inScreen, input[3].inScreen ) );
		float k = ( inScreen || QuadInScreen() ) ? 1.0 : 0.0;
		
		float	max_level = max( max( input[0].level, input[1].level ),
								 max( input[2].level, input[3].level ) );
		gl_TessLevelInner[0] = max_level;
		gl_TessLevelInner[1] = max_level;
		gl_TessLevelOuter[0] = max( input[0].level, input[3].level );
		gl_TessLevelOuter[1] = max( input[0].level, input[1].level );
		gl_TessLevelOuter[2] = max( input[1].level, input[2].level );
		gl_TessLevelOuter[3] = max( input[2].level, input[3].level );
		
		/*gl_TessLevelOuter[0] = Level( input[0].scrCoords, input[3].scrCoords ) * k;
		gl_TessLevelOuter[1] = Level( input[0].scrCoords, input[1].scrCoords ) * k;
		gl_TessLevelOuter[2] = Level( input[1].scrCoords, input[2].scrCoords ) * k;
		gl_TessLevelOuter[3] = Level( input[2].scrCoords, input[3].scrCoords ) * k;
		
		float	max_level = max( max( gl_TessLevelOuter[0], gl_TessLevelOuter[1]),
								 max( gl_TessLevelOuter[2], gl_TessLevelOuter[3] ) );

		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;*/
	}

	gl_out[gl_InvocationID].gl_Position	= gl_in[gl_InvocationID].gl_Position;
	output[gl_InvocationID].texCoord = input[gl_InvocationID].texCoord;
}


#type tessevaluation
#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D heightMap;
uniform sampler2D normalMap;

uniform float heightFactor;

in ToTessEval
{
	vec2 texCoord;
} input[];

out ToFrag
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} output;

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
	vec2 newTexCoord = Interpolate( input, .texCoord );
	newPosition.y = HeightFromTexture(newTexCoord) ;
	
	vec4 worldPosition = modelMatrix * newPosition;

	gl_Position = projectionMatrix * viewMatrix *  worldPosition;

	output.position = worldPosition.xyz;
	output.texCoord = newTexCoord;
	output.normal = normalize(texture(normalMap, newTexCoord).xzy);
}

#type fragment
#version 410 core

layout(location = 0) out vec4 outColor;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float diffInt;

in ToFrag
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} input;

void main()
{
	//vec3 s = normalize(lightPosition - input.position);
	//float sDotN = max(dot(s, input.normal), 0.0);
	//vec4 resultLightColor = vec4(lightColor * diffInt * sDotN, 1);
	vec4 resultLightColor = vec4(vec3(input.normal.y - 0.3f), 1);

	//outColor = vec4(vec3(sDotN), 1.f);
	outColor = resultLightColor;
	//outColor = vec4(1, 0, 0, 1);
	//outColor = vec4(input.normal, 1.f);
	//outColor = vec4(il.nput.position.y, 0, 0, 1);
	//outColor = vec4(input.texCoord, 0, 1);

	
}


