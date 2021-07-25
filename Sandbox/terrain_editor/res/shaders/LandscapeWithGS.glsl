#type vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 WorldVertexPosition;
out vec3 VertexPositionToLight;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
	vec4 worldPosition = modelMatrix * vec4(position, 1);
	WorldVertexPosition = vec3(worldPosition);
	VertexPositionToLight = WorldVertexPosition;
	gl_Position = projectionMatrix * viewMatrix * worldPosition; 
}

#type geometry
#version 400 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 WorldVertexPosition[];
out vec3 Normal;
out vec3 WorldPosition;

vec3 GetNormal()
{
	//vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	//vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 a = WorldVertexPosition[0] - WorldVertexPosition[1];
	vec3 b = WorldVertexPosition[2] - WorldVertexPosition[1];
	return normalize(cross(b, a));
}

void main() 
{ 
	vec3 normal = GetNormal();

	gl_Position = gl_in[0].gl_Position;
	Normal = normal;
	WorldPosition = WorldVertexPosition[0];
	EmitVertex();

	gl_Position = gl_in[1].gl_Position;
	Normal = normal;
	WorldPosition = WorldVertexPosition[1];
	EmitVertex();

	gl_Position = gl_in[2].gl_Position;
	Normal = normal;
	WorldPosition = WorldVertexPosition[2];
	EmitVertex();

	EndPrimitive();
}

#type fragment
#version 400 core

in vec3 WorldPosition;
in vec3 Normal;

out vec4 fragColor; 

uniform vec4 mainColor;
uniform vec4 brushColor;
uniform float brushSize;
uniform vec3 brushCenter;

uniform float intensity;
uniform vec3 lightPosition;

void main() 
{ 
	vec3 s = normalize(lightPosition - WorldPosition);
	float sDotN = max(dot(s, Normal), 0.0);
	vec4 lightColor = vec4(vec3(1, 1, 1) * intensity * sDotN, 1);

	vec4 colorSum = mainColor + lightColor;
	float distance = distance(WorldPosition.xz, brushCenter.xz);
	
	if(distance < brushSize) 
		colorSum += brushColor;

	fragColor = colorSum;	
}
