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

	game::IDEmitterFunc IdEmitter = [](int x, int y, int z) {
		if (y < 10)
			return 2;
		else
			return 1;
	};

	void Engine::run()
	{
		game::ChunkSystem chunkSystem;
		chunkSystem.addChunk(1.f, 0.f, 0.f,IdEmitter);
		graphics::Texture texture("tex1", "texturePack0.png", 1);


		
		graphics::Shader shader("src/shaders/vertex.glsl", "src/shaders/fragment.glsl");

		texture.bind();
		shader.setUniformInt(texture.getName(), texture.getTexUnit());

		graphics::FlyCamera cam({ 0.0f,0.f,17.f }, m_window.getProps().aspectRatio);
		cam.setZFar(1000.f);
		shader.setUniformMat4("uView", cam.getViewMatrix());
		shader.setUniformMat4("uProj", cam.getProjMatrix());
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		float deltaTime, lastFrameTime = 0;
		float currTime;

		while (!m_window.m_ShouldClose)
		{
			currTime = SDL_GetTicks() * 0.001f;
			deltaTime = currTime - lastFrameTime;
			lastFrameTime = currTime;
			MCLONE_TRACE("{}", deltaTime);

			m_window.pollEvents();
			m_window.clearScreen();
			input::Mouse::Update();
			input::Keyboard::Update();

			texture.bind();
			shader.bind();

			cam.handleInput(deltaTime);
			shader.setUniformMat4("uView", cam.getViewMatrix());
			shader.setUniformMat4("uProj", cam.getProjMatrix());

			//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			chunkSystem.chunks[0].vao->bind();
			glDrawArrays(GL_TRIANGLES, 0, chunkSystem.chunks[0].vao->getVertexCount());
			//glDrawElements(GL_TRIANGLES,chunk_vao->getElementCount(),GL_UNSIGNED_INT,0);
			/*chunk2_vao->bind();
			glDrawArrays(GL_TRIANGLES, 0, chunk2_vao->getVertexCount());*/
			m_window.swapbuffers();
		}
	}
}
