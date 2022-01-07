#pragma once
#include<array>
#include<sdl2/SDL_mouse.h>
namespace input
{
	enum class buttons;
	class Mouse
	{
	public:

		static void Init();
		static void Update();

		static bool Button(buttons button);
		static bool ButtonUp(buttons button);
		static bool ButtonDown(buttons button);
		static int x,xLast;
		static int y,yLast;
	private:
		constexpr static const int ButtonCount = 5;
		static std::array<bool, ButtonCount> m_CurrButtonState;
		static std::array<bool, ButtonCount> m_LastButtonState;
	};

	enum class buttons
	{
		MBUTTON_LEFT = 1,
		MBUTTON_MIDDLE = 2,
		MBUTTON_RIGHT = 3,
		MBUTTON_X1 = 4,
		MBUTTON_X2 = 5
	};

}