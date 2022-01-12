#version 410 core
layout (location = 0) in vec2 aTexCoord;
layout (location = 1) in vec3 aPos;
out vec2 texcoords;
void main()
{
	texcoords = aTexCoord;
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}