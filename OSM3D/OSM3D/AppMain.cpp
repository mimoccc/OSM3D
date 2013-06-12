
#include <SDL/SDL.h>

#include "ConfigParser/ConfigParser.h"
#include "Graphics/worldgl.h"

#include <o5m/o5mpp.h>

#undef main
int main( int argc, char * argv[] )
{
	ConfigParser config( "../../../appinfo.cfg" );

	o5m::StreamSession session;
	session.BeginSession( config.GetStringProperty( "o5m_source" ) );

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

	session.EndSession( );

	return 0;
}