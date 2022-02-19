#include<filesystem>

#include "core/engine.h"

#include"log.h"

#include"input/mouse.h"
#include"input/keyboard.h"

#include"sdl2/sdl.h"

#include"glad/glad.h"
#include"graphics/texture.h"
#include"graphics/mipTexture.h"
#include"graphics/shader.h"
#include"graphics/camera.h"

#include"game/chunkSystem.h"

#include"utils/pngPacker.h"

namespace core
{
	Engine::Engine()
	{
		m_logManager.initialize();
		MCLONE_INFO(" cwd:{}", std::filesystem::current_path().string());
		
		m_window.create(core::WindowProperties());
		input::Mouse::Init();
		input::Keyboard::Init();

		//pngPacker::packPngs("res/Block/", "texturePack0.png");
	}

	Engine::~Engine()
	{
		m_window.shutdown();
		m_logManager.shutdown();
	}

	void Engine::run()
	{
		game::ChunkSystem chunkSystem;
		for (int z = -8; z < 8; z++)
		{
			for (int x = -8; x < 8; x++)
			{
				chunkSystem.addChunk(x,z);
			}
		}
		graphics::Texture texture("tex1", "texturePack0.png", 1);


		
		graphics::Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

		texture.bind();
		shader.setUniformInt(texture.getName(), texture.getTexUnit());

		graphics::FlyCamera cam({ 0.0f,385.f,0.f }, m_window.getProps().aspectRatio);
		cam.fastspeed = 50.f;
		cam.setZFar(1000.f);
		shader.setUniformMat4("uView", cam.getViewMatrix());
		shader.setUniformMat4("uProj", cam.getProjMatrix());
		glEnable(GL_CULL_FACE);
		//glEnable(GL_BLEND);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		float deltaTime, lastFrameTime = 0;
		float currTime;

		while (!m_window.m_ShouldClose)
		{
			currTime = SDL_GetTicks() * 0.001f;
			deltaTime = currTime - lastFrameTime;
			lastFrameTime = currTime;
			//MCLONE_TRACE("{}", deltaTime);

			m_window.pollEvents();
			m_window.clearScreen();
			input::Mouse::Update();
			input::Keyboard::Update();

			texture.bind();
			shader.bind();

			cam.handleInput(deltaTime);
			shader.setUniformMat4("uView", cam.getViewMatrix());
			shader.setUniformMat4("uProj", cam.getProjMatrix());

			chunkSystem.render();
			m_window.swapbuffers();
		}
	}
}
