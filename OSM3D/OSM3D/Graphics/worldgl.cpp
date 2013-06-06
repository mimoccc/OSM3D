
#include "worldgl.h"
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <windows.h>
#include <iostream>

#include "../Core/Debug.h"

void WorldGL::ConstructFrameworks ()
{
	SDL_Init (SDL_INIT_EVERYTHING);
	IMG_Init (IMG_INIT_PNG | IMG_INIT_JPG);
	TTF_Init ();
}

void WorldGL::DestructFrameworks ()
{
	SDL_Quit ();
	IMG_Quit ();
	TTF_Quit ();
}

WorldGL::WorldGL ()
{
	m_Window_Width = 800;
	m_Window_Height = 600;
	m_Window_FullscreenWidth = GetSystemMetrics (SM_CXSCREEN);
	m_Window_FullscreenHeight = GetSystemMetrics (SM_CYSCREEN);
	m_Window_IsOpen = false;
	m_Window_IsFullscreen = false;
	m_Window_VerticalSync = false;

	m_Timer_UseAccurateFramerateLimiting = true;

	m_Timer_Start = Timer_GetTime ();
	m_Timer_FrameCount = 0;
	m_Timer_FrameTrackStart = 0;
	m_Timer_FPS = 0;
	m_Timer_UpdatedFPS = false;

	m_Timer_StartHR = Timer_GetTimeHR ();

	m_FpsCap = 60;

	m_Window_LastRenderTickHR = 0;

	m_InputContainer.nullKeys ();
	m_InputContainer.nullMouse ();
	m_InputContainer.nullMouseButton ();
	m_Input_ScrollAmount = 0;

	m_KeyUpdateAmount = 0;

	SDL_GetMouseState (&(m_InputContainer.m_MouseX), &(m_InputContainer.m_MouseY));

	//SDL_ShowCursor (false);
}

WorldGL::~WorldGL ()
{
	if (m_Window_IsOpen)
		Window_Destroy ();
}

void WorldGL::Window_Create ()
{
	SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, (int)m_Window_VerticalSync);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 0);
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 0);

	SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute (SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute (SDL_GL_BUFFER_SIZE, 32);

	if (m_Window_IsFullscreen)
	{
		m_SDL_pRenderSurface =
			SDL_SetVideoMode (m_Window_FullscreenWidth, m_Window_FullscreenHeight, 32, SDL_OPENGL | SDL_FULLSCREEN);
	}
	else
	{
		m_SDL_pRenderSurface =
			SDL_SetVideoMode (m_Window_Width, m_Window_Height, 32, SDL_OPENGL);	
	}

	m_Window_IsOpen = true;
}

void WorldGL::Window_Destroy ()
{
	SDL_FreeSurface (m_SDL_pRenderSurface);
	m_Window_IsOpen = false;
}

void WorldGL::Window_SetWidth (int width)
{
	m_Window_Width = width;
}

void WorldGL::Window_SetHeight (int height)
{
	m_Window_Height = height;
}

int WorldGL::Window_GetWidth ()
{
	return (m_Window_IsFullscreen?m_Window_FullscreenWidth:m_Window_Width);
}

int WorldGL::Window_GetHeight ()
{
	return (m_Window_IsFullscreen?m_Window_FullscreenHeight:m_Window_Height);
}

bool WorldGL::Window_IsOpen ()
{
	SDL_PumpEvents ();
	return m_Window_IsOpen;
}

void WorldGL::Window_Close ()
{
	m_Window_IsOpen = false;
}

