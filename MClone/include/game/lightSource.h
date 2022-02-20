#pragma once
#include"glm/glm.hpp"
#include"graphics/vertex.h"
#include"graphics/shader.h"

namespace game
{
	/// <summary>
	/// this only renders a cube at pos with color. you need to get the pos and color from here and pass it to lightingshader and you also must set view(uView) and projection(uProj) matrix in the game loop.
	/// this uses lightSource Shader file drawing a cube.
	/// </summary>
	class LightSource
	{
	public:
		LightSource(const glm::vec3& pos,const glm::vec3& color);
		void render();
		inline void setPos(const glm::vec3& pos) { lightPos = pos; shader->bind(); shader->setUniformFloat3("uLightPos", lightPos); }
		inline void setColor(const glm::vec3& col) { lightColor= col; shader->bind(); shader->setUniformFloat3("uLightColor", lightColor); }
		void setScale(float s);
		inline const glm::vec3& getPos() { return lightPos; }
		inline const glm::vec3& getColor() { return lightColor; }
		
		std::shared_ptr<graphics::Shader> shader;
	private:
		float scale;
		glm::vec3 lightPos;
		glm::vec3 lightColor;
		std::shared_ptr<graphics::VertexArray> vao;
		glm::mat4 scaleMat;
	};
}