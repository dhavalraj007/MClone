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
#include"game/lightSource.h"
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

		int gridSize = 8;
		for (int z = -gridSize; z < gridSize; z++)
		{
			for (int x = -gridSize; x < gridSize; x++)
			{
				std::shared_ptr<game::Chunk> chunk = std::make_shared<game::Chunk>(glm::ivec3{ x,0,z });
				chunkSystem.createDataForChunk(chunk);
			}
		}

		game::LightSource lightSource({ 0.f,200.f,0.f }, { 1.f, 1.f, 1.f });
		lightSource.setScale(5.f);
		graphics::Shader lightingShader("src/shaders/lightingVertex.glsl", "src/shaders/lightingFragment.glsl");
		


		graphics::Texture texture("tex1", "texturePack0.png", 1);
		texture.bind();
		lightingShader.setUniformInt(texture.getName(), texture.getTexUnit());



		graphics::FlyCamera cam({ 0.0f,385.f,0.f }, m_window.getProps().aspectRatio);
		cam.fastspeed = 50.f;
		cam.setZFar(1000.f);
		lightingShader.setUniformMat4("uView", cam.getViewMatrix());
		lightingShader.setUniformMat4("uProj", cam.getProjMatrix());
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


			if (input::Keyboard::Key(input::Keys::MKEY_UP))
				lightSource.setPos(lightSource.getPos() + glm::vec3{0.f, 1.f, 0.f});
			if (input::Keyboard::Key(input::Keys::MKEY_DOWN))
				lightSource.setPos(lightSource.getPos() + glm::vec3{ 0.f, -1.f, 0.f });
			if (input::Keyboard::Key(input::Keys::MKEY_RIGHT))
				lightSource.setPos(lightSource.getPos() + glm::vec3{ 1.f, 0.f, 0.f });
			if (input::Keyboard::Key(input::Keys::MKEY_LEFT))
				lightSource.setPos(lightSource.getPos() + glm::vec3{ -1.f, 0.f, 0.f });



			lightSource.shader->bind();
			lightSource.shader->setUniformMat4("uView", cam.getViewMatrix());
			lightSource.shader->setUniformMat4("uProj", cam.getProjMatrix());
			lightSource.render();

			texture.bind();
			lightingShader.bind();

			cam.handleInput(deltaTime);
			lightingShader.setUniformMat4("uView", cam.getViewMatrix());
			lightingShader.setUniformMat4("uProj", cam.getProjMatrix());
			lightingShader.setUniformFloat3("uLightPos", lightSource.getPos());
			lightingShader.setUniformFloat3("uLightColor", lightSource.getColor());

			for (auto chunk : chunkSystem.chunks)
			{
				chunk->vao->bind();
				lightingShader.setUniformInt3("uChunkPos", chunk->position);
				glDrawArrays(GL_TRIANGLES, 0, chunk->vao->getVertexCount());
			}

			
			m_window.swapbuffers();
		}
	}
}
