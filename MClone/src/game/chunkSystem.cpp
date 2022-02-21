#include"log.h"
#include"game/chunkSystem.h"
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
#include"glad/glad.h"

namespace game
{
	//todo: add ID based textures to eliminate unorderded_map
	// load name textureformat from mainNode to ChunkSystem::textureFormats
	void ChunkSystem::loadTextureFormat(YAML::Node mainNode, std::string name)
	{
		textureFormat tf;
		tf.name = name;
		tf.uvs[0][0] = mainNode["BLOCKS"][name]["uvs"][0][0].as<float>();
		tf.uvs[0][1] = mainNode["BLOCKS"][name]["uvs"][0][1].as<float>();
		tf.uvs[1][0] = mainNode["BLOCKS"][name]["uvs"][1][0].as<float>();
		tf.uvs[1][1] = mainNode["BLOCKS"][name]["uvs"][1][1].as<float>();
		tf.uvs[2][0] = mainNode["BLOCKS"][name]["uvs"][2][0].as<float>();
		tf.uvs[2][1] = mainNode["BLOCKS"][name]["uvs"][2][1].as<float>();
		tf.uvs[3][0] = mainNode["BLOCKS"][name]["uvs"][3][0].as<float>();
		tf.uvs[3][1] = mainNode["BLOCKS"][name]["uvs"][3][1].as<float>();
		textureFormats[name] = tf;
	}

