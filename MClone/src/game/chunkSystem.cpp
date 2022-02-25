#include"log.h"
#include"game/chunkSystem.h"
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
#include"glad/glad.h"
#include"graphics/Misc.h"
namespace game
{
	void ChunkSystem::loadFormats()
	{
		YAML::Node blockFormatNode = YAML::LoadFile("blockFormat.Yaml");
		YAML::Node textureFormatNode = YAML::LoadFile("textureFormat.yaml");
		blockFormats.resize(100);	// max 100 block types
	
		for (auto Nids : blockFormatNode["BLOCKS"])		// for all the block types fill in the blockFormats vector
		{
			int id = Nids.first.as<int>();
			MCLONE_ASSERT(id != 0, "0 ID is reserved for NULL_BLOCK.");

			blockFormats[id].name = Nids.second["name"].as<std::string>();
			blockFormats[id].side = Nids.second["side"].as<std::string>();
			blockFormats[id].top = Nids.second["top"].as<std::string>();
			blockFormats[id].bottom = Nids.second["bottom"].as<std::string>();
			blockFormats[id].isTransparent = Nids.second["isTransparent"].as<bool>();
			// at this point we have all the information from the blockFormats.yaml file into our datastructre blockFormats vector
			// now for all the block sides we have the texture names but not the ID so we have to get the ID from textureFormats.yaml
			blockFormats[id].sideID   = textureFormatNode["BLOCKS"][blockFormats[id].side]["ID"].as<uint32_t>();
			blockFormats[id].topID    = textureFormatNode["BLOCKS"][blockFormats[id].top]["ID"].as<uint32_t>();
			blockFormats[id].bottomID = textureFormatNode["BLOCKS"][blockFormats[id].bottom]["ID"].as<uint32_t>();
			nameToIdMap[blockFormats[id].name] = id;
		}
	}

	ChunkSystem::ChunkSystem()
	{
		loadFormats();
	}

	enum BlockFace
	{
		BACK = 0,
		FRONT = 1,
		LEFT = 2,
		RIGHT = 3,
		BOTTOM = 4,
		TOP = 5
	};

	static const uint32_t MAX_X = Chunk::CHUNK_WIDTH + 1;	//meaning x can be 0-16//these are also constants in shader lightingVertex.
	static const uint32_t MAX_Y = Chunk::CHUNK_HEIGHT + 1;//meaning y can be 0-256
	static const uint32_t MAX_Z = Chunk::CHUNK_BREADTH + 1;//meaning z can be 0-16

	//I need to do this instead of seperately oring the x,y,z values because if x can go from 0-15 then cube vertex's x pos can go from 0-(16+1) so we need 1 extra bit for every axes.
	static uint32_t toIndexForCompression(uint32_t x, uint32_t y, uint32_t z)
	{
		return x + z * MAX_X + y * (MAX_X * MAX_Z);
	}

