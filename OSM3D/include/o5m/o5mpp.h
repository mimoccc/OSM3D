

#ifndef _o5m_h_
#define _o5m_h_

#include <cstdio>
#include <string>
#include "o5mreader/o5mreader.h"
#include <Type.h>
#include <CommonStringAllocator.h>

namespace o5m
{
	struct StringPair
	{
		StringPair( ) { }
		StringPair( const InlineString & key, const InlineString & value )
			: Key( Key ), Value( value ) { }

		InlineString Key;
		InlineString Value;
	};

	struct RelationElement
	{
		RelationElement( ) { }
		RelationElement( ubigint id, ushort type, const InlineString & role )
			: ID( id ), ElementType( type ), Role( role ) { }

		ubigint			ID;
		ushort			ElementType;
		InlineString	Role;
	};

	typedef std::vector<StringPair>	TagSet;
	typedef std::vector<ubigint>	IdSet;

	typedef std::vector<RelationElement> RelationSet;

	class O5mElement
	{
	public:
		ubigint ID;
		uint Latitude, Longitude;
		TagSet Tags;
	};

	class Node : public O5mElement
	{
	};

	class Way : public O5mElement
	{
	public:
		IdSet Nodes;
	};

	class Relation : public O5mElement
	{
	public:
		RelationSet Elements;
	};

	enum ElementType
	{
		NODE,
		WAY,
		RELATION,
		ENDOFFILE,
		INVALID
	};

	class StreamSession
	{
	public:

		StreamSession( );
		~StreamSession( );

		bool IsOpen( ) const;

		bool BeginSession( const std::string & sourceFile );
		void EndSession( );

		ElementType ReadNextElement( );

		void GetCurrentNode( Node & target );
		void GetCurrentWay( Way & target );
		void GetCurrentRelation( Relation & target );

	private:
		FILE * m_FileHandle;
		O5mreader * m_Reader;
		O5mreaderDataset m_LatestDataset;
	};
}

#endif // _o5m_h_