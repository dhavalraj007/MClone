#include"log.h"
#include"game/chunkSystem.h"
#include"core/globals.h"
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
#include"glad/glad.h"
#include"graphics/Misc.h"

namespace game
{
	static constexpr uint32_t MAX_X = Chunk::CHUNK_WIDTH + 1;	//meaning x can be 0-16//these are also constants in shader lightingVertex.
	static constexpr uint32_t MAX_Y = Chunk::CHUNK_HEIGHT + 1;//meaning y can be 0-256
	static constexpr uint32_t MAX_Z = Chunk::CHUNK_BREADTH + 1;//meaning z can be 0-16

   //I need to do this instead of seperately oring the x,y,z values because if x can go from 0-15 then cube vertex's x pos can go from 0-(16+1) so we need 1 extra bit for every axes.
#define toIndexForCompression(x,y,z) ((x) + (z) * MAX_X + (y) * (MAX_X * MAX_Z))

	// 0-16 = position  (0x  1 FFFF)
	// 17 - 26 = texID(0x7FE 0000)
	// 27 - 28 = uvID(0x1800 0000)
	// 29 - 31 = face(0xE000 0000)
#define CompressVertex(x,y,z,texID,uvID,blockface) \
	toIndexForCompression(x, y, z) & 0x1FFFF \
	| (texID << 17) & 0x7FE0000 \
	| (uvID << 27) & 0x18000000 \
	| (blockface << 29) & 0xE0000000

	//static constexpr uint32_t CompressVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texID, uint32_t uvID, BlockFace blockFace)
	//{
	//	uint32_t data = 0;
	//	data |= toIndexForCompression(x, y, z) & 0x1FFFF;	//0-16 bits
	//	data |= (texID << 17) & 0x7FE0000;					// 17-26 bits
	//	data |= (uvID << 27) & 0x18000000;					// 27-28 bits
	//	data |= (blockFace << 29) & 0xE0000000;				// 29-31 bits
	//	return data;
	//}

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
			blockFormats[id].sideID = textureFormatNode["BLOCKS"][blockFormats[id].side]["ID"].as<uint32_t>();
			blockFormats[id].topID = textureFormatNode["BLOCKS"][blockFormats[id].top]["ID"].as<uint32_t>();
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

