#version 450 core

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

vec3 gridPlane[6] = 
{
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
	vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
};

void main() {
    gl_Position = projection * view * vec4(gridPlane[gl_VertexID].xyz, 1.0);
}