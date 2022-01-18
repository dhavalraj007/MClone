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

#include"utils/pngPacker.h"

#include"glm/gtx/string_cast.hpp"

int main()
{
	
	core::LogManager logger;
	logger.initialize();

	MCLONE_INFO(" cwd:{}", std::filesystem::current_path().string());

	core::Window window;
	window.create(core::WindowProperties());

	input::Mouse::Init();
	input::Keyboard::Init();

	//pngPacker::packPngs("res/block", "testPack.png");

	
	graphics::Camera cam({ 0.0f,0.f,2.f },window.getProps().aspectRatio);

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

	MCLONE_TRACE("view:{}\n\nproj:{}", glm::to_string(cam.getViewMatrix()), glm::to_string(cam.getProjMatrix()))

	/*glm::mat4 view(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -1.0f));
	glm::mat4 proj(1.0f);*/
	//glm::mat4 proj= glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
	//MCLONE_TRACE("view:{}\n\nproj:{}", glm::to_string(view), glm::to_string(proj));
	shader.setUniformMat4("uView", cam.getViewMatrix());
	shader.setUniformMat4("uProj", cam.getProjMatrix());
	glm::vec3 pos = cam.getPos();
	glm::vec3 orientation(0.0f);
	while (!window.m_ShouldClose)
	{
		window.pollEvents();
		window.clearScreen();
		input::Mouse::Update();
		input::Keyboard::Update();
		
		vao->bind();
		texture.bind();
		shader.bind();
		float speed = 0.0001f;
		if (input::Keyboard::Key(input::Keys::MKEY_P))
		{
			if (input::Keyboard::Key(input::Keys::MKEY_Q))
			{
				pos.x += speed;
				cam.setPos(pos);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_E))
			{
				pos.x -= speed;
				cam.setPos(pos);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_A))
			{
				pos.y += speed;
				cam.setPos(pos);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_D))
			{
				pos.y -= speed;
				cam.setPos(pos);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_Z))
			{
				pos.z += speed;
				cam.setPos(pos);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_C))
			{
				pos.z -= speed;
				cam.setPos(pos);
			}
			MCLONE_TRACE("{},{},{}", pos.x, pos.y, pos.z);
		}
		if (input::Keyboard::Key(input::Keys::MKEY_O))
		{
			if (input::Keyboard::Key(input::Keys::MKEY_Q))
			{
				orientation.x += speed;
				cam.setOrientation(orientation);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_E))
			{
				orientation.x -= speed;
				cam.setOrientation(orientation);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_A))
			{
				orientation.y += speed;
				cam.setOrientation(orientation);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_D))
			{
				orientation.y -= speed;
				cam.setOrientation(orientation);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_Z))
			{
				orientation.z += speed;
				cam.setOrientation(orientation);
			}
			if (input::Keyboard::Key(input::Keys::MKEY_C))
			{
				orientation.z -= speed;
				cam.setOrientation(orientation);
			}
		}
		shader.setUniformMat4("uView", cam.getViewMatrix());
		shader.setUniformMat4("uProj", cam.getProjMatrix());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		window.swapbuffers();
	}
	window.shutdown();
	logger.shutdown();
	return 0;
}