
/* Managing libraries through the project settings UI is always a pain */
#ifdef _MSC_VER

#pragma comment (lib, "SDL.lib")
#pragma comment (lib, "SDL_ttf.lib")
#pragma comment (lib, "SDL_image.lib")
#pragma comment (lib, "OpenGL32.lib")

# ifdef _DEBUG
#  pragma comment (lib, "o5mpp-d.lib")
# else
#  pragma comment (lib, "o5mpp.lib")
# endif

#endif