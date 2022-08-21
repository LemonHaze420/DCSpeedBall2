#include	"GameWindow.h"

#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameData.h"
#include	<windows.h>

#include	"GameDefines.h"

extern FILE	*DFH;

CGameWindow::CGameWindow()
{
	WindowHandle	= NULL;
	HInstance		= NULL;
	Active			= false;
	JustActivated	= false;
	LogicalPalette	= NULL;

	SystemPalette	= NULL;

#ifdef USE_GRAPHICS_WRAPPER
	TheGraphicsObject	= NULL;
#endif

#ifdef USE_CONTROL_WRAPPER
	TheController		= NULL;
#endif
}

// ----
CGameWindow::~CGameWindow()
{
#ifdef USE_GRAPHICS_WRAPPER
	SAFELY_DELETE(TheGraphicsObject);
#endif

#ifdef USE_CONTROL_WRAPPER
	SAFELY_DELETE(TheController);
#endif
	char*	charstar = (char*)LogicalPalette;
	SAFELY_DELETE_ARRAY(charstar);
	LogicalPalette = NULL;

	SAFELY_DELETE_ARRAY(SystemPalette);
}

// ----
// Opens a window.
bool	CGameWindow::Init(	HINSTANCE	_hInstance,
							LPTSTR		_lpCmdLine,
							int			_nCmdShow,
							CPlatform*	_PlatformRef,
							CGameData*	_GameDataRef)
{
	DEBUG_LOCALE("Entered method...");

#ifdef USE_GRAPHICS_WRAPPER
	SAFELY_DELETE(TheGraphicsObject);
	NEW(TheGraphicsObject, CGraphics());
#endif

	HWND		hWnd;
	WNDCLASS	wc;

	// Store instance handle.
	HInstance = _hInstance;

	DEBUG_LOCALE("New'd Graphics object");

	// Attempt to locate any existing application instance.
	if (hWnd = FindWindow(GAME_WINDOW_CLASS_NAME, GAME_NAME))
	{
		// Store the handle of the currently active window.
		HWND	oldhWnd = GetActiveWindow();

		// Deactivate the currently active window, then 
		// activate the other instance of this program. 
		// And finally quit.
		PostMessage(oldhWnd, WM_ACTIVATE, WA_INACTIVE, (long)hWnd);
		PostMessage(hWnd, WM_ACTIVATE, WA_ACTIVE, (long)oldhWnd);
		DEBUG_LOCALE("Another instance is already running");
		return true;
	}

	// Either no previous instance, or calls failed. 
	// Go ahead with creating new window.
	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= (WNDPROC)WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= HInstance;
	wc.hIcon			= NULL;
	wc.hCursor			= 0;
	wc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= GAME_WINDOW_CLASS_NAME;

	if (RegisterClass(&wc))
	{
		DEBUG_LOCALE("Registered window class");

#ifdef _WIN32_WCE
		hWnd = CreateWindow(GAME_WINDOW_CLASS_NAME, 
							GAME_NAME, 
							WS_VISIBLE,
							0,								// Full screen display.
							0, 
							GetSystemMetrics(SM_CXSCREEN), 
							GetSystemMetrics(SM_CYSCREEN), 
							NULL, 
							NULL, 
							HInstance, 
							NULL);
#else
		hWnd = CreateWindow(GAME_WINDOW_CLASS_NAME, 
							GAME_NAME, 
							WS_VISIBLE,
							0,								// Emulate a 240x320 display.
							0, 
							240, 
							320,
							NULL, 
							NULL, 
							HInstance, 
							NULL);

#endif

		DEBUG_LOCALE("Called CreateWindow");

		// Quit if failed.
		if (!hWnd)
		{	
			DEBUG_LOCALE("Failed to open window");
			return false;
		}
		

		// Show window
		ShowWindow(hWnd, _nCmdShow);
		DEBUG_LOCALE("Called ShowWindow");

		UpdateWindow(hWnd);
		DEBUG_LOCALE("Called UpdateWindow");

		// ----
		// Palette
		// 1. Determine if we need to generate a palette or not.
		DEBUG_LOCALE("Calling GetDeviceCaps");
		if (GetDeviceCaps(GetDC(hWnd), RASTERCAPS) & RC_PALETTE)
		{
			DEBUG_LOCALE("This is a palette mode screen (Shouldn't be?)");

			//if (_PlatformRef->GetMachineType() == EM_HP_Jornada545)
			//{
			//	MessageBox(hWnd, TEXT("This device reports as a Jornada545, in the 8bit display mode.\n\nSpeedball2 will run in this mode, however, you may enjoy a better gaming experience if you change the device to use 16bit colour. The option can be access via the 'HP settings' program, usually located on the 'Start' menu."), GAME_NAME, MB_OK);
			//}

			PaletteSize = GetDeviceCaps(GetDC(hWnd), SIZEPALETTE);
			if (PaletteSize < 256)
			{
				DEBUG_LOCALE("Small palette");
				return false;	// Palette too small 
			}

			SAFELY_DELETE_ARRAY(SystemPalette);
			SystemPalette = (PALETTEENTRY*)new char[sizeof(PALETTEENTRY) * PaletteSize];
			GetSystemPaletteEntries(	GetDC(hWnd),
										0, 
										PaletteSize,
										SystemPalette);

			/*
			LogicalPalette = (LOGPALETTE*)new char[sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * PaletteSize];
			if (LogicalPalette)
			{
				// 2. Set up a palette then.
				LogicalPalette->palNumEntries	= (WORD)PaletteSize;	// Hopefully 256.
				LogicalPalette->palVersion		= (WORD)0x0300;

				for (int i = 0; i < PaletteSize; i++)
				{
					LogicalPalette->palPalEntry[i].peRed	= (i & 0xE0);
					LogicalPalette->palPalEntry[i].peGreen	= ((i & 0x1C) << 3);
					LogicalPalette->palPalEntry[i].peBlue	= ((i & 0x03) << 6);
					LogicalPalette->palPalEntry[i].peFlags	= 0;	// Flags are ignored on CE, and must be 0.
				}

				// 3. Create a palette
				Palette = CreatePalette(LogicalPalette);	// Palette is a class member now.
				if (Palette)
				{
					//if (!SetPaletteEntries(Palette, 0, 256, PaletteArray))
					//{
					//	MessageBox(hWnd, TEXT("Can't set palette!"), GAME_NAME, MB_OK);
					//}

					//wsprintf(buffer, TEXT("There are %d colours in the logical palette."), GetPaletteEntries(Palette, 0, 256, PaletteArray));
					//MessageBox(hWnd, buffer, GAME_NAME, MB_OK);

					if (SelectPalette(	GetDC(hWnd),
										Palette,
										false))		// Force background is ignored by CE.
					{
						if (RealizePalette(GetDC(hWnd)) == GDI_ERROR)
						{
						//	delete[] LogicalPalette;
							return false;
						}

						// Apparently may need to call it a second time before it takes effect on the display.
						if (RealizePalette(GetDC(hWnd)) == GDI_ERROR)
						{
						//	delete[] LogicalPalette;
							return false;
						}
					}
					else
					{
						//delete[] LogicalPalette;
						//MessageBox(hWnd, TEXT("Failed to select palette"), GAME_NAME, MB_OK);
					}
				}
				else
				{
					//delete[] LogicalPalette;
					return false;	// Failed to create palette.
				}
			}
			else
				return false; // Failed to allocate memory for palette. 
			*/
		}

		//ShowWindow(hWnd, _nCmdShow);
		DEBUG_LOCALE("Called GetDeviceCaps");
		
		UpdateWindow(hWnd);
		DEBUG_LOCALE("Called UpdateWindow");

		if (TheGraphicsObject)
		{
			DEBUG_LOCALE("TheGraphicsObject is valid (at least non zero).");
		}
		else
		{
			DEBUG_LOCALE("TheGraphicsObject is NULL.");
		}

		if (hWnd)
		{
			DEBUG_LOCALE("hWnd is valid (at least non zero).");
		}

		if (_GameDataRef)
		{
			DEBUG_LOCALE("_GameDataRef is valid (at least non zero).");
		}

		if (_PlatformRef)
		{
			DEBUG_LOCALE("_PlatformRef is valid (at least non zero).");
		}

#ifdef USE_GRAPHICS_WRAPPER
		SAFELY_CONTROL(if, TheGraphicsObject, Init(hWnd, Palette, _GameDataRef, _PlatformRef), ==, false)
		{
			DEBUG_LOCALE("Failed to Init Graphics object");
			return false;
		}
		else
		{
			TheGraphicsObject->AssignSysPalette(SystemPalette, PaletteSize);
		}
		DEBUG_LOCALE("Inited graphics okay");
#endif

#ifdef USE_CONTROL_WRAPPER
		NEW(TheController, CControls());
		SAFELY_CALL(TheController, Init());
#endif

		return true;
	}
	DEBUG_LOCALE("Failed to register window class!");
	return false;
}

