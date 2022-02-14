#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
namespace game
{
	Chunk::Chunk(IDEmitterFunc IDEmitter)
	{
		internalBlocks = std::vector<internalBlock>(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH, nullBlock);
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_BREADTH; z++)
				{
					internalBlocks[to1DArray(x,y,z)].id = IDEmitter(x, y, z);
				}
			}
		}
	}
}