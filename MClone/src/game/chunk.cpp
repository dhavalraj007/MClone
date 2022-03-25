#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
#include"perlinNoise/perlinNoise.hpp"
#include<fstream>
#include<filesystem>

namespace game
{
	static internalBlock nullBlock;

	Chunk::Chunk(const glm::ivec3 pos)
		:position(pos)
	{
		vao = std::make_shared<graphics::VertexArray>();
	}

	void Chunk::createData(uint32_t seed)
	{
		static siv::PerlinNoise perlin{ seed };
		float worldChunkX = float(position.x * CHUNK_WIDTH);
		float worldChunkZ = float(position.z * CHUNK_BREADTH);

		static float freq = 8;
		static float fx = freq / (16.f * 4.f * 40.f);
		static float fy = freq / (16.f * 4.f * 40.f);

		internalBlocks = std::vector<internalBlock>(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH, nullBlock);

		std::string filePath = "res/data/" + std::to_string(position.x) + "_" + std::to_string(position.z) + ".bin";
		if (std::filesystem::exists(filePath))
			deserialize("res/data");
		else
		{
			for (int x = 0; x < CHUNK_WIDTH; x++)
			{
				for (int z = 0; z < CHUNK_BREADTH; z++)
				{
					int maxHeight = (int)(perlin.normalizedOctave2D_01((x + worldChunkX) * fx, (z + worldChunkZ) * fy, 8) * CHUNK_HEIGHT);
					int maxHeightStone = (int)(perlin.normalizedOctave2D_01((x + worldChunkX) * fx, (z + worldChunkZ) * fy, 8) * (CHUNK_HEIGHT / 2));
					for (int y = 0; y < CHUNK_HEIGHT; y++)
					{
						if (y == 0)
							internalBlocks[to1DArray(x, y, z)].id = 6;	//bedrock
						else if (y < maxHeightStone)
							internalBlocks[to1DArray(x, y, z)].id = 5;	//stone
						else if (y < maxHeight)
							internalBlocks[to1DArray(x, y, z)].id = 3;	//dirt
						else if (y == maxHeight)
							internalBlocks[to1DArray(x, y, z)].id = 1;	//grass
						else
							internalBlocks[to1DArray(x, y, z)].id = 0;	//null block

						//internalBlocks[to1DArray(x, y, z)].id = IDEmitter(x, y, z);
					}
				}
			}
			serialize("res/data");
		}
		isBlocksInit = true;
	}
	void Chunk::serialize(const std::string& worldPath)
	{
		std::string filePath = worldPath + "/" + std::to_string(position.x) + "_" + std::to_string(position.z) + ".bin";
		std::fstream file;
		file.open(filePath, std::ios_base::out | std::ios_base::binary);
		MCLONE_ASSERT(file.is_open(), "Error: FILE is not opened!");
		if (file.is_open())
		{
			file.write(reinterpret_cast<char*>(&internalBlocks[0]), internalBlocks.size()*sizeof(internalBlock));
		}
		file.close();
	}
	
	void Chunk::deserialize(const std::string& worldPath)
	{
		std::string filePath = worldPath + "/" + std::to_string(position.x) + "_" + std::to_string(position.z) + ".bin";
		std::fstream file(filePath, std::ios::in || std::ios::binary);
		MCLONE_ASSERT(file.is_open(), "Error: FILE is not opened!");
		if (file.is_open())
		{
			for(int i=0;i< CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH;i++)
				internalBlocks[i]=(file);
			/*file.read(reinterpret_cast<char*>(&internalBlocks), CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH*sizeof(internalBlock));*/
		}
	}

	const internalBlock& Chunk::getBlockAt(int x, int y, int z)
	{
		MCLONE_ASSERT(isBlocksInit, "blocks are not inititialized! call createData first!");
		if (isBlocksInit)
			return internalBlocks[to1DArray(x, y, z)];
		else
			return nullBlock;
	}

}