	/// <summary>
	/// this func requires internalBlock data for chunk(i.e which block is of which type(id) ) to be set and then based on that data it creates vertex data (face culled) and that can be rendered using Chunk::vao.
	///  chunk has to have the positions set.(because the perlinNoise generation depends on it and therefore vertex Generation also depends on that).
	/// this also stores the shared ptr to chunk inside vector ChunkSystem::chunks
	/// </summary>
	/// <param name="chunk"> for which the data is to be created</param>
	void ChunkSystem::createVertexDataForChunk(std::shared_ptr<Chunk> chunk)
	{
		MCLONE_ASSERT(chunk->isBlocksInit, "call createData on chunk first!");

		//now we fill chunk->vertexData for chunk that will passed onto Chunk::vao later

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
						chunk->vertexData.insert(chunk->vertexData.end(),
							{
								CompressVertex(0u + x , 0u + y  , 0u + z , blockFormats[blockId].sideID, 0, (int)BlockFace::BACK),
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
						chunk->vertexData.insert(chunk->vertexData.end(),
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
						chunk->vertexData.insert(chunk->vertexData.end(),
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
						chunk->vertexData.insert(chunk->vertexData.end(),
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
						chunk->vertexData.insert(chunk->vertexData.end(),
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
						chunk->vertexData.insert(chunk->vertexData.end(),
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
		MCLONE_TRACE("{}", chunk->vertexData.size());

		chunk->allDataDone = true;
	}

	//gpu upload work
	void ChunkSystem::uploadDataToChunkVao(std::shared_ptr<Chunk> chunk)
	{
		MCLONE_CREATE_VERTEX_BUFFER(vb, uint32_t);
		vb->setLayout({ 1 });
		vb->pushVertices(std::move(chunk->vertexData));
		vb->upload(false);

		chunk->vao->pushBuffer(std::move(vb));
		chunk->vao->upload();
		static int i = 0;
		MCLONE_TRACE("chunk# {}", i++);
		MCLONE_TRACE("chunk pos: {} {} {}", chunk->position.x, chunk->position.y, chunk->position.z);
		chunks.insert(chunk);
	}

	static bool shouldStartDeleting = false;
	static bool shouldStartUploading = true;

	void ChunkSystem::uploadReadyChunksToGpu()
	{
		static const int uploadRatePerFrame = 2;
		static const int uploadInterval = 20;
		if ((globals::frameCount < uploadInterval) || (shouldStartUploading && (globals::frameCount % uploadInterval) == 0))
		{
			// gpu work no async
			int uploadCount = 0;
			auto& chunk_i = newChunks.begin();
			while (((globals::frameCount < uploadInterval) || uploadCount < uploadRatePerFrame) && chunk_i != newChunks.end())
			{
				auto chunk = *(chunk_i);
				if (chunk->allDataDone && !chunk->vao->isValid())
				{
					this->uploadDataToChunkVao(chunk);
					chunk_i = newChunks.erase(chunk_i);
					uploadCount++;
				}
				else
					chunk_i++;
			}

			if (chunk_i == newChunks.end())//meaning all chunks are done uploading
			{
				shouldStartDeleting = true;
				shouldStartUploading = false;
			}
		}
	}
	void ChunkSystem::cleanUpChunks()
	{
		static const int deleteRatePerFrame = 2;
		static const int deleteInterval = 5;

		if (shouldStartDeleting && (globals::frameCount % deleteInterval) == 0)		// delete out of bounds chunks only after loading new chunks
		{
			int deleteCount = 0;
			auto& chunk_i = chunks.begin();
			while (deleteCount < deleteRatePerFrame && chunk_i != chunks.end())
			{
				auto chunk = *(chunk_i);
				if (chunk->position.x > currChunkAreaCenter.x + chunkRadius
					|| chunk->position.z > currChunkAreaCenter.z + chunkRadius
					|| chunk->position.x < currChunkAreaCenter.x - chunkRadius
					|| chunk->position.z < currChunkAreaCenter.z - chunkRadius)
				{
					MCLONE_TRACE("erasing chunk at {} {} {}", chunk->position.x, chunk->position.y, chunk->position.z);
					chunkPosSet.erase(chunk->position);
					chunk_i = chunks.erase(chunk_i);
					deleteCount++;
				}
				else
					chunk_i++;
			}
			if (chunk_i == chunks.end())//meaning all outof bounds chunks are done deleting
			{
				shouldStartDeleting = false;
				shouldStartUploading = true;
			}
		}
	}

	static void w_dataWorker(ChunkSystem* cs, std::shared_ptr<game::Chunk> chunk, uint32_t seed)
	{
		chunk->createData(seed);
		cs->createVertexDataForChunk(chunk);
	}

	void ChunkSystem::createChunkAreaAround(const glm::vec3& camPos)
	{
		currChunkAreaCenter = glm::ivec3{ camPos.x / 16,0,camPos.z / 16 };
		for (int z = -chunkRadius; z <= chunkRadius; z++)
		{
			for (int x = -chunkRadius; x <= chunkRadius; x++)
			{
				auto pos = currChunkAreaCenter + glm::ivec3{ x,0,z };
				if (chunkPosSet.find(pos) != chunkPosSet.end())	//if pos exists
					continue;
				chunkPosSet.insert(pos);

				std::shared_ptr<game::Chunk> chunk = std::make_shared<game::Chunk>(pos);
				newChunks.insert(chunk);

				f_dataFutures.push_back(std::async(std::launch::async, w_dataWorker, this, chunk, 12345));	// creates internalBlock Data
			}
		}
		uploadReadyChunksToGpu();

		// to remove the other chunks
		cleanUpChunks();
	}
}