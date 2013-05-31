
#ifndef _StringUtilities_h_
#define _StringUtilities_h_

#include "Type.h"

typedef std::vector<std::string> StringSet;

namespace StringUtilities
{
	StringSet * SplitStringAtSymbol (std::string text, std::string symbol, StringSet * resultsOutOptional = NULL);
	uint		GetTermCount (std::string text, std::string term);
	std::string EliminateSymbolFromString (std::string text, std::string symbol);
	std::string ConvergeStringSet (StringSet * stringSet, std::string separator = "");
	std::string ReplaceAllOfSymbolInString (std::string string, std::string targetSymbol, std::string replacementSymbol);
	std::string GetStringComponentBySeparatorIndex (std::string str, int index, std::string separator);

	enum e_TrimOptions {
		TRIM_SPACE		= 0x01,
		TRIM_NEWLINE	= 0x02,
		TRIM_TABS		= 0x04,
		TRIM_ALL		= 0x07
	};

	std::string TrimString (std::string text, e_TrimOptions trimOptions = TRIM_ALL);

	std::string DecimalToHexadecimal (uint value);

	std::string ToLower (std::string str);
	std::string ToUpper (std::string str);
}

#endif // _StringUtilities_h_