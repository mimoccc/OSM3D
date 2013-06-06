#ifndef _OSM3D_FILE_IO_H_
#define _OSM3D_FILE_IO_H_

#include <string>
#include <list>

#include "../Common/Type.h"
#include "../Core/StringUtilities.h"

//	WinAPI defines this as DeleteFileA/DeleteFileW; we use the name for our own stuff, so
//		get rid of it
#ifdef DeleteFile
#undef DeleteFile
#endif
#ifdef CopyFile
#undef CopyFile
#endif
#ifdef CreateDirectory
#undef CreateDirectory
#endif

/* TODO: Organize this thing */

namespace IO
{

	void		SetActiveDirectory (std::string pathName);

	std::string	GetActiveDirectory ();

	void		ResetActiveDirectory ();

	std::string	GetApplicationDirectory ();

	std::string	TranslatePath( const std::string & path );

	std::string	CombinePaths( const std::string & firstPart, const std::string & lastPart);

	bool		PathIsAbsolute (const std::string & path);

	bool		PathIsValid (const std::string & path);

	bool		GetEntriesInDirectory (const std::string & directory, StringSet & stringSet);

	bool		EntryIsFolder (const std::string & entry);

	bool		EntryIsFile (const std::string & entry);

	bool		FileExists (const std::string & fileName);

	bool		DirectoryExists (const std::string & directoryName);

	bool		EntryExists (const std::string & entryName);

	byte *		GetFileContents( const std::string & fileName, uint & fileSize, uint extraBytes = 0 );

	uint		GetFileContents (const std::string & fileName, byte * buffer, uint bufferSize);

	uint		GetFileSize (const std::string & fileName);

	bool		WriteFile (const std::string & fileName, byte * fileContents, uint fileSize);

	bool		WriteFile (const std::string & fileName, const std::string & fileContents);

	bool		CreateDirectory (const std::string & directoryName);

	bool		CopyFile (const std::string & sourceFileName, const std::string & newFileName);

	bool		CopyFileOverwrite( const std::string & sourceFileName, const std::string & newFileName );

	bool		DeleteFile (const std::string & fileName);

	bool		DeleteDirectory (const std::string & directoryName);

	std::string	FormatPath( const std::string & path );

}


#endif
