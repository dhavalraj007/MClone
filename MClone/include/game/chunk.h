#pragma once
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"

namespace game
{
	//0 is NULL_BLOCK
	struct blockFormat
	{
		int id;
		std::string name,side, top, bottom;
		bool isTransparent;
		blockFormat()
			:id(0), name("null"), side(""), top(""), bottom(""), isTransparent(true) {}
	};

	struct textureFormat
	{
		std::string name;
		glm::vec2 uvs[4];
	};

	void loadBlockFormats();
	void loadTextureFormats();

	
	std::shared_ptr<graphics::VertexArray> createChunk();
}