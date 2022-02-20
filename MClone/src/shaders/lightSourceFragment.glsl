#version 410 core

out vec4 FragColor;
uniform sampler2D tex1;
uniform vec3 uLightColor;

void main()
{
	FragColor = vec4(uLightColor,1.f);//color
}