void WorldGL::Window_Refresh ()
{
	//	Try to Sleep instead of going into the SDL's CPU-heavy while(true) loop
	//	TODO: A different method of framerate limiting should be applied when vertical-sync
	//		is enabled (TODO TODO: Implement vertical sync toggling)
	if (m_Window_LastRenderTickHR != 0 && m_FpsCap != -1)
	{
		ubigint timePerFrame = (ubigint)(1.0 / Timer_GetResolutionHR ()) / m_FpsCap;

		ubigint curTicks = Timer_GetTimeHR ();
		//	Only sleep if we've got some leftover time to spend
		if (curTicks - m_Window_LastRenderTickHR < timePerFrame)
		{
			ubigint cpuTicks = (ubigint)(timePerFrame - (curTicks - m_Window_LastRenderTickHR));
			int sleepTime = (int)(cpuTicks * Timer_GetResolutionHR () * 1000.0);
			Sleep (sleepTime);
		}

		
		//	Spin while we wait for the rest of the time to get chewed up (yes, by the time we're
		//		done here we'll still need to SDL_GL_SwapBuffers, but that's not significant)
		while (Timer_GetTimeHR () - m_Window_LastRenderTickHR < timePerFrame && m_Timer_UseAccurateFramerateLimiting)
		{
		}
	}

	SDL_GL_SwapBuffers ();
	m_Timer_FrameCount++;

	ubigint ticks = Timer_GetTimeHR ();
	m_Window_LastRenderTickHR = ticks;

	if (m_Timer_FrameTrackStart == 0)
		m_Timer_FrameTrackStart = ticks;

	if (Timer_GetTime () - m_Timer_FrameTrackStart >= 1000)
	{
		m_Timer_FPS = m_Timer_FrameCount;
		m_Timer_FrameCount = 0;

		m_Timer_FrameTrackStart = Timer_GetTime ();

		m_Timer_UpdatedFPS = true;
	}

	//	Reset key press/release states
	for (unsigned int i = 0; i < m_KeyUpdateAmount; i++)
	{
		switch (m_KeyUpdateList [i].m_Update)
		{
		case (KeyUpdate::UPDATE_DOWN):	m_InputContainer.m_KeyState [m_KeyUpdateList [i].m_Key].down = false;	break;
		case (KeyUpdate::UPDATE_UP):	m_InputContainer.m_KeyState [m_KeyUpdateList [i].m_Key].up	 = false;	break;
		}
	}
	m_KeyUpdateAmount = 0;

	//	Reset mouse button states as well
	m_InputContainer.m_LeftMouseButton.down = false;
	m_InputContainer.m_LeftMouseButton.up = false;

	m_InputContainer.m_MiddleMouseButton.down = false;
	m_InputContainer.m_MiddleMouseButton.up = false;

	m_InputContainer.m_RightMouseButton.down = false;
	m_InputContainer.m_RightMouseButton.up = false;

	m_Input_ScrollAmount = 0;

	m_Events.clear ();
}

void WorldGL::Window_DisplayMouse (bool display)
{
	SDL_ShowCursor (display);
}

void WorldGL::Window_SetFullscreen (bool fullscreen)
{
	m_Window_IsFullscreen = fullscreen;
}

bool WorldGL::Window_GetFullscreen ()
{
	return m_Window_IsFullscreen;
}

void WorldGL::Window_ApplySettings ()
{
	Window_Create ();
}

void WorldGL::Window_SetMaxFPS (unsigned int maxFps)
{
	m_FpsCap = maxFps;
}

unsigned int WorldGL::Window_GetMaxFPS ()
{
	return m_FpsCap;
}

void WorldGL::Window_SetUseVsync (bool use)
{
	m_Window_VerticalSync = use;
}

int WorldGL::Timer_GetDelta ()
{
	int prevTicks = m_Timer_Start;
	m_Timer_Start = Timer_GetTime ();
	return m_Timer_Start - prevTicks;
}

int WorldGL::Timer_GetFPS ()
{
	return m_Timer_FPS;
}

bool WorldGL::Timer_UpdatedFPS ()
{
	bool updated = m_Timer_UpdatedFPS;
	if (updated)
		m_Timer_UpdatedFPS = false;
	return updated;
}

int WorldGL::Timer_GetTime ()
{
	return SDL_GetTicks ();
}

double WorldGL::Timer_GetResolution ()
{
	//	SDL timer hits 1000 ticks per second, or 1ms
	return 1.0 / 1000.0;
}

void WorldGL::Timer_UseAccurateFramerateLimiting (bool use)
{
	m_Timer_UseAccurateFramerateLimiting = use;
}

bool WorldGL::Timer_DoesUseAccurateFramerateLimiting ()
{
	return m_Timer_UseAccurateFramerateLimiting;
}

ubigint WorldGL::Timer_GetDeltaHR ()
{
	ubigint prevTime = m_Timer_StartHR;
	m_Timer_StartHR = Timer_GetTimeHR ();
	return m_Timer_StartHR - prevTime;
}

ubigint WorldGL::Timer_GetTimeHR ()
{
	LARGE_INTEGER time;
	QueryPerformanceCounter (&time);
	return time.QuadPart;
}

double WorldGL::Timer_GetResolutionHR ()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency (&freq);
	return 1.0 / (double)freq.QuadPart;
}

bool WorldGL::Input_GetKeyState (SDLKey key)
{
	return m_InputContainer.m_KeyState [key].active;
}

bool WorldGL::Input_GetKeyDown (SDLKey key)
{
	return m_InputContainer.m_KeyState [key].down;
}

bool WorldGL::Input_GetKeyUp (SDLKey key)
{
	return m_InputContainer.m_KeyState [key].up;
}

int WorldGL::Input_GetMouseX ()
{
	return m_InputContainer.m_MouseX;
}

int WorldGL::Input_GetMouseY ()
{
	return m_InputContainer.m_MouseY;
}

void WorldGL::Input_GetMousePos (int * x, int * y)
{
	*x = m_InputContainer.m_MouseX;
	*y = m_InputContainer.m_MouseY;
}

