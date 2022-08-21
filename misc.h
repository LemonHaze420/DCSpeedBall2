#ifndef		_MISC_H_
#define 	_MISC_H_

#include	<windows.h>
#include	"SafeProgramming.h"
#include	"MyString.h"

// ----
float GetTimeDifference(LPSYSTEMTIME _Now, LPSYSTEMTIME _Then);


// ----
// Test for button press 
#define	IsBP(_Button)												\
if ((ControllerRef) && (ControllerRef->IsButtonPressed(_Button)))

// ----
// Test for button press 
#define	IsBPDB(_Button)												\
if ((ControllerRef) && (ControllerRef->IsButtonPressedDB(_Button)))

// ----
// Quick tests to see if a particular button is pressed.
#define	IfSelect	IsBP(Select)
#define	IfBackup	IsBP(Backup)
#define	IfUp		IsBP(Up)
#define	IfDown		IsBP(Down)
#define	IfLeft		IsBP(Left)
#define	IfRight		IsBP(Right)
#define	IfPause		IsBP(Pause)

// ----
// Quick tests to see if a particular button is pressed - debounced.
#define	IfSelectDB	IsBPDB(Select)
#define	IfBackupDB	IsBPDB(Backup)
#define	IfUpDB		IsBPDB(Up)
#define	IfDownDB	IsBPDB(Down)
#define	IfLeftDB	IsBPDB(Left)
#define	IfRightDB	IsBPDB(Right)
#define	IfPauseDB	IsBPDB(Pause)

// ----
// Bit more complicated versions of the above. 
// Best to see where it is used and figure out its purpose.
#define	IfSelectOrMouseSelectDB(_MouseChangedPos)				\
if (	(ControllerRef)											\
	&&	(	ControllerRef->IsButtonPressedDB(Select)			\
		||	(	ControllerRef->IsButtonPressedDB(MouseSelect)	\
			&&	(_MouseChangedPos)								\
	)	)	)

// ----
#define SOUND_SELECT								\
SAFELY_CALL(SoundSystemRef, PlaySample(SelectSound, 1, 0.0f, true));


// ----
// Tells you the length of a file - "_filelength" is not provided for CE.
long GetFileSize(CString* _FileName);


// ----
// Returns true if the point is within the bounds of the 
// rect - the test is inclusive on the top left edge, and 
// exclusive on the bottom right.
bool	IsPointInRect(POINT* _Pos, RECT* _Rect);


// ----
// Rounds _Value to the nearest _Span.
// Modes 
//	-1	=> round down
//	0	=> round at mid point
//	1	=> round up
int		Round(int _Value, int _Snap = 10, int _Mode = -1);

// ----
// Fills the passed CString object with a ansii string form of the UNICODE 
// username value, obtained from the registry.
// A return of true indicated success, and false means no name was generated
bool	GetRegistrationString(CString*	RegString);

// ----
// Validate that a user's name and the key they provide match!
bool	ValidateRegistrationDetails(CString			*UserName, 
									unsigned short	UserKey,
									char			*RPN);

// ----
// Encrypt and decrypt the activation code;
unsigned long	EncryptCode(unsigned long aaaa);
unsigned long	DecryptCode(unsigned long aaaa);


// ----
// Returns whether the file could be opened.
bool	TestReadFile(CString*	_FileName);

// ----
#define	EndianInt16(a)		\
(							\
((a >> 8) & 0x00ff)	|		\
((a << 8) & 0xff00)			\
)

// ----
#define	EndianInt32(a)		\
(							\
((a >>24) & 0x000000ff)	|	\
((a >> 8) & 0x0000ff00)	|	\
((a << 8) & 0x00ff0000)	|	\
((a <<24) & 0xff000000)		\
)


#endif