	// 0-16 = position  (0x  1 FFFF)
	// 17 - 26 = texID(0x7FE 0000)
	// 27 - 28 = uvID(0x1800 0000)
	// 29 - 31 = face(0xE000 0000)
	static uint32_t CompressVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texID, uint32_t uvID, BlockFace blockFace)
	{
		uint32_t data = 0;
		data |= toIndexForCompression(x, y, z) & 0x1FFFF;	//0-16 bits
		data |= (texID << 17) & 0x7FE0000;					// 17-26 bits
		data |= (uvID << 27) & 0x18000000;					// 27-28 bits
		data |= (blockFace << 29) & 0xE0000000;				// 29-31 bits
		return data;
	}

	/// <summary>
	/// this func creates internalBlock data for chunk(i.e which block is of which type(id) ) and then based on that data it creates vertex data (face culled) and that can be rendered using Chunk::vao.
	///  chunk has to have the positions set.(because the perlinNoise generation depends on it and therefore vertex Generation also depends on that).
	/// this also stores the shared ptr to chunk inside vector ChunkSystem::chunks
	/// </summary>
	/// <param name="chunk"> for which the data is to be created</param>
	void ChunkSystem::createDataForChunk(std::shared_ptr<Chunk> chunk)
	{
		chunk->createData({ 12345 });	// creates internalBlock Data

		//now we create vertexData for chunk that will be stored in the Chunk::vao
		std::vector<uint32_t> vertexData;

		for (int x = 0; x < chunk->CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < chunk->CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < chunk->CHUNK_BREADTH; z++)
				{
					int blockId = chunk->getBlockAt(x, y, z).id;
					if (blockId == 0)	//nullblock
						continue;

					/*textureFormat texforside = textureFormats[blockFormats[blockId].side];
					textureFormat texfortop = textureFormats[blockFormats[blockId].top];
					textureFormat texforbottom= textureFormats[blockFormats[blockId].bottom];*/

					//00 - 0
					//01 - 1
					//10 - 2
					//11 - 3

					//back	032301
					if (chunk->isOutOfBounds(x, y, z - 1) || chunk->isBlockNull(x, y, z - 1) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y, z - 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 0, BlockFace::BACK),
								CompressVertex(1 + x ,  1 + y  , 0u + z , blockFormats[blockId].sideID, 3, BlockFace::BACK),
								CompressVertex(1 + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 2, BlockFace::BACK),
								CompressVertex(1 + x ,  1 + y  , 0u + z , blockFormats[blockId].sideID, 3, BlockFace::BACK),
								CompressVertex(0u + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 0, BlockFace::BACK),
								CompressVertex(0u + x ,  1 + y  , 0u + z , blockFormats[blockId].sideID, 1, BlockFace::BACK)
							});
					}
					//front 023310
					if (chunk->isOutOfBounds(x, y, z + 1) || chunk->isBlockNull(x, y, z + 1) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y, z + 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x , 0u + y  ,  1 + z , blockFormats[blockId].sideID, 0 ,BlockFace::FRONT),
								CompressVertex(1 + x , 0u + y  ,  1 + z , blockFormats[blockId].sideID, 2 ,BlockFace::FRONT),
								CompressVertex(1 + x ,  1 + y  ,  1 + z , blockFormats[blockId].sideID, 3 ,BlockFace::FRONT),
								CompressVertex(1 + x ,  1 + y  ,  1 + z , blockFormats[blockId].sideID, 3 ,BlockFace::FRONT),
								CompressVertex(0u + x ,  1 + y  ,  1 + z , blockFormats[blockId].sideID, 1 ,BlockFace::FRONT),
								CompressVertex(0u + x , 0u + y  ,  1 + z , blockFormats[blockId].sideID, 0 ,BlockFace::FRONT)
							});
					}
					//left 310023
					if (chunk->isOutOfBounds(x - 1, y, z) || chunk->isBlockNull(x - 1, y, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x - 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x ,  1 + y  ,  1 + z ,blockFormats[blockId].sideID, 3, BlockFace::LEFT),
								CompressVertex(0u + x ,  1 + y  , 0u + z ,blockFormats[blockId].sideID, 1, BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  , 0u + z ,blockFormats[blockId].sideID, 0, BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  , 0u + z ,blockFormats[blockId].sideID, 0, BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  ,  1 + z ,blockFormats[blockId].sideID, 2, BlockFace::LEFT),
								CompressVertex(0u + x ,  1 + y  ,  1 + z ,blockFormats[blockId].sideID, 3, BlockFace::LEFT)
							});
					}
					//right 301032
					if (chunk->isOutOfBounds(x + 1, y, z) || chunk->isBlockNull(x + 1, y, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x + 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							 CompressVertex(1 + x ,  1 + y  ,  1 + z , blockFormats[blockId].sideID, 3, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 0, BlockFace::RIGHT),
							 CompressVertex(1 + x ,  1 + y  , 0u + z , blockFormats[blockId].sideID, 1, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 0, BlockFace::RIGHT),
							 CompressVertex(1 + x ,  1 + y  ,  1 + z , blockFormats[blockId].sideID, 3, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  ,  1 + z , blockFormats[blockId].sideID, 2, BlockFace::RIGHT)
							});
					}
					//bottom 132201
					if (chunk->isOutOfBounds(x, y - 1, z) || chunk->isBlockNull(x, y - 1, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y - 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							CompressVertex(0u + x , 0u + y  , 0u + z ,blockFormats[blockId].bottomID,  1, BlockFace::BOTTOM),
							CompressVertex(1 + x , 0u + y  , 0u + z ,blockFormats[blockId].bottomID,  3, BlockFace::BOTTOM),
							CompressVertex(1 + x , 0u + y  ,  1 + z ,blockFormats[blockId].bottomID,  2, BlockFace::BOTTOM),
							CompressVertex(1 + x , 0u + y  ,  1 + z ,blockFormats[blockId].bottomID,  2, BlockFace::BOTTOM),
							CompressVertex(0u + x , 0u + y  ,  1 + z ,blockFormats[blockId].bottomID,  0, BlockFace::BOTTOM),
							CompressVertex(0u + x , 0u + y  , 0u + z ,blockFormats[blockId].bottomID,  1, BlockFace::BOTTOM)
							});
					}
					//top 123210
					if (chunk->isOutOfBounds(x, y + 1, z) || chunk->isBlockNull(x, y + 1, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y + 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							CompressVertex(0u + x ,  1 + y  , 0u + z ,blockFormats[blockId].topID, 1, BlockFace::TOP),
							CompressVertex(1 + x ,  1 + y  ,  1 + z ,blockFormats[blockId].topID, 2, BlockFace::TOP),
							CompressVertex(1 + x ,  1 + y  , 0u + z ,blockFormats[blockId].topID, 3, BlockFace::TOP),
							CompressVertex(1 + x ,  1 + y  ,  1 + z ,blockFormats[blockId].topID, 2, BlockFace::TOP),
							CompressVertex(0u + x ,  1 + y  , 0u + z ,blockFormats[blockId].topID, 1, BlockFace::TOP),
							CompressVertex(0u + x ,  1 + y  ,  1 + z ,blockFormats[blockId].topID, 0, BlockFace::TOP)
							});
					}
				}
			}
		}
		MCLONE_TRACE("{}", vertexData.size());

		MCLONE_CREATE_VERTEX_BUFFER(vb, uint32_t);
		vb->setLayout({ 1 });
		vb->pushVertices(vertexData);
		vb->upload(false);

		chunk->vao->pushBuffer(std::move(vb));
		chunk->vao->upload();
		chunks.push_back(chunk);
	}
}