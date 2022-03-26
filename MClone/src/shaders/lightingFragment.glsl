#version 410 core


in vec3 fPos;
flat in uint fFace;
flat in uint texID;
in vec2 texCoords;

out vec4 FragColor;

uniform sampler2DArray uTexArray;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uPlayerPos;
uniform int  uChunkRadius;

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


	float distanceToPlayer = length(fPos-uPlayerPos);
	const float density = 0.01;
	const float gradient = 2;
	float visibility = clamp(exp(-pow((distanceToPlayer*density),gradient)),0.0,1.0);
	//float d = distanceToPlayer/(uChunkRadius*16.0);
	vec4 fogColor = vec4(153.0 / 255.0, 204.0 / 255.0, 1.0, 1.0);


 	vec4 texel = texture(uTexArray,vec3(texCoords,texID));

	vec3 result = (ambient+diffuse)*texel.rgb;
	FragColor = mix(fogColor,vec4(result,1.0),visibility);
	
}