#include<iostream>
#include<filesystem>
#include<memory>
#include"log.h"
#include"glad/glad.h"
#define SDL_MAIN_HANDLED
#include"sdl2/SDL.h"
#include"core/window.h"
#include"core/logmanager.h"

#include"input/mouse.h"
#include"input/keyboard.h"

#include"graphics/shader.h"
#include"graphics/vertex.h"
#include"graphics/texture.h"
#include"graphics/camera.h"

#include"game/chunk.h"

#include"utils/pngPacker.h"
#include"glm/gtx/string_cast.hpp"
namespace test
{
	void YamlWrite();
}
int main()
{	
	core::LogManager logger;
	logger.initialize();

	MCLONE_INFO(" cwd:{}", std::filesystem::current_path().string());
	
	core::Window window;
	window.create(core::WindowProperties());

	test::YamlWrite();
	input::Mouse::Init();
	input::Keyboard::Init();
	pngPacker::packPngs("res/Block/", "texturePack.png");


	auto chunk_vao = game::createChunk( 100,100,1);
	//auto chunk2_vao = game::createChunk(20, 100, 100);
	graphics::Texture texture("tex1", "texturePack.png", 1);
	graphics::Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

	texture.bind();
	shader.setUniformInt(texture.getName(), texture.getTexUnit());

	graphics::FlyCamera cam({ 0.0f,0.f,2.f }, window.getProps().aspectRatio);
	cam.setZFar(1000.f);
	cam.speed = 0.001f;
	shader.setUniformMat4("uView", cam.getViewMatrix());
	shader.setUniformMat4("uProj", cam.getProjMatrix());
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	while (!window.m_ShouldClose)
	{
		window.pollEvents();
		window.clearScreen();
		input::Mouse::Update();
		input::Keyboard::Update();

		texture.bind();
		shader.bind();

		cam.handleInput();
		shader.setUniformMat4("uView", cam.getViewMatrix());
		shader.setUniformMat4("uProj", cam.getProjMatrix());

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		chunk_vao->bind();
		glDrawArrays(GL_TRIANGLES, 0, chunk_vao->getVertexCount());
		//glDrawElements(GL_TRIANGLES,chunk_vao->getElementCount(),GL_UNSIGNED_INT,0);
		/*chunk2_vao->bind();
		glDrawArrays(GL_TRIANGLES, 0, chunk2_vao->getVertexCount());*/
		window.swapbuffers();
	}
	window.shutdown();
	logger.shutdown();
	return 0;
}