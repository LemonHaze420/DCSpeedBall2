#ifndef _MYSTRING_H_
#define _MYSTRING_H_

#include	"Object.h"
#include	<windows.h>
#include	<string.h>
#include	<stdio.h>

// ----
// Stores a string.
class CString : CObject
{
public:
	
	CString();
	CString(char* _String);
	CString(CString* _String);

	~CString();

	// ----
	// Release memory.
	void	Clear();

	// ----
	// Reads in the content of a file.
	// Return value indicates number of bytes read. (I don't know how the file was opened [text/binary])
	int		LoadString(FILE* _FH, int _ReadLength = 0);
	int		LoadEncodedString(FILE* _FH, int _ReadLength = 0);

	// ----
	// Overload inherited method
	TCHAR*	ToString();

	// ----
	TCHAR*	GetString();

	// ----
	UINT	GetStringLength();

	// ----
	// Returns the size of the string in bytes.
	UINT	GetStringSize();

	// ----
	// Simply remove the last character from the string. (Bit of a quick fix)
	void	DeleteLastChar();

#ifdef _UNICODE			// Unicode only method
	void	operator= (WCHAR* _String);
#endif

	void	operator= (char* _String);
	void	operator= (CString* _String);
	void	operator= (CString& _String);

	void	operator+= (char* _String);
	void	operator+= (CString* _String);
	void	operator+= (CString& _String);

	bool	operator== (char* _StringRight);
	bool	operator== (CString* _StringRight);

protected:
#ifdef DYNAMIC_CSTRING
#ifdef _UNICODE
	UNALIGNED TCHAR	*Buffer;
#else
	TCHAR	*Buffer;
#endif
#else
	TCHAR	Buffer[2048];
#endif
};

#endif