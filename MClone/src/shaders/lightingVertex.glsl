#version 410 core
layout (location = 0) in uint aData;



out vec3 fPos;
flat out uint fFace;
flat out uint texID;
out vec2 texCoords;

uniform mat4 uView;
uniform mat4 uProj;
uniform ivec3 uChunkPos;

#define MAX_X 17
#define MAX_Y 257
#define MAX_Z 17


vec3 getPos(in uint data)
{
	uint index= data & 0x1FFFF;
	uint x = index%MAX_X;
	uint z = ((index-x)/MAX_X)%MAX_Z;
	uint y = (index-(x+z))/(MAX_X*MAX_Z);
	return vec3(x,y,z);
}

uint getTexId(in uint data)
{
	return (data & 0x7FE0000)>>17;
}
uint getUVId(in uint data)
{
	return (data & 0x18000000)>>27;
}

vec2 getUVCoords(in uint UVId)
{
	return vec2((UVId&2)>>1,UVId&1);
}

uint getFace(in uint data)
{
	return (data & 0xE0000000)>>29;
}

void main()
{
	vec3 aPos = getPos(aData);
	fPos = (aPos+uChunkPos*16.f);
	fFace = getFace(aData);
	texID = getTexId(aData);
	texCoords = getUVCoords(getUVId(aData));
	gl_Position = uProj*uView*(vec4(aPos.x, aPos.y, aPos.z, 1.0)+vec4(uChunkPos*16.f,0.0));
}