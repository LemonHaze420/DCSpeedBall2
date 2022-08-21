#ifndef _OBJECT_H_
#define _OBJECT_H_

#pragma warning (disable:4514 4201)
#include	<windows.h>

#define	OBJECT_NO_ERROR		0

// ----
// Base class for most classes
class CObject
{
public:
	CObject();
	~CObject();


	// ----
	// Return the object count.
	static int GetObjectCount();

	// ----
	TCHAR*	GetCreationInfo();

	// ----
	void	SetCreation(char* _VarName, char* _Constructor, char* _File, int _Line);

	// ----
	static	void	OutputObjectList();

	// ----
	// Returns an internally stored error code.
	// The error code is particular to a given class. 
	virtual	DWORD	GetErrorCode();

	// ----
	// Returns a string representation for the object's data.
	// WARNING: This is only intended for debug use as it is gonna be slow!!!
	virtual TCHAR*	ToString();

protected:
	DWORD	ErrorCode;

#ifndef ULTRA_FAST_OBJECT
	TCHAR	*StringRepresentation;
	TCHAR	*CreationInfo;

	// A static (i.e. class variable) count of all the objects created. 
	// We'll see if everything is being cleared down properly!!! :o)
	static DWORD		ObjectCount;
	static DWORD		MaxObjectCount;
	DWORD				ObjectID;

	static	CObject*	FirstObject;
	CObject*			NextObject;
#endif

};

#endif