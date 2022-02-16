#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"

namespace game
{
	static internalBlock nullBlock = { 0,0,0,0 };

	Chunk::Chunk(const glm::vec3 pos)
		:position(pos)
	{
		vao = std::make_shared<graphics::VertexArray>();
	}

	void Chunk::createData()
	{
		internalBlocks = std::vector<internalBlock>(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH, nullBlock);
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_BREADTH; z++)
				{
					internalBlocks[to1DArray(x, y, z)].id = IDEmitter(x, y, z);
				}
			}
		}
		isBlocksInit = true;
	}

	const internalBlock& Chunk::getBlockAt(int x, int y, int z)
	{
		MCLONE_ASSERT(isBlocksInit, "blocks are not inititialized! call createData first!");
		if (isBlocksInit)
			return internalBlocks[to1DArray(x, y, z)];
		else
			return nullBlock;
	}

	int Chunk::IDEmitter(int x, int y, int z)
	{
		float worldChunkX = position.x * CHUNK_WIDTH;
		float worldChunkZ = position.z * CHUNK_BREADTH;

		int maxHeight = (int)(perlin.octave2D_01((x+worldChunkX)*0.005f, (z+worldChunkZ)*0.005f, 8)*255.0); // 0-255.f
		if (y == 0)
			return 6;	//bedrock
		if (y < maxHeight)
			return 3;	//dirt
		if (y == maxHeight)
			return 1;	//grass
		else
			return 0;	//null block
	}
}