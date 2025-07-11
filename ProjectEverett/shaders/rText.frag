#version 330 core
in vec2 TexCoords;
out vec4 Color;

uniform sampler2D text;
uniform vec4 textColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
	Color = textColor * sampled;
}