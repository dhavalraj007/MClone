#pragma once
#include"core/logmanager.h"
#include"core/window.h"


namespace core
{
	class Engine
	{
	public:
		Engine();
		~Engine();


		void run();
	private:
		core::LogManager m_logManager;
		core::Window m_window;
	};
}