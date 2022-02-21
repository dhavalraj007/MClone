#version 410 core


in vec3 fPos;
flat in uint fFace;
in vec2 texcoords;

out vec4 FragColor;

uniform sampler2D tex1;
uniform vec3 uLightPos;
uniform vec3 uLightColor;

void faceToNormal(in int face, out vec3 normal)
{
	switch(face)
	{
		case int(0):
			normal = vec3(0, 0, -1);
			break;
		case int(1):
			normal = vec3(0, 0, 1);
			break;
		case int(2):
			normal = vec3(-1, 0, 0);
			break;
		case int(3):
			normal = vec3(1, 0, 0);
			break;
		case int(4):
			normal = vec3(0, -1, 0);
			break;
		case int(5):
			normal = vec3(0, 1, 0);
			break;
	}
}

void main()
{
	float ambientFactor = 0.3f;
	vec3 ambient = ambientFactor*uLightColor;
	
	vec3 normal;
	faceToNormal(int(fFace),normal);
	normal = normalize(normal);
	vec3 lightDir = normalize(uLightPos - fPos);  
	float diff = max(0.f,dot(lightDir,normal));
	vec3 diffuse = diff*uLightColor;
	
	vec3 result = (ambient+diffuse)*texture(tex1,texcoords).rgb;
	FragColor = vec4(result,1.0);
}