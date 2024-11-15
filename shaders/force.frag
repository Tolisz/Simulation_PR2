#version 450 core

layout(location = 0) out vec4 oColor;

uniform vec4 GravitationColor;

void main()
{
	// oColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	oColor = vec4(GravitationColor.xyz, 1.0f);
}