#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

layout(std140, binding = 0) uniform MatricesBlock {
    mat4 view;
    mat4 projection;
};

void main()
{
   gl_Position = projection * view * vec4(aPos, 1.0);
   ourColor = aColor;

// 	gl_Position = vec4(aPos, 1.0);
//    ourColor = aColor;
};