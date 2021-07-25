R"(
#type vertex
#version 400 core

layout(location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1); 
}

#type fragment
#version 400 core

out vec4 fragColor; 

uniform vec4 mainColor;

void main() 
{ 
	fragColor = mainColor;
}
)"