#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include	"Object.h"
#include	"SafeProgramming.h"
#include	"Platform.h"
#include	"MyString.h"
//#include	"WavRead.h"
#include	<windows.h>

// ----
// Store sound sample info.
class CSample : public CObject
{
public:
	CSample();
	~CSample();

	// ----
	bool	Init(	CPlatform*	_PlatformRef,
					CString*	_FileName, 
					bool		_PrependPath = true);

	// ----
	// A rather last minute method to load 
	// in and process mod sample data.
	bool	ModInit(FILE*			_FH,
					unsigned int	ReadBytes,
					int				LoopStart,
					int				LoopEnd);

	// ----
	short*	GetData() {return Data;}
	DWORD	GetDataLength()	{return DataLength;}

	short		LoopStart;
	short		LoopEnd;

protected:
	CPlatform	*PlatformRef;
	
	UNALIGNED	short		*Data;
	DWORD		DataLength;
};

#endif