
#include <list>

#include "../Platform.h"

#ifdef OSM3D_PLATFORM_WINDOWS
#include <Windows.h>
#undef DeleteFile
#else
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>

//OK, this is just me being lazy (Linux Port)
#define GetLastError() " "

#endif

#include "BasicIO.h"

#include <stdio.h>
//NOTE: I CHANGED THIS FROM <vaargs.h> -- It's deprecated - Ben
//RESPONSE TO NOTE: Thanks - Tyler
#include <stdarg.h>
#include "String.h"

#include "../Debug.h"

static std::string g_ActiveDirectory = "";

namespace IO
{

/*
=================
InitializeActiveDirectory
=================
*/
void InitializeActiveDirectory ()
{
	if (g_ActiveDirectory.size () == 0)
	{
		g_ActiveDirectory = GetApplicationDirectory ();
	}
}

/*
=================
CompileActiveDirectory

"Compiling" refers to the point when we were planning on being able
to push/pop active directory states like OpenGL, but decided against it
in the end. Keeping this here since it doesn't hurt.
=================
*/
std::string CompileActiveDirectory ()
{
	InitializeActiveDirectory ();
	return g_ActiveDirectory;
}

/*
=================
GetCompiledActiveDirectory
=================
*/
std::string GetCompiledActiveDirectory ()
{
	return CompileActiveDirectory ();
}


/*
=================
GetPlatformMaxPathSize

Returns the maximum path size (in characters, includes NULL-terminator) that the operating
system permits.
=================
*/
uint GetPlatformMaxPathSize ()
{
#ifdef OSM3D_PLATFORM_WINDOWS
	return MAX_PATH;
#elif defined(OSM3D_PLATFORM_LINUX)
	return PATH_MAX;
#endif
}

/*
=================
SetActiveDirectory
=================
*/
void SetActiveDirectory (const std::string & directory)
{
	if (!PathIsValid (directory))
		return;

	g_ActiveDirectory = TranslatePath (directory);
}

/*
=================
GetActiveDirectory
=================
*/
std::string GetActiveDirectory ()
{
	return GetCompiledActiveDirectory ();
}

/*
=================
ResetActiveDirectory
=================
*/
void ResetActiveDirectory ()
{
	g_ActiveDirectory = "";
	InitializeActiveDirectory ();
}

/*
=================
GetApplicationDirectory
=================
*/
std::string GetApplicationDirectory ()
{
	std::string filePath;

#	ifdef OSM3D_PLATFORM_IDENTIFIED
	byte * filePathBuffer = new byte [GetPlatformMaxPathSize ()];
	DWORD writtenBytes = GetModuleFileNameA (NULL, (LPSTR)filePathBuffer, GetPlatformMaxPathSize ());

	if (GetLastError () == ERROR_INSUFFICIENT_BUFFER)
	{
		Debug::Warning(
			"FileIO.cpp | GetApplicationDirectory - Unable to successfully retrieve application directory - path exceeds maximum length allowed by the operating system."
			"Max path length: %i\n"
			"Retrieved path data: \"%s\"",
			GetPlatformMaxPathSize(), (char *)filePathBuffer
			);
	}

	//	The string returned includes the executable file itself, we need to strip this off
	filePath = (char *)filePathBuffer;
	filePath = filePath.substr (0, filePath.rfind ('\\')+1);

	//	May have to get rid of this line for different std::string implementations.
	delete [] filePathBuffer;
# elif defined(OSM3D_PLATFORM_LINUX)
	const char* fn;
	fn = getcwd(NULL, 0);
	if(!fn)
	{
		Debug::Printf(
			"FileIO.cpp | ore::FileIO::GetApplicationDirectory - Unable to successfully retrieve application directory; unknown reason!"
			);
	}
#      else
#error	UNSUPPORTED PLATFORM FOR GetApplicationDirectory.
#	endif

	//	Force formatting on Windows
#ifdef OSM3D_PLATFORM_WINDOWS
	return FormatPath(filePath);
#elif defined(OSM3D_PLATFORM_LINUX)
	return filePath;
#endif
}

/*
=================
TranslatePath
=================
*/
std::string TranslatePath (const std::string & unformattedPath)
{
	std::string path (FormatPath (unformattedPath));

	if (path == ".")
		return GetApplicationDirectory ();

	if (path.size () > 1)
	{
		std::string sub = path.substr (0, 2);
		if (sub == "./")
		{
			path = GetApplicationDirectory () + path.substr (1);
		}
	}

	//	If it's an absolute path (i.e. C:/... or /blah) then we don't
	//		have any translating to do - the path doesn't make
	//		use of the active directory.
	if (!PathIsAbsolute (path))
	{
		path = CombinePaths (GetCompiledActiveDirectory (), path);
	}

	//path = TranslatePath (path);

	if (path.find ("..") != path.npos)
	{
		// uint currentLocation = path.find_first_of ("..");

		//	StringSet stores by vector, can't use vectors here since iterators are invalidated
		//		upon modification.
		std::list<std::string> results;

		//	Manually split the string at / symbols
		uint currentPosition;
		while( (currentPosition = path.find( "/" )) != path.npos )
		{
			results.push_back( path.substr( 0, currentPosition ) );
			path = path.substr( currentPosition + 1 );
		}
		results.push_back( path );


		for (auto iter = results.begin (); iter != results.end (); iter++)
		{
			if (*iter == "..")
			{
				if (iter != results.begin ())
				{
					//	Erase the previous directory (go one up)
					auto neighbor = iter;
					neighbor--;
					results.erase (neighbor);

					//	Erase the current "directory" (which should be the '..')
					neighbor = iter;
					neighbor++;
					results.erase (iter);

					//	Set our current location to the previous iterator
					iter = neighbor;
					iter--;
				}
			}
		}

		//	Reconstruct the path from the altered stringset
		path = "";
		for (auto iter = results.begin (); iter != results.end (); iter++)
		{
			path += *iter + "/";
		}
	}

	if (path [path.size ()-1] == '/')
		path = path.substr (0, path.size ()-1);

	return path;
}

/*
=================
CombinePaths
=================
*/
std::string CombinePaths (const std::string & firstPath, const std::string & secondPath)
{
	//	Advanced checks can be done to verify firstPath and secondPath, but this can't always
	//		be accurate and will often be fairly intensive. We'll just leave things like this for
	//		now.
	//
	return firstPath + "/" + secondPath;
}

/*
=================
PathIsAbsolute
=================
*/
bool PathIsAbsolute (const std::string & path)
{
	if (path.length() > 0)
		return false;

#if defined(OSM3D_PLATFORM_IDENTIFIED)
	return path.find (':') != path.npos;
#elif defined(OSM3D_PLATFORM_LINUX)
	return path[0] == '/';
#else
#error Unsupported platform for ore::FileIO::PathIsAbsolute ()
#endif
}

/*
=================
PathIsValid
=================
*/
bool PathIsValid (const std::string & path)
{
	//	We can add more checks later (i.e. ensuring that no illegal characters are in the path)
	//		but it's not really necessary now.
	if (path.size ()+1 > GetPlatformMaxPathSize ())
	{
		//	+1 since it includes the NULL terminator

		Debug::Warning(
			"Path:\n%s\nExceeds the path size limit of the operating system."
			"Max Size: %i"
			"Path Length: %i",
			GetPlatformMaxPathSize(), path.size() + 1
			);
		return false;
	}

	return true;
}

/*
=================
GetEntriesInDirectory
=================
*/
bool GetEntriesInDirectory (const std::string & unformattedDirectory, StringSet directoryContents)
{
	//	Take the active directory into account
	std::string directory( TranslatePath (unformattedDirectory) );

	if (!PathIsValid (directory))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS

	if (!DirectoryExists (directory))
		return NULL;

	//	Windows requires that retrieving the contents of a directory be done via the syntax:
	//		A:/b/c/Directory/*
	directory += "/*";

	WIN32_FIND_DATA windowsSearchResults;
	HANDLE searchHandle = FindFirstFile (directory.c_str(), &windowsSearchResults);
	if (searchHandle == INVALID_HANDLE_VALUE)
	{
		if (GetLastError () != ERROR_FILE_NOT_FOUND)
		{
			Debug::Warning(
				"FileIO.cpp | GetEntriesInDirectory - Could not open path for searching:\n\"%s\""
				"GetLastError( ) - %i",
				directory.c_str(), GetLastError( )
				);
		}

		return NULL;
	}

	//	For some reason Windows gives us the values "." and ".." when we're enumerating, which
	//		can cause the function to get stuck in a recursive loop. We just skip these. "." is
	//		generally the first to show up, which is why that's the only check here.
	if (std::string (windowsSearchResults.cFileName) != ".")
		directoryContents.push_back (windowsSearchResults.cFileName);

	bool success = false;

	while (FindNextFile (searchHandle, &windowsSearchResults) != FALSE)
	{
		std::string result = windowsSearchResults.cFileName;
		if (result != "." && result != "..")
		{
			directoryContents.push_back (result);
			success = true;
		}
	}

	if (GetLastError () != ERROR_NO_MORE_FILES)
	{
		Debug::Warning(
			"FileIO.cpp | GetEntriesInDirectory - Error occurred while searching directory:\n%s"
			"GetLastError( ) - %i",
			directory.c_str(), GetLastError( )
			);
	}

	return success;
#elif defined(OSM3D_PLATFORM_LINUX)
	DIR *pDIR = NULL;
	struct dirent *entry;
	if( (pDIR = opendir(C_STRING(directory))))
	{
		while((entry = readdir(pDIR)))
		{
			if( !strcmp(entry->d_name, ".") && !strcmp(entry->d_name, ".."))
				directoryContents->push_back(entry->d_name);
		}
		closedir(pDIR);

		return true;
	}

	Debug::Warning(
		"FileIO.cpp | GetEntriesInDirectory - Error occurred while searching directory:\n%s"
		"Error: %i",
		directory, strerror(errno)
		);

	return false;
#else
#error	UNSUPPORTED PLATFORM FOR GetEntriesInDirectory.
#endif
}

/*
=================
EntryIsFolder
=================
*/
bool EntryIsFolder (const std::string & unformattedEntry)
{
	std::string entry(TranslatePath (unformattedEntry));

	if (!PathIsValid (entry))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	DWORD fileAttributes = GetFileAttributes (entry.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		if (GetLastError () != ERROR_FILE_NOT_FOUND)
		{
			Debug::Warning(
				"FileIO.cpp | EntryIsFolder - File attributes are invalid for:\n%s"
				"GetLastError( ) - %i",
				entry.c_str(), GetLastError( )
				);
		}

		return false;
	}

	//	If any of these flags are set on the file, we don't count it as a directory.
	static const DWORD directoryDiscriminant =
		FILE_ATTRIBUTE_ARCHIVE |
		FILE_ATTRIBUTE_SYSTEM;

	return (fileAttributes & directoryDiscriminant) == 0;
#elif defined(OSM3D_PLATFORM_LINUX)
	struct stat st;
	lstat(C_STRING(entry), &st);
	if(S_ISDIR(st.st_mode))
		return true;
	else
		return false;
#else
#error	UNSUPPORTED PLATFORM FOR EntryIsFolder.
#endif
}

/*
=================
EntryIsFile
=================
*/
bool EntryIsFile (const std::string & unformattedEntry)
{
	std::string entry (TranslatePath (unformattedEntry));

	if (!PathIsValid (entry))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	DWORD fileAttributes = GetFileAttributes (entry.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
	{
		if (GetLastError () != ERROR_FILE_NOT_FOUND)
		{
			Debug::Warning(
				"FileIO.cpp | EntryIsFile - File attributes are invalid for:\n%s"
				"GetLastError( ) - %i",
				entry.c_str(), GetLastError( )
				);
		}
		return false;
	}

	//	If any of these flags are set on the file, we don't count it as a file.
	static const DWORD fileDiscriminant =
		FILE_ATTRIBUTE_DIRECTORY |
		FILE_ATTRIBUTE_SYSTEM;

	return (fileAttributes & fileDiscriminant) == 0;
#elif defined(OSM3D_PLATFORM_LINUX)
	struct stat st;
	lstat(C_STRING(entry), &st);
	if(S_ISDIR(st.st_mode))
		return false;
	else
		return true;
#else
#error	UNSUPPORTED PLATFORM FOR EntryIsFile.
#endif
}

/*
=================
FileExists
=================
*/
bool FileExists (const std::string & unformattedFile)
{
	// Will eventually need to implement OS-specific code so that we can access
	//	permissions data. fopen_s does provide situation-specific error reporting,
	//	and this should be implemented whenever the opportunity presents itself.
	//	You can look at:
	//
	// http://msdn.microsoft.com/en-us/library/t3ayayh1(v=vs.80).aspx
	//
	//  for Windows-specific return values.
	std::string file( TranslatePath (unformattedFile) );

	if (!PathIsValid (file))
		return false;

	FILE * fileHandle = NULL;

	fopen_s (&fileHandle, file.c_str(), "rb");

	bool exists = fileHandle != NULL;
	if (exists)
		fclose (fileHandle);
	return exists;
}

/*
=================
DirectoryExists
=================
*/
bool DirectoryExists (const std::string & unformattedDirectoryName)
{
#ifdef OSM3D_PLATFORM_WINDOWS
	std::string directoryName( TranslatePath (unformattedDirectoryName) );

	if (!PathIsValid (directoryName))
		return false;

	DWORD fileAttributes = GetFileAttributes (directoryName.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES)
		return false;

	return (fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif defined(OSM3D_PLATFORM_LINUX)
	struct stat st;
	return (!lstat(C_STRING(directoryName), &st) || !S_ISDIR(st.st_mode));

#else
#error	UNSUPPORTED PLATFORM FOR DirectoryExists.
#endif
}

/*
=================
EntryExists
=================
*/
bool EntryExists (const std::string & unformattedEntryName)
{
	std::string entryName( TranslatePath (unformattedEntryName) );
	if (!PathIsValid (entryName))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	return GetFileAttributes (entryName.c_str()) != INVALID_FILE_ATTRIBUTES;
#elif defined(OSM3D_PLATFORM_LINUX)
	struct stat st;
	return (!lstat(C_STRING(entryName), &st));
#else
#error	UNSUPPORTED PLATFORM FOR EntryExists.
#endif
}

/*
=================
GetFileContents
=================
*/
byte * GetFileContents (const std::string & unformattedFile, uint & fileSizeOut, uint extraBytes)
{
	// Will eventually need to implement OS-specific code so that we can access
	//	permissions data. fopen_s does provide situation-specific error reporting,
	//	and this should be implemented whenever the opportunity presents itself.
	//	You can look at:
	//
	// http://msdn.microsoft.com/en-us/library/t3ayayh1(v=vs.80).aspx
	//
	//  for Windows-specific return values.

	std::string file( TranslatePath (unformattedFile) );

	if (!PathIsValid (file))
		return false;

	FILE * fileHandle = NULL;
	size_t detectedFileSize;

#ifdef OSM3D_PLATFORM_WINDOWS
	fopen_s (&fileHandle, file.c_str(), "rb");
#else
	fileHandle = fopen(C_STRING(file), "rb");
#endif

	//	If the file could not be opened, output to the appropriate stream
	if (fileHandle == NULL)
	{
		Debug::Error(
			"FileIO.cpp | GetFileContents - Unable to access file \"%s \".",
			file.c_str()
			);
		fileSizeOut = 0;
		return NULL;
	}

	//	Get the file size
	fseek (fileHandle, 0, SEEK_END);
	detectedFileSize = ftell (fileHandle);
	fseek (fileHandle, 0, SEEK_SET);

	byte * resultData = NULL;

	//	Attempt to allocate the necessary amount of memory
	resultData = new byte [detectedFileSize + extraBytes];

	//	Attempt to read in all of the data from the file; if the amount of data read does not match
	//		the size of the file, then output to the appropriate stream.
	if (fread (resultData, 1, detectedFileSize, fileHandle) != detectedFileSize)
	{
		Debug::Error(
			"FileIO.cpp | GetFileContents - Unable to read all contents of file %s."
			"ferror: %i"
			"GetLastError: %i",
			file.c_str(), ferror( fileHandle ), GetLastError( )
			);

		delete resultData;
		fclose (fileHandle);
		return NULL;
	}

	fclose (fileHandle);

	fileSizeOut = (uint)detectedFileSize;

	return resultData;
}

/*
=================
GetFileContents
=================
*/
uint GetFileContents (const std::string & unformattedFile, byte * buffer, uint bufferSize)
{
	std::string file( TranslatePath (unformattedFile) );

	if (!PathIsValid (file))
		return false;

	if (buffer == NULL)
	{
		Debug::Error(
			"FileIO.cpp | GetFileContents - NULL buffer was passed for storage."
			"While accessing file: %s",
			file.c_str()
			);
		return 0;
	}

	if (bufferSize == 0)
	{
		Debug::Warning(
			"FileIO.cpp | GetFileContents - NULL Value was passed for bufferSize. Will attempt to write all data to the given buffer."
			);
	}

	FILE * fileHandle = NULL;
	uint detectedFileSize;
#ifdef OSM3D_PLATFORM_WINDOWS
	fopen_s (&fileHandle, file.c_str(), "rb");
#else
	fileHandle = fopen(C_STRING(file) ,"rb");
#endif

	//	If the file could not be opened, output to the appropriate stream
	if (fileHandle == NULL)
	{
		Debug::Error(
			"FileIO.cpp | GetFileContents - Unable to access file %s.",
			file.c_str()
			);
		return 0;
	}

	//	Version of function that allows a buffer to be taken as a parameter
	fseek (fileHandle, 0, SEEK_END);
	detectedFileSize = ftell (fileHandle);
	fseek (fileHandle, 0, SEEK_SET);

	if (detectedFileSize == 0)
	{
		fclose (fileHandle);
		return 0;
	}

	if (bufferSize < detectedFileSize)
	{
		Debug::Warning(
			"FileIO.cpp | GetFileContents - bufferSize is less than detectedFileSize. Only reading bufferSize bytes."
			"While attempting to access file: %s",
			file.c_str()
			);

		detectedFileSize = bufferSize;
	}

	uint freadResult = 0;
	freadResult = (uint)fread (buffer, 1, detectedFileSize, fileHandle);

	if (freadResult != detectedFileSize)
	{
		Debug::Warning(
			"FileIO.cpp | GetFileContents - Unable to read the requested amount of data from the file."
			"While attempting to access file: %s"
			"ferror result: %i",
			file.c_str(), ferror( fileHandle )
			);
	}

	fclose (fileHandle);

	return freadResult;
}

/*
=================
GetFileSize
=================
*/
uint GetFileSize (const std::string & unformattedFile)
{
	// Will eventually need to implement OS-specific code so that we can access
	//	permissions data. fopen_s does provide situation-specific error reporting,
	//	and this should be implemented whenever the opportunity presents itself.
	//	You can look at:
	//
	// http://msdn.microsoft.com/en-us/library/t3ayayh1(v=vs.80).aspx
	//
	//  for Windows-specific return values.

	std::string file( TranslatePath (unformattedFile) );

	if (!PathIsValid (file))
		return false;

	FILE * fileHandle = NULL;

#ifdef OSM3D_PLATFORM_WINDOWS
	fopen_s (&fileHandle, file.c_str(), "rb");
#else
	fileHandle = fopen(C_STRING(file) ,"rb");
#endif

	if (fileHandle == NULL)
	{
		Debug::Error(
			"FileIO.cpp | GetFileSize - Unable to access file %s.",
			file.c_str()
			);
		return -1;
	}

	fseek (fileHandle, 0, SEEK_END);
	uint fileSize = ftell (fileHandle);
	fclose (fileHandle);

	return fileSize;
}

/*
=================
WriteFile
=================
*/
bool WriteFile (const std::string & unformattedFile, byte * fileContents, uint fileSize)
{
	// Will eventually need to implement OS-specific code so that we can access
	//	permissions data. fopen_s does provide situation-specific error reporting,
	//	and this should be implemented whenever the opportunity presents itself.
	//	You can look at:
	//
	// http://msdn.microsoft.com/en-us/library/t3ayayh1(v=vs.80).aspx
	//
	//  for Windows-specific return values.

	std::string file( TranslatePath (unformattedFile) );

	if (!PathIsValid (file))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	OFSTRUCT of;
	HFILE hFile = OpenFile (file.c_str(), &of, OF_CREATE | OF_READWRITE);
	if (hFile == HFILE_ERROR)
	{
		Debug::Error(
			"FileIO.cpp | WriteFile - Could not write file %s."
			"GetLastError - %i",
			file.c_str(), GetLastError( )
			);
		return false;
	}

	DWORD bytesWritten;
	::WriteFile ((HANDLE)hFile, fileContents, fileSize, &bytesWritten, NULL);
	CloseHandle ((HANDLE)hFile);

	if (bytesWritten != fileSize)
	{
		Debug::Warning(
			"FileIO | WriteFile - Bytes written doesn't match buffer size."
			"GetLastError - ",
			GetLastError( )
			);
	}

#else
	FILE * fileHandle = NULL;
	fileHandle = fopen(C_STRING(file) ,"w+b");

	if (fileHandle == NULL)
	{
		Diagnostics::Error (true)
			.Append ("FileIO.cpp | ore::FileIO::WriteFile - ")
			.Append ("Could not write file " + file + ".")
			.Display ();

		return false;
	}

	fwrite ((void *)fileContents, 1, fileSize, fileHandle);
	fclose (fileHandle);
#endif

	return true;
}

/*
=================
WriteFile
=================
*/
bool WriteFile (const std::string & file, const std::string & fileContents)
{
	return WriteFile (file, (byte *)fileContents.c_str (), (uint)fileContents.size ()+1);
}

/*
=================
CreateDirectory
=================
*/
bool CreateDirectory (const std::string & unformattedDirectoryName)
{
	std::string directoryName( TranslatePath (unformattedDirectoryName) );
	if (!PathIsValid (directoryName))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
# ifdef UNICODE
	BOOL result = CreateDirectoryW( directoryName.c_str(), nullptr );
# else
	BOOL result = CreateDirectoryA( directoryName.c_str(), nullptr );
# endif
	if (result == FALSE)
	{
		Debug::Error(
			"FileIO.cpp | CreateDirectory - Unable to create directory %s"
			"GetLastError - ",
			GetLastError( )
			);
	}
	return result != FALSE;
#elif defined(OSM3D_PLATFORM_LINUX)
	if(mkdir(C_STRING(directoryName), S_IRUSR | S_IWUSR))
	{
		Diagnostics::Error (true)
			.Append ("FileIO.cpp | ore::FileIO::CreateDirectory - Unable to create directory ").Append (directoryName)
			.Display ();

		return false;
	}

	return true;
#else
#error Unsupported platform for CreateDirectory.
#endif
}

/*
=================
CopyFile
=================
*/
bool CopyFile (const std::string & unformattedSourceFileName, const std::string & unformattedNewFileName)
{
	std::string sourceFileName( TranslatePath (unformattedSourceFileName) );
	std::string newFileName( TranslatePath (unformattedNewFileName) );

	if (!PathIsValid (newFileName))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	if (!FileExists (sourceFileName))
	{
		Debug::Error(
			"FileIO.cpp | CopyFile - Could not copy file \"%s\" - file does not exist."
			);
		return false;
	}

#ifdef UNICODE
	BOOL result = CopyFileW (TranslatePath (sourceFileName), TranslatePath (newFileName), true);
#else
	BOOL result = CopyFileA (sourceFileName.c_str(), newFileName.c_str(), true);
#endif
	if (result == FALSE)
	{
		Debug::Error(
			"FileIO.cpp | CopyFile - Could not copy file \"%s\" to location \"%s\"."
			"GetLastError( ) - %i",
			sourceFileName.c_str(), GetLastError( )
			);
	}

	return result != FALSE;
#else
#error Unsupported platform for CopyFile.
#endif
}

/*
=================
CopyFileOverwrite
=================
*/
bool CopyFileOverwrite (const std::string & unformattedSourceFileName, const std::string & unformattedNewFileName)
{
	std::string sourceFileName( TranslatePath (unformattedSourceFileName) );
	std::string newFileName( TranslatePath (unformattedNewFileName) );

	if (!PathIsValid (newFileName))
		return false;

#ifdef OSM3D_PLATFORM_WINDOWS
	if (!FileExists (sourceFileName))
	{
		Debug::Error(
			"FileIO.cpp | CopyFile - Could not copy file '%s' - file does not exist."
			);
		return false;
	}

	BOOL result = CopyFileA (sourceFileName.c_str(), newFileName.c_str(), false);
	if (result == FALSE)
	{
		Debug::Error(
			"FileIO.cpp | CopyFile - Could not copy file '%s' to location '%s'."
			"GetLastError( ) - ",\
			sourceFileName.c_str(), newFileName.c_str(), GetLastError( )
			);
	}

	return result != FALSE;
#else
#error Unsupported platform for CopyFile.
#endif
}

/*
=================
DeleteFile
=================
*/
bool DeleteFile (const std::string & unformattedFileName)
{
	std::string fileName( TranslatePath (unformattedFileName) );

#	ifdef OSM3D_PLATFORM_WINDOWS
	// Doesn't do any sort of detection for why the deletion might fail (yet),
	//	information on implementing this functionality can be found at:
	//
	//	http://msdn.microsoft.com/en-us/library/windows/desktop/aa363915(v=vs.85).aspx
	//

	return DeleteFileA (fileName.c_str()) != FALSE;
#	else
	// Doesn't do any sort of detection for why the deletion might fail (yet),
	//	information on implementing this functionality can be found at:
	//
	//	http://www.delorie.com/gnu/docs/glibc/libc_280.html
	//
	return unlink (fileName.c_str()) != -1;
#	endif
}

/*
=================
DeleteDirectory
=================
*/
bool DeleteDirectory (const std::string & unformattedDirectory)
{
	std::string directory( TranslatePath (unformattedDirectory) );

	if (!PathIsValid (directory))
		return false;
	int result;
#if defined(OSM3D_PLATFORM_WINDOWS)
	char * restructuredPath = new char [directory.size () + 2];
	memcpy (restructuredPath, directory.data (), directory.size ());
	memset (restructuredPath + directory.size (), NULL, 2);

	SHFILEOPSTRUCTA fo;
	ZeroMemory (&fo, sizeof (fo));
	fo.wFunc = FO_DELETE;
	fo.pFrom = restructuredPath;
	fo.fFlags = FOF_NO_UI | FOF_NOCONFIRMATION;
	result = SHFileOperationA (&fo);

	if (result != 0)
	{
		Debug::Warning(
			"FileIO.cpp | DeleteDirectory - SHFileOperation failed with code %i.",
			result
			);
		return false;
	}
#elif defined(OSM3D_PLATFORM_LINUX)
	result = rmdir(directory.c_str());
#endif

	return true;
}

/*
=================
FormatPath
=================
*/
std::string FormatPath( const std::string & unformattedPath )
{
	if (unformattedPath.size () == 0)
		return unformattedPath;

	std::string path( unformattedPath );

	for (unsigned int i = 0; i < path.size (); i++)
	{
		if (path [i] == '\\')
			path [i] = '/';
		if (path [i] == '"' || path [i] == '\'')
		{
			path.erase (i, 1);
			--i;
		}
	}

	if (path [path.size ()-1] == '/')
		path = path.substr (0, path.size ()-1);

	return path;
}

} // !IO