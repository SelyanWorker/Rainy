#type vertex
#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

uniform sampler2D heightMap;
uniform sampler2D normalMap;

uniform float heightFactor;

out ToFrag
{
	vec2 texCoord;
	vec3 position;
} output;

float PCF(in vec2 vTexcoord)
{
	float	height = 0.0;
	height += textureOffset( heightMap, vTexcoord, ivec2(-1,-1) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2(-1, 0) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2(-1, 1) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2( 0,-1) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2( 0, 0) ).r * 2.0;
	height += textureOffset( heightMap, vTexcoord, ivec2( 0, 1) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2( 1,-1) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2( 1, 0) ).r;
	height += textureOffset( heightMap, vTexcoord, ivec2( 1, 1) ).r;
	return ( height * 0.1 );
}

void main()
{
	vec4 pos = vec4(position, 1.f);
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
	output.texCoord = texCoord;
	output.position = pos.xyz;
}


#type fragment
#version 410 core

layout(location = 0) out vec4 outColor;

uniform sampler2D normalMap;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float diffInt;

in ToFrag
{
	vec2 texCoord;
	vec3 position;
} input;

float zNear = 0.1; 
float zFar  = 1000.0; 
  
float LinearizeDepth(float depth) 
{
    // преобразуем обратно в NDC
    float z = depth * 2.0 - 1.0; 
    return (2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear));	
}

void main()
{
	vec3 normal = normalize(texture(normalMap, input.texCoord).xzy);
	vec3 s = normalize(lightPosition - input.position);
	float sDotN = max(dot(s, normal), 0.0);
	vec4 resultLightColor = vec4(lightColor * diffInt * sDotN, 1);

	//outColor = vec4(s, 1.f);
	outColor = resultLightColor;
	//outColor = vec4(1, 0, 0, 1);
	//outColor = vec4(input.normal, 1.f);
	//outColor = vec4(input.position.y, 0, 0, 1);
	//outColor = vec4(input.texCoord, 0, 1);

	//float depth = LinearizeDepth(gl_FragCoord.z);
    //outColor = vec4(vec3(depth), 1.0);
}



