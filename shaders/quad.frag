#version 450 core

layout(location = 0) out vec4 oColor;

uniform vec4 GravitationColor;

void main()
{
	oColor = GravitationColor;
}