#version 450 core

// =========================================
out FS_IN
{
    vec3 worldPos;
    vec3 norm;
} o;
// =========================================

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
    vec4 worldPosition = vec4(quad[gl_VertexID], 1.0);
    o.worldPos = worldPosition.xyz;
    o.norm = vec3(0.0f, 1.0f, 0.0f);
    
	gl_Position = projection * view * worldPosition;
}