	void ChunkSystem::loadFormats()
	{
		YAML::Node blockFormatNode = YAML::LoadFile("blockFormat.Yaml");
		YAML::Node textureFormatNode = YAML::LoadFile("textureFormat.yaml");
		blockFormats.resize(100);
		for (auto Nids : blockFormatNode["BLOCKS"])
		{
			int id = Nids.first.as<int>();
			MCLONE_ASSERT(id != 0, "0 ID is reserved for NULL_BLOCK.");

			blockFormats[id].name = Nids.second["name"].as<std::string>();
			blockFormats[id].side = Nids.second["side"].as<std::string>();
			blockFormats[id].top = Nids.second["top"].as<std::string>();
			blockFormats[id].bottom = Nids.second["bottom"].as<std::string>();
			blockFormats[id].isTransparent = Nids.second["isTransparent"].as<bool>();

			loadTextureFormat(textureFormatNode, blockFormats[id].side);
			loadTextureFormat(textureFormatNode, blockFormats[id].top);
			loadTextureFormat(textureFormatNode, blockFormats[id].bottom);
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

	static const uint32_t MAX_X = Chunk::CHUNK_WIDTH+1;	//meaning x can be 0-16//these are also constants in shader lightingVertex.
	static const uint32_t MAX_Y = Chunk::CHUNK_HEIGHT+1;//meaning y can be 0-256
	static const uint32_t MAX_Z = Chunk::CHUNK_BREADTH+1;//meaning z can be 0-16
	
	//I need to do this instead of seperately oring the x,y,z values because if x can go from 0-15 then cube vertex's x pos can go from 0-(16+1) so we need 1 extra bit for every axes.
	static uint32_t toIndexForCompression(uint32_t x, uint32_t y, uint32_t z)
	{
		return x + z * MAX_X + y * (MAX_X * MAX_Z);
	}
	
	// 0-16 = position  (0x1FFFF)
	// 17 - 28 = texID(0x1FFE0000)
	// 29 - 31 = face(0xE0000000)
	static uint32_t CompressVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texID, BlockFace blockFace)
	{
		uint32_t data = 0;
		data |= toIndexForCompression(x, y, z) & 0x1FFFF;	//0-16 bits
		data |= (texID << 17) & 0x1FFE0000;					// 17-28 bits
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
		std::vector<float> vertexUVData;
		//std::vector<float> vertexData;

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
					if (chunk->isBlockNull(x, y, z - 1) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y, z - 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x , 0u + y  , 0u + z , 0, BlockFace::BACK),
								CompressVertex( 1 + x ,  1 + y  , 0u + z , 0, BlockFace::BACK),
								CompressVertex( 1 + x , 0u + y  , 0u + z , 0, BlockFace::BACK),
								CompressVertex( 1 + x ,  1 + y  , 0u + z , 0, BlockFace::BACK),
								CompressVertex(0u + x , 0u + y  , 0u + z , 0, BlockFace::BACK),
								CompressVertex(0u + x ,  1 + y  , 0u + z , 0, BlockFace::BACK)
							});
						vertexUVData.insert(vertexUVData.end(), {
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1]
							});
					}
					//front 023310
					if (chunk->isBlockNull(x, y, z + 1) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y, z + 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x , 0u + y  ,  1 + z , 0, BlockFace::FRONT),
								CompressVertex( 1 + x , 0u + y  ,  1 + z , 0, BlockFace::FRONT),
								CompressVertex( 1 + x ,  1 + y  ,  1 + z , 0, BlockFace::FRONT),
								CompressVertex( 1 + x ,  1 + y  ,  1 + z , 0, BlockFace::FRONT),
								CompressVertex(0u + x ,  1 + y  ,  1 + z , 0, BlockFace::FRONT),
								CompressVertex(0u + x , 0u + y  ,  1 + z , 0, BlockFace::FRONT)
							});
						vertexUVData.insert(vertexUVData.end(), {
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1]
							});
					}
					//left 310023
					if (chunk->isBlockNull(x - 1, y, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x - 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								CompressVertex(0u + x ,  1 + y  ,  1 + z ,0,BlockFace::LEFT),
								CompressVertex(0u + x ,  1 + y  , 0u + z ,0,BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  , 0u + z ,0,BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  , 0u + z ,0,BlockFace::LEFT),
								CompressVertex(0u + x , 0u + y  ,  1 + z ,0,BlockFace::LEFT),
								CompressVertex(0u + x ,  1 + y  ,  1 + z ,0,BlockFace::LEFT)
							});
						vertexUVData.insert(vertexUVData.end(), {
							 textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							 textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
							 textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							 textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							 textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
							 textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1]
							});
					}
					//right 301032
					if (chunk->isBlockNull(x + 1, y, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x + 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							 CompressVertex(1 + x ,  1 + y  ,  1 + z , 0, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  , 0u + z , 0, BlockFace::RIGHT),
							 CompressVertex(1 + x ,  1 + y  , 0u + z , 0, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  , 0u + z , 0, BlockFace::RIGHT),
							 CompressVertex(1 + x ,  1 + y  ,  1 + z , 0, BlockFace::RIGHT),
							 CompressVertex(1 + x , 0u + y  ,  1 + z , 0, BlockFace::RIGHT)
							});
						vertexUVData.insert(vertexUVData.end(), {
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
							textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1]
							});
					}
					//bottom 132201
					if (chunk->isBlockNull(x, y - 1, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y - 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							CompressVertex(0u + x , 0u + y  , 0u + z ,0, BlockFace::BOTTOM),
							CompressVertex( 1 + x , 0u + y  , 0u + z ,0, BlockFace::BOTTOM),
							CompressVertex( 1 + x , 0u + y  ,  1 + z ,0, BlockFace::BOTTOM),
							CompressVertex( 1 + x , 0u + y  ,  1 + z ,0, BlockFace::BOTTOM),
							CompressVertex(0u + x , 0u + y  ,  1 + z ,0, BlockFace::BOTTOM),
							CompressVertex(0u + x , 0u + y  , 0u + z ,0, BlockFace::BOTTOM)
							});
						vertexUVData.insert(vertexUVData.end(), {
							 textureFormats[blockFormats[blockId].bottom].uvs[1][0],textureFormats[blockFormats[blockId].bottom].uvs[1][1],
							 textureFormats[blockFormats[blockId].bottom].uvs[3][0],textureFormats[blockFormats[blockId].bottom].uvs[3][1],
							 textureFormats[blockFormats[blockId].bottom].uvs[2][0],textureFormats[blockFormats[blockId].bottom].uvs[2][1],
							 textureFormats[blockFormats[blockId].bottom].uvs[2][0],textureFormats[blockFormats[blockId].bottom].uvs[2][1],
							 textureFormats[blockFormats[blockId].bottom].uvs[0][0],textureFormats[blockFormats[blockId].bottom].uvs[0][1],
							 textureFormats[blockFormats[blockId].bottom].uvs[1][0],textureFormats[blockFormats[blockId].bottom].uvs[1][1]
							});
					}
					//top 123210
					if (chunk->isBlockNull(x, y + 1, z) || blockFormats[chunk->internalBlocks[chunk->to1DArray(x, y + 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							CompressVertex(0u + x ,  1 + y  , 0u + z ,0,BlockFace::TOP),
							CompressVertex( 1 + x ,  1 + y  ,  1 + z ,0,BlockFace::TOP),
							CompressVertex( 1 + x ,  1 + y  , 0u + z ,0,BlockFace::TOP),
							CompressVertex( 1 + x ,  1 + y  ,  1 + z ,0,BlockFace::TOP),
							CompressVertex(0u + x ,  1 + y  , 0u + z ,0,BlockFace::TOP),
							CompressVertex(0u + x ,  1 + y  ,  1 + z ,0,BlockFace::TOP)
							});
						vertexUVData.insert(vertexUVData.end(), {
							  textureFormats[blockFormats[blockId].top].uvs[1][0],textureFormats[blockFormats[blockId].top].uvs[1][1],
							  textureFormats[blockFormats[blockId].top].uvs[2][0],textureFormats[blockFormats[blockId].top].uvs[2][1],
							  textureFormats[blockFormats[blockId].top].uvs[3][0],textureFormats[blockFormats[blockId].top].uvs[3][1],
							  textureFormats[blockFormats[blockId].top].uvs[2][0],textureFormats[blockFormats[blockId].top].uvs[2][1],
							  textureFormats[blockFormats[blockId].top].uvs[1][0],textureFormats[blockFormats[blockId].top].uvs[1][1],
							  textureFormats[blockFormats[blockId].top].uvs[0][0],textureFormats[blockFormats[blockId].top].uvs[0][1]
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
		
		MCLONE_CREATE_VERTEX_BUFFER(vb2, float);
		vb2->setLayout({ 2 });
		vb2->pushVertices(vertexUVData);
		vb2->upload(false);

		chunk->vao->pushBuffer(std::move(vb));
		chunk->vao->pushBuffer(std::move(vb2));
		chunk->vao->upload();
		chunks.push_back(chunk);
	}
}