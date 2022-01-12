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



int main()
{
	core::LogManager logger;
	logger.initialize();

	MCLONE_INFO(" cwd:{}", std::filesystem::current_path().string());

	core::Window window;
	window.create(core::WindowProperties());

	input::Mouse::Init();
	input::Keyboard::Init();

	float vertices[] = {
			 0.5f,  0.5f, 0.f,      //top right
			 0.5f, -0.5f, 0.f,      //bottom right
			-0.5f, -0.5f, 0.f,       //bottom left
			-0.5f,  0.5f, 0.f      //top left
	};
	uint8_t indices[] = {
		0,1,3,
		3,1,2
	};
	
	std::shared_ptr<graphics::VertexArray> vao = std::make_shared<graphics::VertexArray>();
	{
		MCLONE_CREATE_VERTEX_BUFFER(vb, float);
		vb->pushVertex({  1.0f,1.0f, 0.5f,  0.5f, 0.f });   //top right
		vb->pushVertex({  1.0f,0.0f, 0.5f, -0.5f, 0.f });    //bottom right
		vb->pushVertex({  0.0f,0.0f,-0.5f, -0.5f, 0.f });   //bottom left
		vb->pushVertex({  0.0f,1.0f,-0.5f,  0.5f, 0.f });   //top left
		vb->setLayout({ 2,3 });
		vb->upload(false);
		vao->pushBuffer(std::move(vb));
	}
	//{
	//	MCLONE_CREATE_VERTEX_BUFFER(vb, float);
	//	vb->pushVertex({  0.5f,  0.5f, 0.f });   //top right
	//	vb->pushVertex({ 0.5f, -0.5f, 0.f });    //bottom right
	//	vb->pushVertex({ -0.5f, -0.5f, 0.f });   //bottom left
	//	vb->pushVertex({ -0.5f,  0.5f, 0.f });   //top left
	//	vb->setLayout({ 3 });
	//	vb->upload(false);
	//	vao->pushBuffer(std::move(vb));
	//}
	//{
	//	MCLONE_CREATE_VERTEX_BUFFER(vb, float);
	//	vb->pushVertex({ 1.0f,1.0f });
	//	vb->pushVertex({ 1.0f,0.0f });
	//	vb->pushVertex({ 0.0f,0.0f });
	//	vb->pushVertex({ 0.0f,1.0f });
	//	vb->setLayout({ 2 });
	//	vb->upload(false);
	//	vao->pushBuffer(std::move(vb));
	//}
	vao->setElements({ 0,1,3,3,1,2 });
	vao->upload();
	
	graphics::Texture texture("tex1", "res/container.jpg", 1);

	graphics::Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

	texture.bind();
	shader.setUniformInt(texture.getName(), texture.getTexUnit());

	while (!window.m_ShouldClose)
	{
		window.pollEvents();
		window.clearScreen();
		input::Mouse::Update();
		input::Keyboard::Update();
		
		vao->bind();
		texture.bind();
		shader.bind();

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		window.swapbuffers();
	}
	window.shutdown();
	logger.shutdown();
	return 0;
}