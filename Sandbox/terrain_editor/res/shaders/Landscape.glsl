#type vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 WorldPositionToFS;
out vec3 NormalToFS;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
	vec4 worldPosition = modelMatrix * vec4(position, 1);
	WorldPositionToFS = vec3(worldPosition);
	NormalToFS = normalize((modelMatrix * vec4(normal, 0)).xyz);
	//NormalToFS  = normal;
	gl_Position = projectionMatrix * viewMatrix * worldPosition; 
}

#type fragment
#version 400 core

in vec3 WorldPositionToFS;
in vec3 NormalToFS;

out vec4 fragColor; 

uniform vec4 mainColor;
uniform vec4 brushColor;
uniform float brushSize;
uniform vec3 brushCenter;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform float diffInt;

void main()
{ 
	vec3 s = normalize(lightPosition - WorldPositionToFS);
	float sDotN = max(dot(s, NormalToFS), 0.0);
	vec4 lightColor = vec4(lightColor * diffInt * sDotN, 1);

	vec4 colorSum = mainColor + lightColor;
	//vec4 colorSum = vec4(NormalToFS, 1) + lightColor;

	float distance = distance(WorldPositionToFS.xz, brushCenter.xz);
	if(distance < brushSize) 
		colorSum += brushColor;

	fragColor = colorSum;	
}
