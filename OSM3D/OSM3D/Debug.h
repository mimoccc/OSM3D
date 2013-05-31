

#ifndef _Debug_h_
#define _Debug_h_

#include <string>

namespace Debug
{
	void				Error( std::string format, ... );
	void				Warning( std::string format, ... );

	void				Printf( std::string format, ... );
}

#endif // _Debug_h_