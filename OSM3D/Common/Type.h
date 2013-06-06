
#ifndef _Type_h_
#define _Type_h_

#include <sstream>
#include <algorithm>
#include <vector>
#include <cstdint>

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef uint64_t		ubigint;

typedef uchar byte;




template <typename T>
inline T StringToReal( const std::string & string );

template <>
inline bool StringToReal<bool>( const std::string & string );

template <typename T>
inline std::string RealToString( T value );


inline bool IsReal( const std::string & string );
inline bool IsBool( const std::string & string );
inline bool IsString( const std::string & string );

#include "Type.inl"

#endif // _Type_h__