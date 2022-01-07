#pragma once

namespace core
{
	class LogManager
	{
	public:
		LogManager() = default;
		~LogManager() = default;

		void initialize();
		void shutdown();
	};
}