void WorldGL::Input_SetMousePos (int x, int y)
{
	SDL_WarpMouse (x, y);
}

bool WorldGL::Input_GetLMBState ()
{
	return m_InputContainer.m_LeftMouseButton.active;
}

bool WorldGL::Input_GetMMBState ()
{
	return m_InputContainer.m_MiddleMouseButton.active;
}

bool WorldGL::Input_GetRMBState ()
{
	return m_InputContainer.m_RightMouseButton.active;
}

void WorldGL::Input_ProcessEvent( const SDL_Event & Event )
{
	m_Events.push_back (Event);

	switch (Event.type)
	{
	case (SDL_KEYDOWN):
		{
			if (m_KeyUpdateAmount > KEY_UPDATE_QUEUE_MAX)
				Debug::Warning("WorldGL::Input_ProcessEvent | Unable to process key event; too many event received at once. (m_KeyUpdateAmount > KEY_UPDATE_QUEUE_MAX");

			SDLKey key = Event.key.keysym.sym;
			m_InputContainer.m_KeyState [key].active = true;
			m_InputContainer.m_KeyState [key].down = true;
			m_KeyUpdateList [m_KeyUpdateAmount].m_Key = key;
			m_KeyUpdateList [m_KeyUpdateAmount].m_Update = KeyUpdate::UPDATE_DOWN;

			m_KeyUpdateAmount++;
		}
		break;

	case (SDL_KEYUP):
		{
			SDLKey key = Event.key.keysym.sym;
			m_InputContainer.m_KeyState [key].active = false;
			m_InputContainer.m_KeyState [key].up = true;
			m_KeyUpdateList [m_KeyUpdateAmount].m_Key = key;
			m_KeyUpdateList [m_KeyUpdateAmount].m_Update = KeyUpdate::UPDATE_UP;

			m_KeyUpdateAmount++;
		}
		break;

	case (SDL_MOUSEMOTION):
		{
			m_InputContainer.m_MouseX = Event.motion.x;
			m_InputContainer.m_MouseY = Event.motion.y;
		}
		break;

	case (SDL_MOUSEBUTTONDOWN):
		{
			switch (Event.button.button)
			{
				case (SDL_BUTTON_LEFT):	
					m_InputContainer.m_LeftMouseButton.active = true;
					m_InputContainer.m_LeftMouseButton.down = true;
					break;

				case (SDL_BUTTON_MIDDLE):
					m_InputContainer.m_MiddleMouseButton.active = true;
					m_InputContainer.m_MiddleMouseButton.down = true;
					break;

				case (SDL_BUTTON_RIGHT):
					m_InputContainer.m_RightMouseButton.active = true;
					m_InputContainer.m_RightMouseButton.down = true;
					break;

				case (SDL_BUTTON_WHEELDOWN):
					m_Input_ScrollAmount--;
					break;
			}
			break;
		}

	case (SDL_MOUSEBUTTONUP):
		{
			switch (Event.button.button)
			{
				//	TODO: Implement access for press/release events on mouse buttons
				case (SDL_BUTTON_LEFT):	
					m_InputContainer.m_LeftMouseButton.active = false;
					m_InputContainer.m_LeftMouseButton.up = true;
					break;

				case (SDL_BUTTON_MIDDLE):
					m_InputContainer.m_MiddleMouseButton.active = false;
					m_InputContainer.m_MiddleMouseButton.up = true;
					break;

				case (SDL_BUTTON_RIGHT):
					m_InputContainer.m_RightMouseButton.active = false;
					m_InputContainer.m_RightMouseButton.up = true;
					break;

				case (SDL_BUTTON_WHEELUP):
					m_Input_ScrollAmount++;
					break;
			}
			break;
		}

	case (SDL_QUIT):
		{
			Window_Close ();
			break;
		}
	}
}

bool WorldGL::Input_GetLMBUp ()
{
	return m_InputContainer.m_LeftMouseButton.up;
}

bool WorldGL::Input_GetLMBDown ()
{
	return m_InputContainer.m_LeftMouseButton.down;
}

bool WorldGL::Input_GetMMBUp ()
{
	return m_InputContainer.m_MiddleMouseButton.up;
}

bool WorldGL::Input_GetMMBDown ()
{
	return m_InputContainer.m_MiddleMouseButton.down;
}

bool WorldGL::Input_GetRMBUp ()
{
	return m_InputContainer.m_RightMouseButton.up;
}

bool WorldGL::Input_GetRMBDown ()
{
	return m_InputContainer.m_RightMouseButton.down;
}

int WorldGL::Input_GetMouseScroll ()
{
	return m_Input_ScrollAmount;
}

const std::vector <SDL_Event> & WorldGL::Input_GetEvents ()
{
	return m_Events;
}