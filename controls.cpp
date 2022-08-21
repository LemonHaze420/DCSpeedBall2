#include	"Controls.h"
#include	"Object.h"

#ifdef USE_GAPI
	#include	"gx.h"
#endif

CControls::CControls()
{
	CurrentButtonState	= 0;
	PreviousButtonState	= 0;
}

CControls::~CControls()
{
#ifdef USE_GAPI
	GXCloseInput();
#endif
}

// ----
// Init the object.
bool	CControls::Init()
{
	CurrentButtonState	= 0;
	PreviousButtonState	= 0;

#ifdef USE_GAPI
	GXCloseInput();

	bool ret = (GXOpenInput() == 1);
	if (ret)
		GAPIKeyList = GXGetDefaultKeys(GX_NORMALKEYS);
	else
		ErrorCode = GetLastError();

	return ret;
#else
	return true;
#endif
}

// ----
// Process / update key state.
bool	CControls::Process()
{
	// Store previous key states.
	PreviousButtonState = CurrentButtonState;

	// Start by assuming there won't be an error.
	ErrorCode	= CONTROL_NO_ERROR;

	// Using GAPI on an actual target device.
#if defined(USE_GAPI) && (!defined(_WIN32_WCE_EMULATION))

	// Set buttons according to the values returned by GetAsyncKeyState.
	SetButton(Up,				(GetAsyncKeyState(GAPIKeyList.vkUp)		& 0x8000) != 0);
	SetButton(Down,				(GetAsyncKeyState(GAPIKeyList.vkDown)	& 0x8000) != 0);
	SetButton(Left,				(GetAsyncKeyState(GAPIKeyList.vkLeft)	& 0x8000) != 0);
	SetButton(Right,			(GetAsyncKeyState(GAPIKeyList.vkRight)	& 0x8000) != 0);
	SetButton(Select,			(GetAsyncKeyState(GAPIKeyList.vkA)		& 0x8000) != 0);
	SetButton(MouseSelect,		(GetAsyncKeyState(VK_LBUTTON)			& 0x8000) != 0);
	SetButton(Backup,			(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000) != 0);;
	SetButton(Pause,			(GetAsyncKeyState(GAPIKeyList.vkStart)	& 0x8000) != 0);
	SetButton(DebugQuickExit,	(GetAsyncKeyState(GAPIKeyList.vkC)		& 0x8000) != 0);
#ifdef NOT_FINAL_BUILD
	//SetButton(TakePicture,		(	(GetAsyncKeyState(VK_LBUTTON)			& 0x8000)
	//							&&	(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000)));

	//SetButton(TakePicture,		(GetAsyncKeyState(GAPIKeyList.vkB)		& 0x8000) != 0);
	SetButton(DebugQuickExit,	(GetAsyncKeyState(GAPIKeyList.vkC)		& 0x8000) != 0);
#endif
	// Okay, do this if we are enumating.
#elif _WIN32_WCE_EMULATION 

	// Set buttons according to the values returned by GetAsyncKeyState.
	SetButton(Up,				(GetAsyncKeyState(VK_UP)			& 0x8000) != 0);
	SetButton(Down,				(GetAsyncKeyState(VK_DOWN)			& 0x8000) != 0);
	SetButton(Left,				(GetAsyncKeyState(VK_LEFT)			& 0x8000) != 0);
	SetButton(Right,			(GetAsyncKeyState(VK_RIGHT)			& 0x8000) != 0);
	SetButton(Select,			(GetAsyncKeyState(VK_SPACE)			& 0x8000) != 0);
	SetButton(MouseSelect,		(GetAsyncKeyState(VK_LBUTTON)		& 0x8000) != 0);
	SetButton(Backup,			(GetAsyncKeyState(VK_ESCAPE)		& 0x8000) != 0);
	SetButton(Pause,			(GetAsyncKeyState(VK_PAUSE)			& 0x8000) != 0);
#ifdef NOT_FINAL_BUILD
	SetButton(DebugQuickExit,	(GetAsyncKeyState(VK_HOME)			& 0x8000) != 0);
#endif

#endif
	return true;
}

// ----
// Call this method before making calls to SetButton. 
// State information is otherwise lost.
bool	CControls::PrepareForExternalButtonSet()
{
	PreviousButtonState = CurrentButtonState;
	return true;
}


// ----
// Set current button state.
bool	CControls::SetButton(EButton _Button, bool	_State)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		if (_State)
		{
			CurrentButtonState |= (1 << index);
		}
		else
		{
			DWORD mask = 0xFFFFFFFF;
			mask ^= (1 << index);
			CurrentButtonState &= mask;				
		}

		return true;
	}
	else
	{
		return false;
	}
}

// ----
// Get current button state and a debounced version.
bool	CControls::IsButtonPressed(EButton _Button)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		return ((CurrentButtonState & (1 << index)) > 0);
	}
	else
	{
		return false;
	}
}

bool	CControls::IsButtonPressedDB(EButton _Button)
{
	int index = (int)_Button;
	
	if (index < 31)
	{
		return	(	(CurrentButtonState & (1 << index))
				&&	(!(PreviousButtonState & (1 << index)))
					);
	}
	else
	{
		return false;
	}
}


// ----
// Returns true if an button is pressed.
bool	CControls::AreAnyKeysPressed()
{
	return (CurrentButtonState != 0);
}

// ----
// Returns the direction, as defined in game defines, 
// for the direction the user is pressing on the pad.
int		CControls::GetDirection()
{
	if (IsButtonPressed(Up))
	{
		if (IsButtonPressed(Left))
			return NW;

		if (IsButtonPressed(Right))
			return NE;

		return N;
	}
	
	if (IsButtonPressed(Down))
	{
		if (IsButtonPressed(Left))
			return SW;

		if (IsButtonPressed(Right))
			return SE;

		return S;
	}

	if (IsButtonPressed(Left))
			return W;

	if (IsButtonPressed(Right))
			return E;

	return STILL;
}
