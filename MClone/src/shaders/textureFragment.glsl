#version 410 core

in vec2 texcoords;
out vec4 FragColor;
uniform sampler2D tex1;


void main()
{
	FragColor = texture(tex1,texcoords);
}