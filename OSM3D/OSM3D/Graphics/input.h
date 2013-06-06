
#ifndef _INPUT_CONTAINER_H_
#define _INPUT_CONTAINER_H_

#include <SDL/SDL.h>

struct InputContainer
{
	struct ButtonInfo {
		bool active, down, up;
	};

	void nullMouse ();
	void nullMouseButton ();
	void nullKeys ();

	int m_MouseX, m_MouseY;
	ButtonInfo m_LeftMouseButton, m_MiddleMouseButton, m_RightMouseButton;
	ButtonInfo m_KeyState [SDLK_LAST];	//	Don't use keyState [0] for anything
};

struct KeyUpdate {
	SDLKey m_Key;
	enum {
		UPDATE_DOWN,
		UPDATE_UP
	} m_Update;
};

#endif