// ----
// Static callback method used to process windows messages.
LRESULT CALLBACK CGameWindow::WindowProc(	HWND	_hWnd,	
											UINT	_uMsg, 
											WPARAM	_wParam, 
											LPARAM	_lParam)
{
	switch (_uMsg)
    {
		// - Life control ---------------------------------
		// Basically, it is very easy for an application 
		// to lose focus, and some PocketPCs make it 
		// difficult for the user to return to a running 
		// application. Therefore, the application may 
		// remain running for days without the user being 
		// aware of the fact. With the systems limited 
		// resources we need to handle Windows messages 
		// correctly, especially WM_CLOSE and WM_HIBERNATE.

		// ----------------------------
		// Creation / Destruction
		// ----------------------------

		case WM_CREATE:
			// This code will be executed before the 
			// Windows create call returns. 
			break;

		case WM_CLOSE:
			// CE wants us to quit, and will kill this task 
			// in 8 seconds if we do not stop gracefully.

			// Don't need to save data, etc, yet.
			//MessageBox(NULL, TEXT("Destroy window"), TEXT("Speedball2"), MB_OK);
			DestroyWindow(_hWnd);
			PostQuitMessage(0);
			break;

		case WM_DESTROY:
			PostMessage(_hWnd, WM_MAIN_THREAD_DESTROY, _wParam, _lParam);
			break;

		// ----------------------------
		// Hibernation / Activation
		// ----------------------------

		case WM_ACTIVATE:
			// Should get this message when coming out of 
			// hibernation and when the application is 
			// reactivated.
	
			PostMessage(_hWnd, WM_MAIN_THREAD_ACTIVATE, _wParam, _lParam);

			// Check to see that this is an activation 
			// message (and not a deactivation message). 
			if ((LOWORD(_wParam) == WA_ACTIVE) || 
				(LOWORD(_wParam) == WA_CLICKACTIVE))
			{
				// Attempt to, and probably fail to, set 
				// this as the active window.
				if (!SetForegroundWindow(_hWnd))
				{
					OutputDebugString(TEXT("Activation message!"));
					ShowWindow(_hWnd, SW_SHOW);
					UpdateWindow(_hWnd);
				}
				PostMessage(_hWnd, WM_PAINT, 0, 0);

				return false;
			}
			return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
		
		case WM_ENABLE:
			// The window has been enabled or disabled.
			// The state (i.e. whether it is enabled or 
			// disabled) can be obtained using the 
			// following expression -- (BOOL)wParam;
			//return 0;
			break;
		
#ifdef _WIN32_WCE
		case WM_HIBERNATE:
			// Windows wants us to return as many 
			// resources as possible. Probably best if we 
			// release all resources when we lose focus 
			// anyway. 
			break;
#endif


		case WM_KILLFOCUS:
			// Once GAPI is working we MUST call GXSuspend() 
			// here or otherwise the user is not going to 
			// be able to control the device. -  we would 
			// still have control of the screen and the 
			// function keys. 
#ifdef USE_GAPI
			GXSuspend();
#endif
			break;


		case WM_SETFOCUS:
			// And always call GXResume() here.
#ifdef USE_GAPI			
			if (!GXResume())
#else
			if (false)
#endif
			{
				//MessageBox(NULL, TEXT("WM_SETFOCUS"), TEXT("Speedball2"), MB_OK);
				PostQuitMessage(0);
			}
			else
			{
				// Okay, GAPI restored the display. 
				// Send a message WM_PAINT message to self.
				PostMessage(_hWnd, WM_PAINT, 0, 0);
			}
			break;

		case WM_COMMAND:
			// Process menu input - although there isn't one at the moment.
			break;

		case WM_LBUTTONDOWN:
			//PostQuitMessage(0); // For safety for now.
			break;

		case WM_LBUTTONUP:
			break;

		//case WM_RBUTTONDOWN:
		//	MessageBox(NULL, TEXT("Right button"), TEXT("Speedball2"), MB_OK);
		//	PostQuitMessage(0);
		//	break;
		
		case WM_RBUTTONUP:
			break;

		case WM_MOUSEMOVE:
 			break;

		default:
			return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	return false;
}


#ifdef USE_GRAPHICS_WRAPPER
// ----
// Return a handle to the GAPI wrapper object.
CGraphics*	CGameWindow::GetGraphicsObject()
{
	return TheGraphicsObject;
}
#endif


#ifdef USE_CONTROL_WRAPPER
// ----
// Return a handle to the GAPI wrapper object.
CControls*	CGameWindow::GetControlObject()
{
	return TheController;
}
#endif


// ----
// Process Windows(R) messages. 
// Returns true to indicate all is okay. 
// Returns false if the program should exit.
// This method will block execution if the app is minimised / lost focus.
bool	CGameWindow::Process()
{
	BOOL	Continue;
	MSG		Msg;

	// Obviously the program is running and hence not just been activated. 
	JustActivated = false;

	// Update controls.
	SAFELY_CALL(TheController, Process());

	// Start by invalidating mouse pointer location. 
	// (Unless the new feature to disable this is set).
	// If the stylus is on the screen then it will be updated.
	if (!TheController->IsButtonPressed(MouseSelect))
	{
	//	MousePointer.x = -1;
	//	MousePointer.y = -1;
	}

	// This loop has two states. One for when the 
	// application is active and another for when the app 
	// has been deactivated. The first basically peeks for 
	// messages and dispatches them to the WinProc function
	// for processing. The second state involves using the 
	// GetMessage function which blocks until the function 
	// can return a message. The loop can not be exited
	// when it is in the second state, unless a WM_QUIT 
	// message is received. The second state is a wait 
	// state, for when the program has been deactivated. 
	// The loop changes between states when it processes a 
	// WM_MAIN_THREAD_ACTIVATE message. This is 
	// effectively a custom name for the WM_ACTIVATE 
	// message typically sent directly to the WinProc 
	// function. It would be safe to process the message 
	// using the DispatchMessage function, however, there 
	// is no point doing so.
	Continue = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
	while (Continue)
	{
		// Activate and deactivate the application - should stop 
		// the program running when it is deactivated.
		switch (Msg.message)
		{
		case WM_MAIN_THREAD_ACTIVATE:
		case WM_ACTIVATE:
			
			// Force the display to refresh.
			SAFELY_CALL(TheGraphicsObject, ForceRedraw());

			Active =	(	(LOWORD(Msg.wParam) != WA_INACTIVE) );
					//	&& 	(!HIWORD(Msg.wParam))	// Minimised!
					//	); 

			// Will be set to true if the program has just been activated. 
			JustActivated = Active;
			
			// Just been reactiveated - update controls.
			SAFELY_CALL(TheController, Process());
			if (!TheController->IsButtonPressed(MouseSelect))
			{
				MousePointer.x = -1;
				MousePointer.y = -1;
			}
			break;

		// Destroy the window.
		case WM_MAIN_THREAD_DESTROY:
		case WM_DESTROY:
#ifdef USE_GRAPHICS_WRAPPER
			SAFELY_DELETE(TheGraphicsObject);
#endif
#ifdef USE_CONTROL_WRAPPER
			SAFELY_DELETE(TheController);
#endif
			PostQuitMessage(0);
			break;

		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
			//fwKeys = wParam;  
			MousePointer.x = LOWORD(Msg.lParam);
			MousePointer.y = HIWORD(Msg.lParam);
			break;

		case WM_LBUTTONUP:
			//fwKeys = wParam;  
			MousePointer.x = -1;//LOWORD(Msg.lParam);
			MousePointer.y = -1;//HIWORD(Msg.lParam);
			break;

		default:
			// Translate and dispatch the message
			//if (!(m_pMainWindow && TranslateAccelerator(m_pMainWindow->GetHwnd(), m_hAccel, &msg)))
			{
				TranslateMessage(&Msg);
				DispatchMessage(&Msg);
			}
			break;
		}

		// Get next message. 
		if (Active)
		{
			Continue = PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE);
			if (Msg.message == WM_QUIT)
			{
				//MessageBox(NULL, TEXT("WM_QUIT"), TEXT("Speedball2"), MB_OK);
				return false;
			}
		}
		else
		{
			Continue = GetMessage(&Msg, NULL, 0, 0);
			if (!Continue) // i.e. the message was WM_QUIT.
			{
				//MessageBox(NULL, TEXT("WM_QUIT"), TEXT("Speedball2"), MB_OK);
				return false;
			}
		}
    }

	return true;
}

// ----
// Returns the location of the mouse pointer.
// -1, -1 means there is no mouse.
POINT*	CGameWindow::GetMousePoint()
{
	return &MousePointer;
}

// ----
void	CGameWindow::InvalidateMouse()
{
	MousePointer.x = -1;
	MousePointer.y = -1;
}

// ----
// Returns true if the program has just woken up.
bool	CGameWindow::GetJustActivated()
{
	return JustActivated;
}

