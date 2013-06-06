
#ifndef _WORLDGL_H_
#define _WORLDGL_H_

/* Used for window management and easy-access input functions (i.e. Input_GetKeyState). Maintains SDL, IMG, and SDL_ttf
		library components */

#if defined (_WIN32) || defined (_MSC_VER) || defined (WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <SDL/SDL.h>
#endif

#include "../Common/Type.h"
#include "input.h"

#define KEY_UPDATE_QUEUE_MAX 256


class WorldGL
{
	int	m_Window_Width,
		m_Window_Height,
		m_Window_FullscreenWidth,
		m_Window_FullscreenHeight,
		m_Timer_Start,
		m_Input_ScrollAmount;

	unsigned int
		m_Timer_FrameCount,
		m_Timer_FPS;

	ubigint
		m_Timer_StartHR,
		m_Timer_FrameTrackStart,
		m_Window_LastRenderTickHR;

	bool
		m_Window_IsOpen,
		m_Timer_UpdatedFPS,
		m_Window_IsFullscreen,
		m_Timer_UseAccurateFramerateLimiting,
		m_Window_VerticalSync;

	SDL_Surface 
		* m_SDL_pRenderSurface;

	InputContainer
		m_InputContainer;

	KeyUpdate
		m_KeyUpdateList [KEY_UPDATE_QUEUE_MAX];

	unsigned int
		m_KeyUpdateAmount;

	unsigned int
		m_FpsCap;

	std::vector <SDL_Event>
		m_Events;

public:

	//	Set up/shut down the systems at work behind the WorldGL code
	static void	ConstructFrameworks ();
	static void	DestructFrameworks ();

			WorldGL ();
			~WorldGL ();
	
	void	Window_Create ();
	void	Window_Destroy ();

	void	Window_SetWidth (int width);
	void	Window_SetHeight (int height);
	int		Window_GetWidth ();
	int		Window_GetHeight();
	bool	Window_IsOpen ();
	void	Window_Close ();
	void	Window_Refresh ();
	void	Window_DisplayMouse (bool display);
	void	Window_SetMaxFPS (unsigned int maxFps);
	unsigned int Window_GetMaxFPS ();
	void	Window_SetUseVsync (bool use);

	void	Window_SetFullscreen (bool fullscreen);
	bool	Window_GetFullscreen ();

	void	Window_ApplySettings ();

	int		Timer_GetFPS ();
	bool	Timer_UpdatedFPS ();
	int		Timer_GetDelta ();
	int		Timer_GetTime ();
	//	Resolution, in seconds
	double	Timer_GetResolution ();

	void	Timer_UseAccurateFramerateLimiting (bool accurate);
	bool	Timer_DoesUseAccurateFramerateLimiting ();

	//	High-resolution (nanosecond) timing functions
	ubigint	Timer_GetDeltaHR ();
	ubigint	Timer_GetTimeHR ();
	double	Timer_GetResolutionHR ();

	void Input_ProcessEvent (const SDL_Event & Event);
	bool	Input_GetKeyState (SDLKey key);
	bool	Input_GetKeyDown (SDLKey key);
	bool	Input_GetKeyUp (SDLKey key);
	int		Input_GetMouseX ();
	int		Input_GetMouseY ();
	void	Input_GetMousePos (int * x, int * y);
	void	Input_SetMousePos (int x, int y);
	bool	Input_GetLMBState ();
	bool	Input_GetMMBState ();
	bool	Input_GetRMBState ();
	int		Input_GetMouseScroll ();

	bool	Input_GetLMBUp		();
	bool	Input_GetLMBDown	();
	bool	Input_GetMMBUp		();
	bool	Input_GetMMBDown	();
	bool	Input_GetRMBUp		();
	bool	Input_GetRMBDown	();

	const std::vector <SDL_Event> & Input_GetEvents ();
};

#endif