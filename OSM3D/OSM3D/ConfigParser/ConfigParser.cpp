
#include "ConfigParser.h"
#include "../Type.h"
#include "../FileIO/BasicIO.h"


ConfigParser::ConfigParser()
{

}

ConfigParser::ConfigParser( const std::string & fileName )
{
	LoadFromFile( fileName );
}

ConfigParser::~ConfigParser()
{

}

/*
=================
GetStringProperty
=================
*/
std::string ConfigParser::GetStringProperty( const std::string & propertyName, const std::string & defaultValue /*= "" */ ) const
{
	if( !PropertyExists( propertyName ) )
		return defaultValue;

	return m_PropertyMappings.find( propertyName )->second;
}

/*
=================
GetRealProperty
=================
*/
double ConfigParser::GetRealProperty( const std::string & propertyName, double defaultValue /*= 0.0 */ ) const
{
	if( !PropertyExists( propertyName ) )
		return defaultValue;

	std::string value = m_PropertyMappings.find( propertyName )->second;
	if (!IsReal( value ))
		return defaultValue;

	return StringToReal<double>(value);
}

/*
=================
GetBoolProperty
=================
*/
bool ConfigParser::GetBoolProperty( const std::string & propertyName, bool defaultValue /*= false */ ) const
{
	if( !PropertyExists( propertyName ) )
		return defaultValue;

	std::string value = m_PropertyMappings.find( propertyName )->second;
	if ( !IsBool( value ) )
		return defaultValue;

	return StringToReal<bool>( value );
}

/*
=================
PropertyExists
=================
*/
bool ConfigParser::PropertyExists( const std::string & propertyName ) const
{
	return m_PropertyMappings.find( propertyName ) != m_PropertyMappings.end( );
}

/*
=================
SetStringProperty
=================
*/
void ConfigParser::SetStringProperty( const std::string & propertyName, const std::string & value )
{
	m_PropertyMappings[propertyName] = value;
}

/*
=================
SetRealProperty
=================
*/
void ConfigParser::SetRealProperty( const std::string & propertyName, double value )
{
	m_PropertyMappings[propertyName] = RealToString(value);
}

/*
=================
SetBoolProperty
=================
*/
void ConfigParser::SetBoolProperty( const std::string & proeprtyName, bool value )
{
	m_PropertyMappings[proeprtyName] = RealToString(value);
}

/*
=================
LoadFromFile
=================
*/
void ConfigParser::LoadFromFile( const std::string & fileName )
{
	/* Get file contents */
	uint fileSize;
	byte * fileContents = IO::GetFileContents( fileName, fileSize );
	
	std::string fileData = (char *)fileContents;
	delete [] fileContents;

	throw;
}

/*
=================
SaveToFile
=================
*/
std::string ConfigParser::SaveToFile( const std::string & fileName )
{
	throw;
}