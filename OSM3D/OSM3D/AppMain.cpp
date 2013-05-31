
#include <SDL/SDL.h>

#include "ConfigParser/ConfigParser.h"

#undef main
int main( int argc, char * argv[] )
{
	ConfigParser config( "../../../appinfo.cfg" );

	return 0;
}