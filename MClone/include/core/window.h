#pragma once
#include<string>
#include<glm/glm.hpp>

typedef struct SDL_Window SDL_Window;
typedef void* SDL_GLContext;
namespace core
{
	struct WindowProperties
	{
		std::string title;
		int x, y, w, h;
		int wMin, hMin;
		float aspectRatio;
		int flags;
		glm::vec4 clearColor;
		WindowProperties();
	};

	class Window
	{
	public:
		Window();
		~Window() {}
		void create(const WindowProperties& props);
		void shutdown();
		void setClearColor(glm::vec4 cc);
		void clearScreen();
		void swapbuffers();
		void pollEvents();
		inline WindowProperties getProps() const { return m_props; }
		bool m_ShouldClose = false;
	private:
		SDL_Window* m_Window;
		SDL_GLContext m_Context;
		WindowProperties m_props;
	}; 
}