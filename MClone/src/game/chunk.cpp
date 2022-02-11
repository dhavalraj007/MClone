#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
namespace game
{
	constexpr int CHUNK_WIDTH = 16;
	constexpr int CHUNK_HEIGHT = 384;
	constexpr int CHUNK_BREADTH = 16;

	static std::unordered_map<std::string, int> nameToIdMap;
	static std::vector<blockFormat> blockFormats;
	static std::unordered_map<std::string, textureFormat> textureFormats;
	static std::vector<int> blockIds;

	// load name textureformat from mainNode to std::unordered_map<std::string,textureFormat> textureFormats
	void loadTextureFormat(YAML::Node mainNode, std::string name)
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

	void loadFormats()
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

	inline int to1DArray(int x, int y, int z)
	{
		if (x >= CHUNK_WIDTH || x < 0 || y >= CHUNK_HEIGHT || y < 0 || z >= CHUNK_BREADTH || z < 0)
			return -1;
		return (x + y * CHUNK_WIDTH + z * (CHUNK_WIDTH * CHUNK_HEIGHT));
	}

	bool isBlockNull(int x, int y, int z)
	{
		int ret = to1DArray(x, y, z);
		return ret == -1 ?
			true
			: (blockIds[ret] == 0);
	}

	void genBlockIds()
	{
		blockIds = std::vector<int>(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH, 0);
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_BREADTH; z++)
				{
					if(y<20)
					blockIds[to1DArray(x, y, z)] = 2;
					else
					blockIds[to1DArray(x, y, z)] = 1;
				}
			}
		}
	}
	std::shared_ptr<graphics::VertexArray> createChunk()
	{
		loadFormats();
		std::shared_ptr<graphics::VertexArray> chunk_vao = std::make_shared<graphics::VertexArray>();
		std::vector<float> vertexData;

		genBlockIds();
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				for (int z = 0; z < CHUNK_BREADTH; z++)
				{
					int blockId = blockIds[to1DArray(x, y, z)];
					textureFormat texformSide = textureFormats[blockFormats[blockId].side];
					textureFormat texformTop = textureFormats[blockFormats[blockId].top];
					textureFormat texformBottom = textureFormats[blockFormats[blockId].bottom];

					//00 - 0
					//01 - 1
					//10 - 2
					//11 - 3

					//back	032301
					if (isBlockNull(x, y, z - 1) || blockFormats[blockIds[to1DArray(x, y, z - 1)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x, -0.5f + y, -0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
								 0.5f + x,  0.5f + y, -0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
								 0.5f + x, -0.5f + y, -0.5f + z,  texformSide.uvs[2][0],texformSide.uvs[2][1],
								 0.5f + x,  0.5f + y, -0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
								-0.5f + x, -0.5f + y, -0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
								-0.5f + x,  0.5f + y, -0.5f + z,  texformSide.uvs[1][0],texformSide.uvs[1][1],
							});
					}
					//front 023310
					if (isBlockNull(x, y, z + 1) || blockFormats[blockIds[to1DArray(x, y, z + 1)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x, -0.5f + y,  0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
								 0.5f + x, -0.5f + y,  0.5f + z,  texformSide.uvs[2][0],texformSide.uvs[2][1],
								 0.5f + x,  0.5f + y,  0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
								 0.5f + x,  0.5f + y,  0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
								-0.5f + x,  0.5f + y,  0.5f + z,  texformSide.uvs[1][0],texformSide.uvs[1][1],
								-0.5f + x, -0.5f + y,  0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
							});
					}
					//left 310023
					if (isBlockNull(x - 1, y, z) || blockFormats[blockIds[to1DArray(x - 1, y, z)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
								-0.5f + x,  0.5f + y,  0.5f + z, texformSide.uvs[3][0],texformSide.uvs[3][1],
								-0.5f + x,  0.5f + y, -0.5f + z, texformSide.uvs[1][0],texformSide.uvs[1][1],
								-0.5f + x, -0.5f + y, -0.5f + z, texformSide.uvs[0][0],texformSide.uvs[0][1],
								-0.5f + x, -0.5f + y, -0.5f + z, texformSide.uvs[0][0],texformSide.uvs[0][1],
								-0.5f + x, -0.5f + y,  0.5f + z, texformSide.uvs[2][0],texformSide.uvs[2][1],
								-0.5f + x,  0.5f + y,  0.5f + z, texformSide.uvs[3][0],texformSide.uvs[3][1],
							});
					}
					//right 301032
					if (isBlockNull(x + 1, y, z) || blockFormats[blockIds[to1DArray(x + 1, y, z)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							 0.5f + x,  0.5f + y,  0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
							 0.5f + x, -0.5f + y, -0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
							 0.5f + x,  0.5f + y, -0.5f + z,  texformSide.uvs[1][0],texformSide.uvs[1][1],
							 0.5f + x, -0.5f + y, -0.5f + z,  texformSide.uvs[0][0],texformSide.uvs[0][1],
							 0.5f + x,  0.5f + y,  0.5f + z,  texformSide.uvs[3][0],texformSide.uvs[3][1],
							 0.5f + x, -0.5f + y,  0.5f + z,  texformSide.uvs[2][0],texformSide.uvs[2][1],
							});
					}
					//bottom 132201
					if (isBlockNull(x, y - 1, z) || blockFormats[blockIds[to1DArray(x, y - 1, z)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							-0.5f + x, -0.5f + y, -0.5f + z,  texformBottom.uvs[1][0],texformBottom.uvs[1][1],
							 0.5f + x, -0.5f + y, -0.5f + z,  texformBottom.uvs[3][0],texformBottom.uvs[3][1],
							 0.5f + x, -0.5f + y,  0.5f + z,  texformBottom.uvs[2][0],texformBottom.uvs[2][1],
							 0.5f + x, -0.5f + y,  0.5f + z,  texformBottom.uvs[2][0],texformBottom.uvs[2][1],
							-0.5f + x, -0.5f + y,  0.5f + z,  texformBottom.uvs[0][0],texformBottom.uvs[0][1],
							-0.5f + x, -0.5f + y, -0.5f + z,  texformBottom.uvs[1][0],texformBottom.uvs[1][1],
							});
					}

					//top 123210
					if (isBlockNull(x, y + 1, z) || blockFormats[blockIds[to1DArray(x, y + 1, z)]].isTransparent)
					{
						vertexData.insert(vertexData.end(),
							{
							-0.5f + x,  0.5f + y, -0.5f + z,  texformTop.uvs[1][0],texformTop.uvs[1][1],
							 0.5f + x,  0.5f + y,  0.5f + z,  texformTop.uvs[2][0],texformTop.uvs[2][1],
							 0.5f + x,  0.5f + y, -0.5f + z,  texformTop.uvs[3][0],texformTop.uvs[3][1],
							 0.5f + x,  0.5f + y,  0.5f + z,  texformTop.uvs[2][0],texformTop.uvs[2][1],
							-0.5f + x,  0.5f + y, -0.5f + z,  texformTop.uvs[1][0],texformTop.uvs[1][1],
							-0.5f + x,  0.5f + y,  0.5f + z,  texformTop.uvs[0][0],texformTop.uvs[0][1],
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
		chunk_vao->pushBuffer(std::move(vb));
		chunk_vao->upload();
		return chunk_vao;
	}
}