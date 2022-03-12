#pragma once
#include"core/window.h"

//gloabal variables decs
namespace globals
{
	extern core::WindowProperties windowProps;
	extern bool MouseFirst;
	extern float deltaTime;
	extern uint32_t frameCount;	// round to 0 after 2^32 -1
}
