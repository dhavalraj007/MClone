#include "core\window.h"
#include "core\globals.h"
#include"log.h"
#include<sdl2/SDL.h>
#include<glad/glad.h>
#include<iostream>
#include"graphics/camera.h"

namespace core
{
	WindowProperties::WindowProperties()
	{
		title = "MClone!";
		x = SDL_WINDOWPOS_CENTERED;
		y = SDL_WINDOWPOS_CENTERED;
		w = 1024;
		h = 576;
		wMin = 320;
		hMin = 180;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
		aspectRatio = 16.0f / 9.0f;
		clearColor = glm::vec4(153.0 / 255.0, 204.0 / 255.0, 1.0,1.0);
	}
	Window::Window()
		:m_Window(nullptr),
		m_Context(nullptr)
	{}

	void Window::create(const WindowProperties& props)
	{
		m_props = props;
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			std::cout << "error init SDL" << std::endl;
		}

		m_Window = SDL_CreateWindow(props.title.c_str(), props.x, props.y, props.w, props.h,props.flags);
		if (!m_Window)
		{
			std::cout << "error window creation" << std::endl;
		}
		SDL_SetWindowMinimumSize(m_Window, props.wMin, props.hMin);
		
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_WarpMouseInWindow(m_Window, props.w/2, props.h/2);
		SDL_SetHintWithPriority(SDL_HINT_MOUSE_RELATIVE_MODE_WARP, "1", SDL_HINT_OVERRIDE);
		SDL_SetRelativeMouseMode(SDL_TRUE);

		m_Context = SDL_GL_CreateContext(m_Window);

		int major, minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &minor);
		MCLONE_TRACE("GLcontext version {}.{}", major, minor);

		gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress);
		glViewport(0, 0, props.w, props.h);
		glClearColor(props.clearColor.r, props.clearColor.g, props.clearColor.b, props.clearColor.a);
		glEnable(GL_DEPTH_TEST);
	}

	void Window::shutdown()
	{
		SDL_DestroyWindow(m_Window);
		SDL_GL_DeleteContext(m_Context);
	}

	void Window::setClearColor(glm::vec4 cc)
	{
		m_props.clearColor = cc;
		glClearColor(cc.r, cc.g, cc.b, cc.a);
	}

	void Window::clearScreen()
	{
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	}

	void Window::swapbuffers()
	{
		SDL_GL_SwapWindow(m_Window);
	}

	void Window::pollEvents(graphics::FlyCamera& cam)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				m_ShouldClose = true;
				break;
			case SDL_KEYDOWN:
			{
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_ESCAPE:
					m_ShouldClose = true;
					break;
				default:
					break;
				}
			}
			break;
			case SDL_MOUSEMOTION:
			{
				if (globals::MouseFirst)
				{
					cam.handleMouseInput(globals::deltaTime, 0, 0);
					globals::MouseFirst = false;
				}
				else
					cam.handleMouseInput(globals::deltaTime, e.motion.xrel, -e.motion.yrel);
				break;
			}
			default:
				break;
			}
		}
		
	}

}

