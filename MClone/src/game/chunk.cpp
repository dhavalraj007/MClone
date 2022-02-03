#include"graphics/vertex.h"
#include"glm/glm.hpp"

namespace game
{
	std::vector<float> genCube(const glm::vec3& pos)
	{
		std::vector<float> verts
		{
			//pos					 					  //uvs

			//back
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  0.0f, 0.0f,
			 0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  1.0f, 0.0f,
			 0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  1.0f, 1.0f,
			 0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  1.0f, 1.0f,
			-0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  0.0f, 1.0f,
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  0.0f, 0.0f,

			//front
			-0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  0.0f, 0.0f,
			 0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  1.0f, 0.0f,
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  1.0f, 1.0f,
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  1.0f, 1.0f,
			-0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  0.0f, 1.0f,
			-0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  0.0f, 0.0f,

			//left
			-0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,   1.0f, 1.0f,
			-0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,   0.0f, 1.0f,
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,   0.0f, 0.0f,
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,   0.0f, 0.0f,
			-0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,   1.0f, 0.0f,
			-0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,   1.0f, 1.0f,

			//right
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,   1.0f, 1.0f,
			 0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,   0.0f, 1.0f,
			 0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,   0.0f, 0.0f,
			 0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,   0.0f, 0.0f,
			 0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,   1.0f, 0.0f,
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,   1.0f, 1.0f,

			 //bottom
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  0.0f, 1.0f,
			 0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  1.0f, 1.0f,
			 0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  1.0f, 0.0f,
			 0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  1.0f, 0.0f,
			-0.5f + pos.x, -0.5f + pos.y,  0.5f + pos.z,  0.0f, 0.0f,
			-0.5f + pos.x, -0.5f + pos.y, -0.5f + pos.z,  0.0f, 1.0f,

			//top
			-0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  0.0f, 1.0f,
			 0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  1.0f, 1.0f,
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  1.0f, 0.0f,
			 0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  1.0f, 0.0f,
			-0.5f + pos.x,  0.5f + pos.y,  0.5f + pos.z,  0.0f, 0.0f,
			-0.5f + pos.x,  0.5f + pos.y, -0.5f + pos.z,  0.0f, 1.0f
		};
		return (verts);
	}

	std::shared_ptr<graphics::VertexArray> createChunk(const glm::vec3& pos, int width, int height, int breadth)
	{
		std::shared_ptr<graphics::VertexArray> chunk_vao = std::make_shared<graphics::VertexArray>();

		std::vector<float> verts;
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				for (int z = 0; z < breadth; z++)
				{
					auto cpos = genCube({ pos.x+x,pos.y+y,pos.z+z });
					verts.insert(verts.end(), cpos.begin(), cpos.end());
				}
			}
		}

		MCLONE_CREATE_VERTEX_BUFFER(vb, float);
		vb->setLayout({ 3,2 });
		vb->pushVertices(verts);
		vb->upload(false);
		chunk_vao->pushBuffer(std::move(vb));
		chunk_vao->upload();
		return chunk_vao;
	}
}