#include"game/lightSource.h"
#include"glad/glad.h"
#include "glm\gtc\matrix_transform.hpp"

namespace game {
	
	LightSource::LightSource(const glm::vec3& pos, const glm::vec3& color)
	{
		std::vector<float> vertexData{  // Back face
    -0.5f, -0.5f, -0.5f, // Bottom-left
     0.5f,  0.5f, -0.5f, // top-right
     0.5f, -0.5f, -0.5f, // bottom-right         
     0.5f,  0.5f, -0.5f, // top-right
    -0.5f, -0.5f, -0.5f, // bottom-left
    -0.5f,  0.5f, -0.5f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f, // bottom-left
     0.5f, -0.5f,  0.5f, // bottom-right
     0.5f,  0.5f,  0.5f, // top-right
     0.5f,  0.5f,  0.5f, // top-right
    -0.5f,  0.5f,  0.5f, // top-left
    -0.5f, -0.5f,  0.5f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f, // top-right
    -0.5f,  0.5f, -0.5f, // top-left
    -0.5f, -0.5f, -0.5f, // bottom-left
    -0.5f, -0.5f, -0.5f, // bottom-left
    -0.5f, -0.5f,  0.5f, // bottom-right
    -0.5f,  0.5f,  0.5f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f, // top-left
     0.5f, -0.5f, -0.5f, // bottom-right
     0.5f,  0.5f, -0.5f, // top-right         
     0.5f, -0.5f, -0.5f, // bottom-right
     0.5f,  0.5f,  0.5f, // top-left
     0.5f, -0.5f,  0.5f, // bottom-left     
    // Bottom face
    -0.5f, -0.5f, -0.5f, // top-right
     0.5f, -0.5f, -0.5f, // top-left
     0.5f, -0.5f,  0.5f, // bottom-left
     0.5f, -0.5f,  0.5f, // bottom-left
    -0.5f, -0.5f,  0.5f, // bottom-right
    -0.5f, -0.5f, -0.5f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f, // top-left
     0.5f,  0.5f,  0.5f, // bottom-right
     0.5f,  0.5f, -0.5f, // top-right     
     0.5f,  0.5f,  0.5f, // bottom-right
    -0.5f,  0.5f, -0.5f, // top-left
    -0.5f,  0.5f,  0.5f // bottom-left
        };
            
        vao = std::make_shared<graphics::VertexArray>();
		MCLONE_CREATE_VERTEX_BUFFER(vb, float);
        vb->setLayout({ 3 });
        vb->pushVertices(vertexData);
        vb->upload(false);
        vao->pushBuffer(std::move(vb));
        vao->upload();

        shader = std::make_shared<graphics::Shader>("src/shaders/lightSourceVertex.glsl", "src/shaders/lightSourceFragment.glsl");
        setPos(pos);
        setColor(color);
        setScale(1.f);
	}
    void LightSource::render()
    {
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    void LightSource::setScale(float s)
    {
        scale = s;
        scaleMat = glm::mat4(1.f);
        scaleMat = glm::scale(scaleMat, { s,s,s });
        shader->setUniformMat4("uScaleMat", scaleMat);
    }
}