#include "input\Keyboard.h"

namespace input
{
	std::array<bool, Keyboard::KeyCount> Keyboard::m_CurrKeyState;
	std::array<bool, Keyboard::KeyCount> Keyboard::m_LastKeyState;
	void Keyboard::Init()
	{
		std::fill(m_CurrKeyState.begin(), m_CurrKeyState.end(), false);
		std::fill(m_LastKeyState.begin(), m_LastKeyState.end(), false);
	}

	void Keyboard::Update()
	{
		m_LastKeyState = m_CurrKeyState;
		const uint8_t* state = SDL_GetKeyboardState(nullptr);
		for (int i = static_cast<int>(Keys::MKEY_FIRST); i < KeyCount; i++)
		{
			m_CurrKeyState[i] = state[i];
		}
	}

	bool Keyboard::Key(Keys Key)
	{
		return m_CurrKeyState[static_cast<int>(Key)];
	}

	bool Keyboard::KeyUp(Keys Key)
	{
		return m_LastKeyState[static_cast<int>(Key)] && !m_CurrKeyState[static_cast<int>(Key)];
	}

	bool Keyboard::KeyDown(Keys Key)
	{
		return !m_LastKeyState[static_cast<int>(Key)] && m_CurrKeyState[static_cast<int>(Key)];
	}
}