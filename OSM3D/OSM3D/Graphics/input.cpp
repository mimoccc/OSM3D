
#include "input.h"

void InputContainer::nullKeys ()
{
	for (int i = 0; i < SDLK_LAST; i++)
	{
		m_KeyState [i].active = false;
		m_KeyState [i].down = false;
		m_KeyState [i].up = false;
	}
}

void InputContainer::nullMouse ()
{
	m_MouseX = 0;
	m_MouseY = 0;
}

void InputContainer::nullMouseButton ()
{
	m_LeftMouseButton.active = false;
	m_LeftMouseButton.down = false;
	m_LeftMouseButton.up = false;

	m_MiddleMouseButton.active = false;
	m_MiddleMouseButton.down = false;
	m_MiddleMouseButton.up = false;

	m_RightMouseButton.active = false;
	m_RightMouseButton.down = false;
	m_RightMouseButton.up = false;
}