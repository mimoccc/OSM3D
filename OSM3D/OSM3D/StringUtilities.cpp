
#include "StringUtilities.h"

StringSet * StringUtilities::SplitStringAtSymbol (std::string text, std::string symbol, StringSet * resultsOutOptional)
{
	if (resultsOutOptional == NULL)
		resultsOutOptional = new StringSet;

	uint currentPosition;
	while ((currentPosition = text.find (symbol)) != text.npos)
	{
		resultsOutOptional->push_back (text.substr (0, currentPosition));
		text = text.substr (currentPosition+symbol.size ());
	}

	//	At this point we have a string that just doesn't have any of that symbol, just use the rest of the string
	resultsOutOptional->push_back (text);

	return resultsOutOptional;
}

uint StringUtilities::GetTermCount (std::string text, std::string term)
{
	uint termCount = 0;
	uint findPosition = 0;
	while ((findPosition = text.find (term, findPosition)) != text.npos)
	{
		termCount++;
		findPosition += term.size ();
	}

	return termCount;
}

std::string StringUtilities::EliminateSymbolFromString (std::string text, std::string symbol)
{
	uint currentPosition;
	while ((currentPosition = text.find (symbol)) != text.npos)
	{
		text.erase (currentPosition, symbol.size ());
	}
	return text;
}

std::string StringUtilities::ConvergeStringSet (StringSet * stringSet, std::string separator)
{
	if (stringSet->size () == 0)
		return 0;

	std::string result;
	for (auto iter = stringSet->begin (); iter != --(stringSet->end ()); iter++)
	{
		result += *iter + separator;
	}
	result += *(--(stringSet->end ()));

	return result;
}

std::string StringUtilities::TrimString (std::string text, e_TrimOptions trimOptions)
{
	if (trimOptions == 0)
		return text;

	std::string results;

	for (uint i = 0; i < text.size (); i++)
	{
		if (trimOptions & TRIM_NEWLINE)
			if (text [i] == '\n')
				continue;

		if (trimOptions & TRIM_SPACE)
			if (text [i] == ' ')
				continue;

		if (trimOptions & TRIM_TABS)
			if (text [i] == '\t')
				continue;

		results += text [i];
	}

	return results;
}

#	define DecimalToHexadecimalCase(hexValue) case (0x##hexValue): result += #hexValue; break;
//	Here we assume a Big-Endian system
std::string StringUtilities::DecimalToHexadecimal (uint value)
{
	std::string result = "0x";
	for (uchar i = 1; i <= sizeof (value)*2; i++)
	{
		uchar currentChunk = (0xF & (value >> ((sizeof (value)*2 - i)*4)));
		switch (currentChunk)
		{
			DecimalToHexadecimalCase (0)
				DecimalToHexadecimalCase (1)
				DecimalToHexadecimalCase (2)
				DecimalToHexadecimalCase (3)
				DecimalToHexadecimalCase (4)
				DecimalToHexadecimalCase (5)
				DecimalToHexadecimalCase (6)
				DecimalToHexadecimalCase (7)
				DecimalToHexadecimalCase (8)
				DecimalToHexadecimalCase (9)
				DecimalToHexadecimalCase (A)
				DecimalToHexadecimalCase (B)
				DecimalToHexadecimalCase (C)
				DecimalToHexadecimalCase (D)
				DecimalToHexadecimalCase (E)
				DecimalToHexadecimalCase (F)
		}
	}

	return result;
}
#	undef DecimalToHexadecimalCase

std::string StringUtilities::ReplaceAllOfSymbolInString (std::string string, std::string targetSymbol, std::string replacementSymbol)
{
	std::size_t pos;
	while ((pos = string.find (targetSymbol)) != string.npos)
	{
		string.replace (pos, targetSymbol.size (), replacementSymbol);
	}

	return string;
}

std::string StringUtilities::GetStringComponentBySeparatorIndex (std::string str, int index, std::string separator)
{
	StringSet dest;
	SplitStringAtSymbol (str, separator, &dest);

	int i = 0;
	for (auto iter = dest.begin (); iter != dest.end (); iter++, i++)
	{
		if (index == i)
			return *iter;
	}

	return str;
}

std::string StringUtilities::ToLower (std::string str)
{
	for (uint i = 0; i < str.size (); i++)
	{
		str [i] = tolower (str [i]);
	}

	return str;
}

std::string StringUtilities::ToUpper (std::string str)
{
	for (uint i = 0; i < str.size (); i++)
	{
		str [i] = toupper (str [i]);
	}

	return str;
}
