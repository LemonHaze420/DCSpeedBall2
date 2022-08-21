#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#include	"SafeProgramming.h"
#include	"Object.h"
#include	<windows.h>
#include	"GameData.h"

#ifdef USE_GAPI
	#include	"gx.h"
#endif


#define	CONTROL_NO_ERROR	OBJECT_NO_ERROR


// The in game buttons.
enum EButton
{
	Up				= 0,
	Down,
	Left,
	Right,
	Select, 
	MouseSelect,			// Stylus
	Backup,
	Pause,
	
	// The following button definitions are for debug / development use only.
	TakePicture,			
	DebugQuickExit
};

#define	CONTROL_ERROR_NO_ERROR						OBJECT_NO_ERROR
#define CONTROL_ERROR_DONT_CARE_ABOUT_THAT_BUTTON	1

// ----
// Control input / processing, etc.
class CControls : public CObject
{
public:
	CControls();
	~CControls();

	// ----
	// Init the object.
	bool	Init();

	// ----
	// Process / update key state.
	bool	Process();
	
	// ----
	// Call this method before making calls to SetButton. 
	// State information is otherwise lost.
	bool	PrepareForExternalButtonSet();

	// ----
	// Set current button state.
	bool	SetButton(EButton _Button, bool	_State = true);

	// ----
	// Get current button state and a debounced version.
	bool	IsButtonPressed(EButton _Button);
	bool	IsButtonPressedDB(EButton _Button);

	// ----
	// Returns true if an button is pressed.
	bool	AreAnyKeysPressed();

	// ----
	// Returns the direction, as defined in game defines, 
	// for the direction the user is pressing on the pad.
	int		GetDirection();


protected:
	DWORD	PreviousButtonState;
	DWORD	CurrentButtonState;

#ifdef USE_GAPI
	GXKeyList	GAPIKeyList;
#endif
};

#endif
