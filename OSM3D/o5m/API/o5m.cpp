
#include "o5m.h"
#include "../../Common/Type.h"



/*
=================
BeginSession
=================
*/
bool o5m::StreamSession::BeginSession( const std::string & sourceFile )
{
	int result = fopen_s( &m_FileHandle, sourceFile.c_str(), "r" );

	if( result != 0 )
		return false;

	byte firstByte = 0;
	fread_s( &firstByte, 1, 1, 1, m_FileHandle );
	if( firstByte != 0xFF )
	{
		EndSession( );
		return false; // Invalid formatting. "Each .o5m file starts with a byte 0xff and ends with byte 0xfe"
	}

	return true;
}

/*
=================
EndSession
=================
*/
void o5m::StreamSession::EndSession()
{
	if( !IsOpen( ) )
		return;

	fclose( m_FileHandle );
	m_FileHandle = nullptr;
}

/*
=================
PeekNextElement
=================
*/
o5m::ElementType o5m::StreamSession::PeekNextElement() const
{
	

	return o5m::INVALID;
}

/*
=================
StreamSession
=================
*/
o5m::StreamSession::StreamSession()
{
	m_FileHandle = nullptr;
}

/*
=================
~StreamSession
=================
*/
o5m::StreamSession::~StreamSession()
{
	if( m_FileHandle != nullptr )
		fclose( m_FileHandle );
}

/*
=================
IsOpen
=================
*/
bool o5m::StreamSession::IsOpen() const
{
	return m_FileHandle != nullptr;
}
