

#include <cstdarg>
#include <iostream>
#include <cstdarg>

#include <Windows.h>

#include "Debug.h"



// Takes the given string reference and evaluates the formatting via the variadic parameters
//	that are assumed to follow it. Not suitable for arbitrary formatting.
std::string EvaluateFormattedText( std::string & format, ... )
{
	static char printBuffer [0x40000];

	va_list args;
	va_start(args, format);

	vsprintf_s (printBuffer, format.c_str (), args);

	va_end(args);

	return printBuffer;
}



/*
=================
Debug::Error

Reports an error by displaying a message box for the user to interact with.
=================
 */
void Debug::Error( std::string format, ... )
{
	std::string errorString;
	errorString += "Debug::Error invoked:\n\n";
	errorString += EvaluateFormattedText(format);
	errorString += "\n\nWould you like to run a debug break?";

	//	Display the error to the user
	int result = MessageBoxA(NULL, errorString.c_str(), "Debug::Error", MB_APPLMODAL | MB_YESNOCANCEL);

	//	Act based on their intent
	switch (result)
	{
	case (IDYES):
		{
			__debugbreak();
			break;
		}

	case (IDNO):
		{
			break;
		}

	case (IDCANCEL):
		{
			exit (0);
			break;
		}

	default:
		break;
	}
}

/*
=================
Debug::Warning
=================
 */
void Debug::Warning( std::string format, ... )
{
	std::cout << EvaluateFormattedText(format) << std::endl;
}

/*
=================
Debug::Printf

Adds the given text to the list of text to be drawn.
Reference: http://www.cplusplus.com/reference/cstdio/printf/
=================
 */
void Debug::Printf (std::string format, ...)
{
	std::cout << EvaluateFormattedText( format ) << std::endl;
}