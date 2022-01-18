#version 410 core
layout (location = 0) in vec2 aTexCoord;
layout (location = 1) in vec3 aPos;

out vec2 texcoords;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
	texcoords = aTexCoord;
	gl_Position = uProj*uView*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}