#include "input\mouse.h"

namespace input
{
	int Mouse::x = 0;
	int Mouse::y = 0;
	int Mouse::xLast = 0;
	int Mouse::yLast = 0;
	std::array<bool, Mouse::ButtonCount> Mouse::m_CurrButtonState;
	std::array<bool, Mouse::ButtonCount> Mouse::m_LastButtonState;
	void Mouse::Init()
	{
		std::fill(m_CurrButtonState.begin(), m_CurrButtonState.end(), false);
		std::fill(m_LastButtonState.begin(), m_LastButtonState.end(), false);
	}

	void Mouse::Update()
	{
		xLast = x;
		yLast = y;
		uint32_t state = SDL_GetMouseState(&x, &y);

		for (int i=0;i<ButtonCount;i++)
		{
			m_LastButtonState[i] = m_CurrButtonState[i];
			m_CurrButtonState[i] = state & SDL_BUTTON(i+1);
		}
	}

	bool Mouse::Button(buttons button)
	{
		return m_CurrButtonState[static_cast<int>(button)-1];
	}

	bool Mouse::ButtonUp(buttons button)
	{
		return m_LastButtonState[static_cast<int>(button)-1] && !m_CurrButtonState[static_cast<int>(button)-1];
	}

	bool Mouse::ButtonDown(buttons button)
	{
		return !m_LastButtonState[static_cast<int>(button)-1] && m_CurrButtonState[static_cast<int>(button)-1];
	}
}