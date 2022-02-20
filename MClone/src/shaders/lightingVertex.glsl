#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aFace;

out vec2 texcoords;
out vec3 fPos;
flat out float fFace;

uniform mat4 uView;
uniform mat4 uProj;

void main()
{
	fFace = aFace;
	fPos = aPos;
	texcoords = aTexCoord;
	gl_Position = uProj*uView*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}