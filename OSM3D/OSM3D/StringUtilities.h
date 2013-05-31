
#ifndef _StringUtilities_h_
#define _StringUtilities_h_

#include "Type.h"

typedef std::vector<std::string> StringSet;

namespace StringUtilities
{
	void		SplitStringAtSymbol( const std::string & text, const std::string & symbol, StringSet & resultsOut );
	uint		GetTermCount( const std::string & text, const std::string & term );
	std::string EliminateSymbolFromString( const std::string & text, const std::string & symbol );
	std::string ConvergeStringSet( const StringSet & stringSet, const std::string & separator = "" );
	std::string ReplaceAllOfSymbolInString( const std::string & string, const std::string & targetSymbol, const std::string & replacementSymbol );
	std::string GetStringComponentBySeparatorIndex( const std::string & str, int index, const std::string & separator );

	enum e_TrimOptions {
		TRIM_SPACE		= 0x01,
		TRIM_NEWLINE	= 0x02,
		TRIM_TABS		= 0x04,
		TRIM_ALL		= 0x07
	};

	std::string TrimString( const std::string & text, e_TrimOptions trimOptions = TRIM_ALL );

	std::string DecimalToHexadecimal( uint value );

	std::string ToLower( const std::string & str );
	std::string ToUpper( const std::string & str );
}

#endif // _StringUtilities_h_