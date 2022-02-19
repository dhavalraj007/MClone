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

	

	void ChunkSystem::addChunk(int posX,int posZ)
	{
		Chunk chunk({ posX,0.0f,posZ });
		MCLONE_TRACE("{},{}", posX, posZ);
		chunk.createData({12345});

		int worldPosX = posX*chunk.CHUNK_WIDTH;
		int worldPosZ = posZ*chunk.CHUNK_BREADTH;

		std::vector<float> vertexData;

		for (int x = 0; x < chunk.CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < chunk.CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < chunk.CHUNK_BREADTH; z++)
				{
					int blockId = chunk.getBlockAt(x,y,z).id;
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
					if (chunk.isBlockNull(x, y, z - 1) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x, y, z - 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
								 0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
								 0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
								 0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
								-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
								-0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
							});
					}
					//front 023310
					if (chunk.isBlockNull(x, y, z + 1) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x, y, z + 1)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
								 0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
								 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
								 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
								-0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
								-0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							});
					}
					//left 310023
					if (chunk.isBlockNull(x - 1, y, z) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x - 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
								-0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
								-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
								-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
								-0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
								-0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ, textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							});
					}
					//right 301032
					if (chunk.isBlockNull(x + 1, y, z) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x + 1, y, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							 0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							 0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[1][0],textureFormats[blockFormats[blockId].side].uvs[1][1],
							 0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[0][0],textureFormats[blockFormats[blockId].side].uvs[0][1],
							 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[3][0],textureFormats[blockFormats[blockId].side].uvs[3][1],
							 0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].side].uvs[2][0],textureFormats[blockFormats[blockId].side].uvs[2][1],
							});
					}
					//bottom 132201
					if (chunk.isBlockNull(x, y - 1, z) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x, y - 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[1][0],textureFormats[blockFormats[blockId].bottom].uvs[1][1],
							 0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[3][0],textureFormats[blockFormats[blockId].bottom].uvs[3][1],
							 0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[2][0],textureFormats[blockFormats[blockId].bottom].uvs[2][1],
							 0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[2][0],textureFormats[blockFormats[blockId].bottom].uvs[2][1],
							-0.5f + x + worldPosX, -0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[0][0],textureFormats[blockFormats[blockId].bottom].uvs[0][1],
							-0.5f + x + worldPosX, -0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].bottom].uvs[1][0],textureFormats[blockFormats[blockId].bottom].uvs[1][1],
							});
					}
					//top 123210
					if (chunk.isBlockNull(x, y + 1, z) || blockFormats[chunk.internalBlocks[chunk.to1DArray(x, y + 1, z)].id].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							-0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[1][0],textureFormats[blockFormats[blockId].top].uvs[1][1],
							 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[2][0],textureFormats[blockFormats[blockId].top].uvs[2][1],
							 0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[3][0],textureFormats[blockFormats[blockId].top].uvs[3][1],
							 0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[2][0],textureFormats[blockFormats[blockId].top].uvs[2][1],
							-0.5f + x + worldPosX,  0.5f + y  , -0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[1][0],textureFormats[blockFormats[blockId].top].uvs[1][1],
							-0.5f + x + worldPosX,  0.5f + y  ,  0.5f + z + worldPosZ,  textureFormats[blockFormats[blockId].top].uvs[0][0],textureFormats[blockFormats[blockId].top].uvs[0][1],
							});
					}
				}
			}
		}
		MCLONE_TRACE("{}", vertexData.size());

		MCLONE_CREATE_VERTEX_BUFFER(vb, float);
		vb->setLayout({ 3,2 });
		vb->pushVertices(vertexData);
		vb->upload(false);
		chunk.vao->pushBuffer(std::move(vb));
		chunk.vao->upload();
		chunks.push_back(std::move(chunk));
	}
	void ChunkSystem::render()
	{
		for (auto& chunk : chunks)
		{
			chunk.vao->bind();
			glDrawArrays(GL_TRIANGLES, 0, chunk.vao->getVertexCount());
		}
	}
}