#include<iostream>
#include<filesystem>

#include"log.h"
#include"glad/glad.h"
#define SDL_MAIN_HANDLED
#include"sdl2/SDL.h"
#include"core/window.h"
#include"core/logmanager.h"

#include"input/mouse.h"
#include"input/keyboard.h"

#include"graphics/shader.h"


int main()
{
	core::LogManager logger;
	logger.initialize();

	MCLONE_INFO(" cwd:{}", std::filesystem::current_path().string());

	core::Window window;
	window.create(core::WindowProperties());

	input::Mouse::Init();
	input::Keyboard::Init();

	std::string vertexShaderSource = "#version 330 core\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";
	std::string fragmentShaderSource = "#version 330 core\n"
		"out vec4 FragColor;\n"
		"void main()\n"
		"{\n"
		"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
		"}\n\0";

	
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


	uint32_t vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	uint32_t vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) , vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	uint32_t ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	graphics::Shader shader("src/shaders/vertex.glsl", "src/shaders/fragmemt.glsl");

	while (!window.m_ShouldClose)
	{
		window.pollEvents();
		window.clearScreen();
		input::Mouse::Update();
		input::Keyboard::Update();
		
		shader.bind();
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
		
		window.swapbuffers();
	}
	window.shutdown();
	logger.shutdown();
	return 0;
}