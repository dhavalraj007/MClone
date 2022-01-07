#include<iostream>

#include"glad/glad.h"
#define SDL_MAIN_HANDLED
#include"sdl2/SDL.h"
#include"core/window.h"
#include"input/mouse.h"
#include"input/keyboard.h"

int main()
{
	core::Window window;
	window.create(core::WindowProperties());
	input::Mouse::Init();
	input::Keyboard::Init();

	while (!window.m_ShouldClose)
	{
		window.pollEvents();
		window.clearScreen();
		input::Mouse::Update();
		input::Keyboard::Update();
		
		uint32_t vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		float data[] = {
			 0.5f,  0.5f, 0.f,      //top right
			 0.5f, -0.5f, 0.f,      //bottom right
			-0.5f, -0.5f, 0.f,       //bottom left
			-0.5f,  0.5f, 0.f      //top left
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
		
		window.swapbuffers();
	}
	window.shutdown();

	return 0;
}