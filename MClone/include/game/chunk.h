#pragma once
#include"graphics/vertex.h"
#include"glm/glm.hpp"
namespace game
{
	std::shared_ptr<graphics::VertexArray> createChunk(const glm::vec3& pos, int width, int height, int breadth);
}