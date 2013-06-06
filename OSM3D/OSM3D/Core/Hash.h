
#ifndef _ORGUI_HASH_H_
#define _ORGUI_HASH_H_

#include <cstdint>
#include "../Core/Type.h"


/* Generic hashing algorithms; they're not very fancy or secure, but they don't really need to be */

typedef uint64_t Hash;

inline Hash CreateHash (byte * data, int size)
{
	int result = 0;
	for (int i = 0; i < size; i++)
	{
		result ^= ((int)data [i]) << ((i%4)*8);
	}
	return result;
}

inline Hash CreateHash (const std::string & text)
{
	return CreateHash ((byte *)text.c_str( ), text.size ());
}

#endif