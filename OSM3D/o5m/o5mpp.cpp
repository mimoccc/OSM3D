
#include "API/o5mpp.h"
#include "API/o5mreader/o5mreader.h"
#include "../Common/Type.h"

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

	O5mreaderRet o5mResult = o5mreader_open( &m_Reader, m_FileHandle );
	return o5mResult == O5MREADER_RET_OK;
}

/*
=================
EndSession
=================
*/
void o5m::StreamSession::EndSession( )
{
	if( !IsOpen( ) )
		return;

	o5mreader_close( m_Reader );
	m_Reader = nullptr;

	fclose( m_FileHandle );
	m_FileHandle = nullptr;
}

/*
=================
ReadNextElement
=================
*/
o5m::ElementType o5m::StreamSession::ReadNextElement( )
{
	if( !IsOpen( ) )
		return o5m::INVALID;

	O5mreaderIterateRet iterationResult = o5mreader_iterateDataSet( m_Reader, &m_LatestDataset );
	
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
	m_Reader = nullptr;
	m_LatestDataset.type = O5MREADER_DS_END;
}

/*
=================
~StreamSession
=================
*/
o5m::StreamSession::~StreamSession()
{
	EndSession( );
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

/*
=================
GetCurrentNode
=================
*/
void o5m::StreamSession::GetCurrentNode( Node & target )
{
	if( m_LatestDataset.type != O5MREADER_DS_WAY )
		return;

	target.ID		= m_LatestDataset.id;
	target.Latitude	= m_LatestDataset.lat;
	target.Longitude= m_LatestDataset.lon;

	char * key, * value;

	while( o5mreader_iterateTags( m_Reader, &key, &value ) )
		target.Tags.push_back( StringPair( key, value ) );
}

/*
=================
GetCurrentWay
=================
*/
void o5m::StreamSession::GetCurrentWay( Way & target )
{
	if( m_LatestDataset.type != O5MREADER_DS_WAY )
		return;

	target.ID		= m_LatestDataset.id;
	target.Latitude	= m_LatestDataset.lat;
	target.Longitude= m_LatestDataset.lon;

	ubigint nodeId;
	while( o5mreader_iterateNds( m_Reader, &nodeId ) == O5MREADER_ITERATE_RET_NEXT )
		target.Nodes.push_back( nodeId );

	char * key, * value;

	while( o5mreader_iterateTags( m_Reader, &key, &value ) == O5MREADER_ITERATE_RET_NEXT )
		target.Tags.push_back( StringPair( key, value ) );
}

/*
=================
GetCurrentRelation
=================
*/
void o5m::StreamSession::GetCurrentRelation( Relation & target )
{
	if( m_LatestDataset.type != O5MREADER_DS_REL )
		return;

	target.ID		= m_LatestDataset.id;
	target.Latitude	= m_LatestDataset.lat;
	target.Longitude= m_LatestDataset.lon;

	ubigint refId;
	byte elementType;
	char * role;

	while( o5mreader_iterateRefs( m_Reader, &refId, &elementType, &role ) == O5MREADER_ITERATE_RET_NEXT )
		target.Elements.push_back( RelationElement( refId, elementType, role ) );

	char * key, * value;
	
	while( o5mreader_iterateTags( m_Reader, &key, &value ) == O5MREADER_ITERATE_RET_NEXT )
		target.Tags.push_back( StringPair( key, value ) );
}
