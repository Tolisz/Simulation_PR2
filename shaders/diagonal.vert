#version 450 core

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

uniform mat4 model;

vec3 linePoints[2] = {
    vec3(-1, -1, -1), 
	vec3(+1, +1, +1)
};

void main() 
{
	gl_Position = projection * view * model * vec4(linePoints[gl_VertexID], 1.0f);
}