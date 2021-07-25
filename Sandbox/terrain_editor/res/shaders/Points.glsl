R"(
#type vertex
#version 400 core

layout(location = 0) in vec3 position;

uniform vec3 landscapeScale;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
	vec4 worldPos = vec4(position, 1);
	gl_Position = projectionMatrix * viewMatrix * worldPos; 
}


#type fragment
#version 400 core
 
uniform vec4 pointColor;

out vec4 fragColor; 

void main() 
{ 
	fragColor = pointColor; 
	//fragColor = vec4(outTexCoord, 1, 1); 
}
)"