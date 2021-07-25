/*
	Простой шейдер для displacement mapping'а с квадратными патчами.
*/

#type vertex
#version 410 core

layout(location = 0) in vec2 position;		
layout(location = 1) in vec2 texCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform float		unMaxTessLevel	= 32.0;
uniform float		unDetailLevel	= 100.0;
uniform sampler2D	unHeightMap;

out ToTessControl
{
	vec2 texCoord0;
	float	fLevel;
} output; 

float Level(float dist)
{
	return clamp( unDetailLevel / dist - 2.0, 1.0, unMaxTessLevel );
}

void main()
{
	gl_Position = vec4( position.x, position.y, 0, 1.0 ).xzyw;
	output.texCoord0 = texCoord;
	vec4	pos			= vec4( gl_Position.xyz + vec3(0, texture( unHeightMap, output.texCoord0).r * 2.0, 0.0 ), 1.0);
	pos = projectionMatrix * viewMatrix * pos;
	output.fLevel		= Level( length(pos) );
}

#type tesscontrol
#version 410 core

layout(vertices = 4) out;

in ToTessControl
{
	vec2 texCoord0;
	float	fLevel;
} input[]; 

out ToTessEval
{
	vec2 texCoord;
} output[];

void main()
{
	
	float	max_level = max( max( input[0].fLevel, input[1].fLevel ),
								max( input[2].fLevel, input[3].fLevel ) );
	gl_TessLevelInner[0] = max_level;
	gl_TessLevelInner[1] = max_level;
	gl_TessLevelOuter[0] = max( input[0].fLevel, input[3].fLevel );
	gl_TessLevelOuter[1] = max( input[0].fLevel, input[1].fLevel );
	gl_TessLevelOuter[2] = max( input[1].fLevel, input[2].fLevel );
	gl_TessLevelOuter[3] = max( input[2].fLevel, input[3].fLevel );


	gl_out[gl_InvocationID].gl_Position	= gl_in[gl_InvocationID].gl_Position;
	output[gl_InvocationID].texCoord = input[gl_InvocationID].texCoord0;

}


#type tessevaluation
#version 410 core

layout(quads, equal_spacing, ccw) in;


uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D unHeightMap;
uniform float unHeightScale	= 10.0;


in ToTessEval
{
	vec2 texCoord;
} input[];

#define Interpolate( a, p ) \
	( mix( \
		mix( a[0] p, a[1] p, gl_TessCoord.x ), \
		mix( a[3] p, a[2] p, gl_TessCoord.x ), \
		gl_TessCoord.y ) )

float PCF(in vec2 vTexcoord)
{
	float height = 0.0;
	height += textureOffset( unHeightMap, vTexcoord, ivec2( 0, 0) ).r * 2.0;
	return ( height * 0.1 );
}

void main()
{
	vec4 pos = Interpolate( gl_in, .gl_Position );
	vec2 texc = Interpolate( input, .texCoord );
	pos.y = PCF(texc) * unHeightScale;

	gl_Position = projectionMatrix * viewMatrix * pos;
}


#type fragment
#version 410 core

layout(location = 0) out vec4	outColor;

void main()
{
	outColor = vec4(0.3f, 0.3f, 0.3f, 1.f);
}
