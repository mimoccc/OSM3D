
#include "../Common/Platform.h"

#ifdef OSM3D_DEBUG

/* Note: The project under visual studio is set to export to OSM3D's global lib folder, next to
 *	SDL.lib and whatnot.
 */

#include <string>
#include "API/o5m.h"

int main( )
{
	std::string fileName = "D:/Earth/NewYork_01.o5m";

	o5m::StreamSession session;
	if( !session.BeginSession( fileName ) )
		throw;

	session.EndSession( );

	return 0;
}

#endif