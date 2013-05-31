
#ifndef _Platform_h_
#define _Platform_h_

# if defined (WIN32) || defined (_WIN32)
#  define OSM3D_PLATFORM_WINDOWS
#  define OSM3D_PLATFORM_IDENTIFIED
# endif

#if defined (linux) || defined (__linux) || defined (__linux__)
#  define OSM3D_PLATFORM_LINUX
#  define OSM3D_PLATFORM_IDENTIFIED
# endif

# ifdef __APPLE__
#  define OSM3D_PLATFORM_MACOSX
#  define OSM3D_PLATFORM_IDENTIFIED
# endif



//	Not sure how this works on Mac OSX x86_64 applications, but I guess we'll see.
# if defined (LP64) || defined (__LP64__) || defined (_WIN64)
#  define OSM3D_PLATFORM_64BIT
# else
#  define OSM3D_PLATFORM_32BIT
# endif



# ifndef OSM3D_PLATFORM_IDENTIFIED
#  error Compiling for unidentified (and possibly unsupported) platform.
# endif

#endif // _Platform_h_