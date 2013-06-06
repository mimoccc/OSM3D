
/*
=================
StringToReal

Converts the given string to the specified real-value primitive type. Does not do any checks
for valid input.
=================
*/
template <typename T>
inline T StringToReal( const std::string & string )
{
	std::istringstream stream( string );
	T buffer;
	stream >> buffer;
	return buffer;
}

/*
=================
StringToReal<bool>
=================
*/
template <>
inline bool StringToReal<bool>( const std::string & string )
{
	if ( IsReal( string ) )
	{
		return string != "0";
	}

	std::string lowerCaseString;
	std::transform( string.begin(), string.end(), lowerCaseString.begin(), ::tolower );
	return lowerCaseString == "true" ? true : false;
}

/*
=================
RealToString

Converts the given real-valued primitive to an STL string.
=================
*/
template <typename T>
inline std::string RealToString( T value )
{
	std::ostringstream stream;
	stream << value;
	return stream.str();
}

template <>
inline std::string RealToString<bool>( bool value )
{
	return value ? "true" : "false";
}

/*
=================
IsReal

Returns whether or not the text within 'string' is meant to be a real-valued number.
=================
*/
inline bool IsReal( const std::string & string )
{
	std::istringstream stream( string );
	double junkValue;
	stream >> junkValue;
	return !stream.fail();
}

/*
=================
IsBool
=================
*/
inline bool IsBool( const std::string & string )
{
	std::string lowerCaseString;
	std::transform( string.begin(), string.end(), lowerCaseString.begin(), ::tolower );

	return lowerCaseString == "true" || lowerCaseString == "false";
}

/*
=================
IsString

Returns whether or not the text within 'string' is actually representative of text.
=================
*/
template <typename T>
inline bool IsString( const std::string & string )
{
	return !IsReal<T>( string );
}