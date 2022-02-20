#version 410 core
layout (location = 0) in vec3 aPos;


uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uScaleMat;
uniform vec3 uLightPos;

void main()
{
	gl_Position = uProj*uView*(uScaleMat*vec4(aPos.x, aPos.y, aPos.z, 1.0) + vec4(uLightPos,0.f));
}