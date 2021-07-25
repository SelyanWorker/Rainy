

#type vertex
#version 410 core

layout(location = 0) in vec2 position;		
layout(location = 1) in vec2 texCoord;

out ToTessControl
{
	vec2 texCoord0;
} output; 


void main()
{
	gl_Position = vec4( position.x, position.y, 0, 1.0 ).xzyw;
	output.texCoord0 = texCoord;
}

#type tesscontrol
#version 410 core

layout(vertices = 4) out;

uniform float maxTessLevel = 64.0;

in ToTessControl
{
	vec2 texCoord0;
} input[]; 

out ToTessEval
{
	vec2 texCoord;
} output[];

void main()
{
	
	gl_TessLevelInner[0] = maxTessLevel;
	gl_TessLevelInner[1] = maxTessLevel;
	gl_TessLevelOuter[0] = maxTessLevel;
	gl_TessLevelOuter[1] = maxTessLevel;
	gl_TessLevelOuter[2] = maxTessLevel;
	gl_TessLevelOuter[3] = maxTessLevel;

	gl_out[gl_InvocationID].gl_Position	= 
		gl_in[gl_InvocationID].gl_Position;
	output[gl_InvocationID].texCoord = 
		input[gl_InvocationID].texCoord0;

}


#type tessevaluation

#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D heightMap;
uniform float heightScale = 10.0;

in ToTessEval
{
	vec2 texCoord;
} input[];

#define Interpolate( a, p ) \
	( mix( \
		mix( a[0] p, a[1] p, gl_TessCoord.x ), \
		mix( a[3] p, a[2] p, gl_TessCoord.x ), \
		gl_TessCoord.y ) )

void main()
{
	vec4 pos = Interpolate( gl_in, .gl_Position );
	vec2 texc = Interpolate( input, .texCoord );
	pos.y = texture( heightMap, texCoord ).r * heightScale;

	gl_Position = projectionMatrix * viewMatrix * pos;
}


#type fragment
#version 410 core

layout(location = 0) out vec4	outColor;

void main()
{
	outColor = vec4(0.3f, 0.3f, 0.3f, 1.f);
}
