
#ifndef _CommonStringAllocator_h_
#define _CommonStringAllocator_h_

#include <string>

/* A custom STL allocator that can be used with std::basic_string 
 *	to use inline memory. This stops std::string from copying the
 *	given text, which is the standard behavior, and instead uses
 *	a direct reference to the string that was given. Note that size
 *	is assumed to be constant.
 */

class CommonStringAllocator
{
};

//typedef const std::basic_string<char, std::char_traits<char>, CommonStringAllocator> InlineString;
typedef std::string InlineString;

#endif // _CommonStringAllocator_h_