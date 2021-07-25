#type vertex
#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 NormalToDraw;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() 
{
	vec4 worldPosition = modelMatrix * vec4(position, 1);
	NormalToDraw = (viewMatrix * modelMatrix * vec4(normal, 0)).xyz;
	gl_Position = projectionMatrix * viewMatrix * worldPosition; 
}

#type geometry
#version 400 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 NormalToDraw[];

const float MAGNITUDE = 0.4;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(NormalToDraw[index], 0.0) * MAGNITUDE;
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
	fragColor = vec4(1.0, 1.0, 0.0, 1.0);
}  
