#version 450 core

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;
uniform float edgeLength;

const vec3 massCenter = {0.0f, 0.0f, 0.0f};

void main() 
{
	vec4 worldPos = model * vec4(massCenter, 1.0f);
	if (gl_VertexID == 1) 
	{
		worldPos.y -= 0.5f * edgeLength;
	} 

	gl_Position = projection * view * worldPos;
}