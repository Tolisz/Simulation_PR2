#version 450 core

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

vec3 quad[6] = 
{
    vec3(1, 0, 1), vec3(-1, 0, -1), vec3(-1, 0, 1),
	vec3(-1, 0, -1), vec3(1, 0, 1), vec3(1, 0, -1)
};

void main()
{
	gl_Position = projection * view * vec4(quad[gl_VertexID], 1.0);
}