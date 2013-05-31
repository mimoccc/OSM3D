
#ifndef _ConfigParser_h_
#define _ConfigParser_h_

#include <string>
#include <map>

class ConfigParser
{
public:
				ConfigParser( );
				ConfigParser( const std::string & fileName );
				~ConfigParser( );

	void		LoadFromFile( const std::string & fileName );
	std::string	SaveToFile( const std::string & fileName );

	std::string	GetStringProperty( const std::string & propertyName, const std::string & defaultValue = "" ) const;
	double		GetRealProperty( const std::string & propertyName, double defaultValue = 0.0 ) const;
	bool		GetBoolProperty( const std::string & propertyName, bool defaultValue = false ) const;

	bool		PropertyExists( const std::string & propertyName ) const;

	void		SetStringProperty( const std::string & propertyName, const std::string & value );
	void		SetRealProperty( const std::string & propertyName, double value );
	void		SetBoolProperty( const std::string & proeprtyName, bool value );

private:

	std::map<std::string, std::string> m_PropertyMappings;
};

#endif // _ConfigParser_h_