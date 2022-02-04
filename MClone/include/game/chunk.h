#pragma once
#include"graphics/vertex.h"
#include"glm/glm.hpp"
#include"yaml-cpp/yaml.h"

namespace game
{
	struct blockFormat
	{
		int id;
		std::string name,side, top, bottom;
	};

	struct textureFormat
	{
		std::string name;
		glm::vec2 uvs[4];
	};

	void loadBlockFormats();
	void loadTextureFormats();

	
	std::shared_ptr<graphics::VertexArray> createChunk(int width, int height, int breadth);
}