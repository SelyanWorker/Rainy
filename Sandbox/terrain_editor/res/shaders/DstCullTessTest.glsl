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
uniform float		unDetailLevel	= 1000.0;
uniform sampler2D	unHeightMap;

out ToTessControl
{
	vec2 texCoord0;
	vec2	vScrCoords;
	float	fLevel;
	bool	bInScreen;
} output; 

bool InScreen(in vec2 pos)
{
	const float		size = 1.2;
	return ( abs(pos.x) <= size && abs(pos.y) <= size );
}

float Level(float dist)
{
	return clamp( unDetailLevel/*1000.0*/ / dist - 2.0, 1.0, unMaxTessLevel );
}

void main()
{
	gl_Position = vec4( position.x, position.y, 0, 1.0 ).xzyw;
	output.texCoord0 = texCoord;
	vec4	pos			= vec4( gl_Position.xyz + vec3(0, texture( unHeightMap, output.texCoord0).r * 2.0, 0.0 ), 1.0);
	pos = projectionMatrix * viewMatrix * pos;
	output.vScrCoords	= pos.xy / pos.w;
	output.fLevel		= Level( length(pos) );
	output.bInScreen	= InScreen( output.vScrCoords );
}

#type tesscontrol
#version 410 core

layout(vertices = 4) out;

in ToTessControl
{
	vec2 texCoord0;
	vec2	vScrCoords;
	float	fLevel;
	bool	bInScreen;
} input[]; 

out ToTessEval
{
	vec2 texCoord;
} output[];

#define Min4( _a, _b, _c, _d )	min( min( _a, _b ), min( _c, _d ) )
#define Max4( _a, _b, _c, _d )	max( max( _a, _b ), max( _c, _d ) )

vec4 Rect(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3)
{
	return 	 vec4(	Min4( p0.x, p1.x, p2.x, p3.x ),
					Min4( p0.y, p1.y, p2.y, p3.y ),
					Max4( p0.x, p1.x, p2.x, p3.x ),
					Max4( p0.y, p1.y, p2.y, p3.y ) );
}

bool QuadInScreen()
{
	const float		size = 1.2;
	
	vec4	screen	= vec4( -size, -size, size, size );
	vec4	rect 	=  Rect( input[0].vScrCoords, input[1].vScrCoords,
							 input[2].vScrCoords, input[3].vScrCoords );
	return	( rect[0] < screen[2] && rect[2] > screen[0] &&
			  rect[1] < screen[3] && rect[3] > screen[1] ) ||
			( screen[2] < rect[0] && screen[0] > rect[2] &&
			  screen[3] < rect[1] && screen[1] > rect[3] );
}

void main()
{
	
	if ( gl_InvocationID == 0 )
	{
		bool	in_screen = any( bvec4( input[0].bInScreen, input[1].bInScreen,
										input[2].bInScreen, input[3].bInScreen ) );
		float	max_level = max( max( input[0].fLevel, input[1].fLevel ),
								 max( input[2].fLevel, input[3].fLevel ) );
		float	k = ( in_screen || QuadInScreen() ) ? 1.0 : 0.0;
		
		gl_TessLevelInner[0] = max_level * k;
		gl_TessLevelInner[1] = max_level * k;
		gl_TessLevelOuter[0] = max( input[0].fLevel, input[3].fLevel ) * k;
		gl_TessLevelOuter[1] = max( input[0].fLevel, input[1].fLevel ) * k;
		gl_TessLevelOuter[2] = max( input[1].fLevel, input[2].fLevel ) * k;
		gl_TessLevelOuter[3] = max( input[2].fLevel, input[3].fLevel ) * k;
	}


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
