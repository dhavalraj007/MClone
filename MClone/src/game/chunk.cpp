#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"game/chunk.h"
namespace game
{
	static std::unordered_map<std::string, int> nameToIdMap;
	static YAML::Node blockFormatNode;
	static YAML::Node textureFormatNode;
	void loadBlockFormats()
	{
		blockFormatNode = YAML::LoadFile("blockFormat.Yaml");
		for (auto Nids : blockFormatNode["BLOCKS"])
		{
			nameToIdMap[Nids.second["name"].as<std::string>()] = Nids.first.as<int>();
		}
	}
	void loadTextureFormats()
	{
		textureFormatNode = YAML::LoadFile("textureFormat.yaml");
	}

	std::shared_ptr<graphics::VertexArray> createChunk(int width, int height, int breadth)
	{
		loadBlockFormats();
		loadTextureFormats();
		std::shared_ptr<graphics::VertexArray> chunk_vao = std::make_shared<graphics::VertexArray>();
		std::vector<float> vertexData;
		//std::vector<uint32_t> elements(width * height * breadth * 36);
			//std::vector<float> verts;

		std::string id("1");
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < breadth; z++)
				{
					if (y < 10)
						id = "2";
					else
						id = "1";
					auto uvsSideNode = textureFormatNode["BLOCKS"][blockFormatNode["BLOCKS"][id]["side"].as<std::string>()]["uvs"];
					auto uvsTopNode = textureFormatNode["BLOCKS"][blockFormatNode["BLOCKS"][id]["top"].as<std::string>()]["uvs"];
					auto uvsBottomNode = textureFormatNode["BLOCKS"][blockFormatNode["BLOCKS"][id]["bottom"].as<std::string>()]["uvs"];

					//00 - 0
					//01 - 1
					//10 - 2
					//11 - 3

					vertexData.insert(vertexData.end(),
						{
							//pos					 					  //uvs

							//back
							-0.5f + x, -0.5f + y, -0.5f + z,  uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							 0.5f + x, -0.5f + y, -0.5f + z,  uvsSideNode["2"][0].as<float>(),uvsSideNode["2"][1].as<float>(),
							 0.5f + x,  0.5f + y, -0.5f + z,  uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							 0.5f + x,  0.5f + y, -0.5f + z,  uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							-0.5f + x,  0.5f + y, -0.5f + z,  uvsSideNode["1"][0].as<float>(),uvsSideNode["1"][1].as<float>(),
							-0.5f + x, -0.5f + y, -0.5f + z,  uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),

							//front
							-0.5f + x, -0.5f + y,  0.5f + z,  uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							 0.5f + x, -0.5f + y,  0.5f + z,  uvsSideNode["2"][0].as<float>(),uvsSideNode["2"][1].as<float>(),
							 0.5f + x,  0.5f + y,  0.5f + z,  uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							 0.5f + x,  0.5f + y,  0.5f + z,  uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							-0.5f + x,  0.5f + y,  0.5f + z,  uvsSideNode["1"][0].as<float>(),uvsSideNode["1"][1].as<float>(),
							-0.5f + x, -0.5f + y,  0.5f + z,  uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),

							//left
							-0.5f + x,  0.5f + y,  0.5f + z,   uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							-0.5f + x,  0.5f + y, -0.5f + z,   uvsSideNode["1"][0].as<float>(),uvsSideNode["1"][1].as<float>(),
							-0.5f + x, -0.5f + y, -0.5f + z,   uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							-0.5f + x, -0.5f + y, -0.5f + z,   uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							-0.5f + x, -0.5f + y,  0.5f + z,   uvsSideNode["2"][0].as<float>(),uvsSideNode["2"][1].as<float>(),
							-0.5f + x,  0.5f + y,  0.5f + z,   uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),

							//right
							 0.5f + x,  0.5f + y,  0.5f + z,   uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),
							 0.5f + x,  0.5f + y, -0.5f + z,   uvsSideNode["1"][0].as<float>(),uvsSideNode["1"][1].as<float>(),
							 0.5f + x, -0.5f + y, -0.5f + z,   uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							 0.5f + x, -0.5f + y, -0.5f + z,   uvsSideNode["0"][0].as<float>(),uvsSideNode["0"][1].as<float>(),
							 0.5f + x, -0.5f + y,  0.5f + z,   uvsSideNode["2"][0].as<float>(),uvsSideNode["2"][1].as<float>(),
							 0.5f + x,  0.5f + y,  0.5f + z,   uvsSideNode["3"][0].as<float>(),uvsSideNode["3"][1].as<float>(),

							 //bottom
							-0.5f + x, -0.5f + y, -0.5f + z,  uvsBottomNode["1"][0].as<float>(),uvsBottomNode["1"][1].as<float>(),
							 0.5f + x, -0.5f + y, -0.5f + z,  uvsBottomNode["3"][0].as<float>(),uvsBottomNode["3"][1].as<float>(),
							 0.5f + x, -0.5f + y,  0.5f + z,  uvsBottomNode["2"][0].as<float>(),uvsBottomNode["2"][1].as<float>(),
							 0.5f + x, -0.5f + y,  0.5f + z,  uvsBottomNode["2"][0].as<float>(),uvsBottomNode["2"][1].as<float>(),
							-0.5f + x, -0.5f + y,  0.5f + z,  uvsBottomNode["0"][0].as<float>(),uvsBottomNode["0"][1].as<float>(),
							-0.5f + x, -0.5f + y, -0.5f + z,  uvsBottomNode["1"][0].as<float>(),uvsBottomNode["1"][1].as<float>(),

							//top
							-0.5f + x,  0.5f + y, -0.5f + z,  uvsTopNode["1"][0].as<float>(),uvsTopNode["1"][1].as<float>(),
							 0.5f + x,  0.5f + y, -0.5f + z,  uvsTopNode["3"][0].as<float>(),uvsTopNode["3"][1].as<float>(),
							 0.5f + x,  0.5f + y,  0.5f + z,  uvsTopNode["2"][0].as<float>(),uvsTopNode["2"][1].as<float>(),
							 0.5f + x,  0.5f + y,  0.5f + z,  uvsTopNode["2"][0].as<float>(),uvsTopNode["2"][1].as<float>(),
							-0.5f + x,  0.5f + y,  0.5f + z,  uvsTopNode["0"][0].as<float>(),uvsTopNode["0"][1].as<float>(),
							-0.5f + x,  0.5f + y, -0.5f + z,  uvsTopNode["1"][0].as<float>(),uvsTopNode["1"][1].as<float>(),
						});
				}
			}
		}

		MCLONE_CREATE_VERTEX_BUFFER(vb, float);
		vb->setLayout({ 3,2 });
		vb->pushVertices(vertexData);
		vb->upload(false);
		chunk_vao->pushBuffer(std::move(vb));
		chunk_vao->upload();
		return chunk_vao;
	}
}