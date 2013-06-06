

#ifndef _o5m_h_
#define _o5m_h_

#include <cstdio>
#include <string>

/*
 * TODO: Reader should be able to handle out-of-memory situations via a disk-caching system.
 */

namespace o5m
{
	class Node
	{
	public:

	};

	class Way 
	{
	};

	class Relation
	{
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

		ElementType PeekNextElement( ) const;

	private:
		FILE * m_FileHandle;
	};
}

#endif // _o5m_h_