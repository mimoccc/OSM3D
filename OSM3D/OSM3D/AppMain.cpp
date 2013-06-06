
#include <SDL/SDL.h>

#include "ConfigParser/ConfigParser.h"
#include "Graphics/worldgl.h"

#undef main
int main( int argc, char * argv[] )
{
	ConfigParser config( "../../../appinfo.cfg" );

	WorldGL::ConstructFrameworks();
	WorldGL world;
	world.Window_Create( );

	bool isRunning = true;
	while( world.Window_IsOpen( ) )
	{
		SDL_Event currentEvent;
		SDL_PumpEvents( );
		while( SDL_PollEvent( &currentEvent ) )
		{
			world.Input_ProcessEvent( currentEvent );
		}

		world.Window_Refresh( );
	}

	WorldGL::DestructFrameworks();

	return 0;
}