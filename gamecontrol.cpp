#include	"GameControl.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"FrontEnd.h"
#include	"Misc.h"

#ifdef USE_GRAPHICS_WRAPPER
	#include	"GraphicsClasses.h"
#endif


extern FILE	*DFH;
// ----
// Controls the entire show.
// Init to intialise data, run to start the game and cleanup to close.
// Other methods are messaged internally.

CGameControl::CGameControl()
{
	GameState		= GameError;
	HInstance		= NULL;
	
	ThePlatform		= NULL;
	TheGameWindow	= NULL;
	TheSoundSystem	= NULL;
	TheGameData		= NULL;
	
	TheFrontEnd		= NULL;
	TheMatch		= NULL;

	FileMissing		= false;

#ifdef DEMO_BUILD
	SplashGraphic	= NULL;
	ThePopUpFont	= NULL;
	BitmapBrothersLogoGraphic	= NULL;	
	InfiniteVenturesLogoGraphic	= NULL;
	BroadswordLogoGraphic		= NULL;
#endif
}

CGameControl::~CGameControl()
{
	SAFELY_DELETE(TheMatch);
	SAFELY_DELETE(TheFrontEnd);
	SAFELY_DELETE(TheSoundSystem);
	SAFELY_DELETE(TheGameData);
	SAFELY_DELETE(TheGameWindow);
	SAFELY_DELETE(ThePlatform);

#ifdef DEMO_BUILD
	SAFELY_DELETE(SplashGraphic);
	SAFELY_DELETE(BitmapBrothersLogoGraphic);
	SAFELY_DELETE(InfiniteVenturesLogoGraphic);
	SAFELY_DELETE(BroadswordLogoGraphic);
#endif
}

// ----
// Sets up game control data, opens application window.
// Returns true only if everything succeeded.
bool	CGameControl::Init(	HINSTANCE	_hInstance,
							LPTSTR		_lpCmdLine,
							int			_nCmdShow)
{
	DEBUG_LOCALE();

	// Start by assuming we're gonna fail.
	GameState = GameError;

	// Create an object to store the platform information.
	SAFELY_DELETE(ThePlatform);
	NEW(ThePlatform, CPlatform());
	SAFELY_CONTROL(if, ThePlatform, Init(), ==, false)
		return false;

	DEBUG_LOCALE();

	// Create game data object
	SAFELY_DELETE(TheGameData);
	NEW(TheGameData, CGameData());
	SAFELY_CONTROL(if, TheGameData, LoadPreferences(ThePlatform), ==, false)
		return false;

	DEBUG_LOCALE();

	// Validate files.
	FileMissing = !(TheGameData->ValidateFiles());
	if (FileMissing)
	{
		return false;
	}

	DEBUG_LOCALE();

	// Create app window + start GAPI.
	NEW(TheGameWindow, CGameWindow());
	SAFELY_CONTROL(if, TheGameWindow, Init(_hInstance, _lpCmdLine, _nCmdShow, ThePlatform, TheGameData), ==, true)
	{	
		DEBUG_LOCALE();

		// Time to create soundsystem object.
		SAFELY_DELETE(TheSoundSystem);
		NEW(TheSoundSystem, CSoundSystem());
		SAFELY_CONTROL(if, TheSoundSystem, Init(ThePlatform, TheGameData->GetPreferences()->Volume), ==, false)
			return false;

		DEBUG_LOCALE();

//#if	defined(USE_GRAPHICS_WRAPPER) || defined(USE_GRAPHICS_WRAPPER)	// What? (I was clearly having a bad day when I wrote this!!!)
	#ifdef USE_GRAPHICS_WRAPPER
		CGraphics* LocalTheGraphicsObject = SAFELY_RETURN(TheGameWindow, GetGraphicsObject());
		if (LocalTheGraphicsObject)
		{
			DEBUG_LOCALE();

			SAFELY_CALL(LocalTheGraphicsObject, AssignPlatform(ThePlatform));
			SAFELY_CALL(LocalTheGraphicsObject, AssignSoundSystem(TheSoundSystem));
			
			// Okay, we're ready, so set the game state to starting.
			GameState = GameStarting;
			return true;
		}
	#endif
//#else
		GameState = GameStarting;
		DEBUG_LOCALE();
		return true;
//#endif
	}
	GameState = GameError;
	DEBUG_LOCALE();
	return false;
}

// ----
// Shutdown game, closing window, etc.
void	CGameControl::CleanUp()
{
}

// ----
// Start the control game going. 
// Doesn't return until user or the system quits the app.
// Returns false to indicate that an error occured during execution.
bool	CGameControl::Run()
{
	DEBUG_LOCALE("Okay, everything is in place. the game is about to start");

//	MessageBox(NULL, TEXT("Running - starting"), TEXT("Speedball2"), MB_OK);

#ifdef USE_GRAPHICS_WRAPPER
	CGraphics* LocalTheGraphicsObject	= SAFELY_RETURN(TheGameWindow, GetGraphicsObject());
#endif

#ifdef USE_CONTROL_WRAPPER
	CControls* LocalTheController		= SAFELY_RETURN(TheGameWindow, GetControlObject());
#endif

	SYSTEMTIME LastPressed;
	GetSystemTime(&LastPressed);

#ifdef NOT_FINAL_BUILD
	MEMORYSTATUS	StartMem;
	StartMem.dwLength = sizeof(StartMem);
	GlobalMemoryStatus(&StartMem);
	TCHAR memconvert[60];
	wsprintf(	memconvert, 
				TEXT("Free mem: %d\n"), 
				StartMem.dwAvailPhys);
	OutputDebugString(memconvert);
	long	LowestMem = StartMem.dwAvailPhys;
#endif

	bool	Continue	= true;
	while (Continue)
	{
		DEBUG_LOCALE("Main loop");

//		MessageBox(NULL, TEXT("Running - loop"), TEXT("Speedball2"), MB_OK);
		// For use with GetTickCount.
		DWORD	PreTickCount;
		DWORD	PostTickCount;

		PreTickCount = GetTickCount();
		//SYSTEMTIME pre;
		//GetSystemTime(&pre);

//		LARGE_INTEGER tock; 
//		if (QueryPerformanceCounter(&tock))
//			OutputDebugString(TEXT("HighResTimer Present\n"));
//		else
//			OutputDebugString(TEXT("HighResTimer Not Present\n"));


		// Emergency escape.
#if defined(USE_CONTROL_WRAPPER) && defined(NOT_FINAL_BUILD)
		SAFELY_CONTROL(if, LocalTheController, IsButtonPressedDB(DebugQuickExit), ==, true)
		{
			Continue = false;
		}
#endif

		// Process Windows(R) messages
		Continue &= SAFELY_RETURN(TheGameWindow, Process());
		if (	(	(LocalTheController) 
				&&	(LocalTheController->AreAnyKeysPressed()))
			||	(	(TheGameWindow) 
				&&	(TheGameWindow->GetJustActivated()))
			)
			GetSystemTime(&LastPressed);

		// Process sound
		SAFELY_CALL(TheSoundSystem, Process()); 

		switch (GameState)
		{
		// ------------------------------------------------
		// GameStarting: Starting the game.
		case GameStarting:
#ifndef DEMO_BUILD
			SAFELY_DELETE(TheFrontEnd);
			NEW(TheFrontEnd, CFrontEnd());

			// Assume that there is going to be an error.
			GameState = GameError;

			SAFELY_CONTROL(if, TheFrontEnd, Init(ThePlatform, TheGameData, TheGameWindow, LocalTheGraphicsObject, LocalTheController, TheSoundSystem), ==, true)
			{
				// Everything initialised correctly.
				GameState = FrontEnd;
				DEBUG_LOCALE("Going into frontend");
			}
#else
			// Do any set up code here
			GameState = FrontEnd;
			SAFELY_DELETE(SplashGraphic);
			NEW(SplashGraphic, CGraphicsData());
			SAFELY_CALL(SplashGraphic, Init(LocalTheGraphicsObject, &CString("splash"), ThePlatform));

			SAFELY_DELETE(ThePopUpFont);
			NEW(ThePopUpFont, CFont());
			SAFELY_CALL(ThePopUpFont, Init(ThePlatform, LocalTheGraphicsObject, &CString(POPUP_FONT)));

			SAFELY_DELETE(BitmapBrothersLogoGraphic);
			NEW(BitmapBrothersLogoGraphic, CGraphicsData());
			SAFELY_CALL(BitmapBrothersLogoGraphic, Init(LocalTheGraphicsObject, &CString("bblogo"), ThePlatform));

#if 0 // No logos yet.
			SAFELY_DELETE(InfiniteVenturesLogoGraphic);
			NEW(InfiniteVenturesLogoGraphic, CGraphicsData());
			SAFELY_CALL(InfiniteVenturesLogoGraphic, Init(LocalTheGraphicsObject, &CString("ivlogo"), ThePlatform));

			SAFELY_DELETE(BroadswordLogoGraphic);
			NEW(BroadswordLogoGraphic, CGraphicsData());
			SAFELY_CALL(BroadswordLogoGraphic, Init(LocalTheGraphicsObject, &CString("bslogo"), ThePlatform));
#endif

			LocalTheGraphicsObject->StartDraw();
			RECT Region;
			Region.top = 0;
			Region.left = 0;
			Region.bottom = 320;
			Region.right = 240;
			LocalTheGraphicsObject->DrawSprite(	0,
												0,
												SplashGraphic,
												Region);

			// Draw bitmap brothers logo.
			Region.left = 0;
			Region.top = 0;
			Region.right = 57;
			Region.bottom = 56;	
			SAFELY_CALL(LocalTheGraphicsObject, DrawSprite(4, 256, BitmapBrothersLogoGraphic, Region));

			// Render IF and BS logos here.


			LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
												&CString("Speedball2 Rolling Demo"), 
												(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
												260,
												TEXT_CENTRED);


			LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
												&CString("Broadsword Interactive"),
												(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
												270,
												TEXT_CENTRED);

			LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
												&CString("Copyright 1990 2001"), 
												(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
												280,
												TEXT_CENTRED);

			LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
												&CString("The Bitmap Brothers"),
												(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
												288,
												TEXT_CENTRED);


			LocalTheGraphicsObject->EndDraw();
			Sleep(2000);
#endif
			break;


		// ------------------------------------------------
		// FrontEnd: Process the front end.
		case FrontEnd:
#ifndef DEMO_BUILD
			EFrontEndExitProcessState ret;
			if (TheFrontEnd)
			{
				if (	(TheGameWindow) 
					&&	(TheGameWindow->GetJustActivated()))
					TheFrontEnd->ForceRedraw();

				ret = TheFrontEnd->Process();
				switch (ret)
				{
				case EFEEPS_Normal:
					break;

				case EFEEPS_GoingInToGame:
					GameState = GoingInToGame;
					break;

				case EFEEPS_Quiting:
					SAFELY_DELETE(TheFrontEnd);
					SAFELY_DELETE(TheMatch);
					Continue = false;
					break;

				case EFEEPS_Error:
					// Don't know yet...
					GameState = GameError;
					break;

				default:
					GameState = GameError;
					break;
				}
			}
#else
				SAFELY_CALL(TheGameData, Init(DEMO, ThePlatform));
				GameState = GoingInToGame;
#endif
			break;


		// ------------------------------------------------
		// GoingInToGame: Going from front end to game
		case GoingInToGame:
			SAFELY_DELETE(TheMatch);
			SAFELY_DELETE(TheFrontEnd);

			NEW(TheMatch, CMatch());
			SAFELY_CONTROL(if, TheMatch, Init(ThePlatform, TheGameData, TheGameWindow, LocalTheGraphicsObject, LocalTheController, TheSoundSystem), ==, true)
			{
				// Move to in game state.
				GameState = InGame;
				break;
			}

			// Only here if something went wrong
			GameState = GameError;
			break;


		// --------------------------------------------
		// InGame: In the game itself.
		case InGame:
			SAFELY_CONTROL(if, TheMatch, Process(), ==, true)
			{
				if (SAFELY_RETURN(TheMatch, IsGameAbandoned()))
				{
					// Assume that something is gonna go wrong.
					GameState = GameError;

					// The match is over, free the object.
					SAFELY_DELETE(TheMatch);

#ifndef DEMO_BUILD
					// Re-create front end
					SAFELY_DELETE(TheFrontEnd);
					NEW(TheFrontEnd, CFrontEnd());

					SAFELY_CONTROL(if, TheFrontEnd, Init(ThePlatform, TheGameData, TheGameWindow, LocalTheGraphicsObject, LocalTheController, TheSoundSystem), ==, true)
					{
						TheFrontEnd->SetReturnState(EFES_Main);
						GameState = FrontEnd;
					}
#else
					GameState = GameStarting;
#endif
					break;
				}

				if (SAFELY_RETURN(TheMatch, IsGameOver()))
				{
					GameState = ComingOutOfGame;
					break;
				}
				break;
			}

			GameState = GameError;			
			break;


		// --------------------------------------------
		// ComingOutOfGame: Coming out of the game.
		case ComingOutOfGame:

			// The match is over, free the object.
			SAFELY_DELETE(TheMatch);

#ifndef DEMO_BUILD
			// Re-create front end
			SAFELY_DELETE(TheFrontEnd);
			NEW(TheFrontEnd, CFrontEnd());

			// Assume that there is going to be an error.
			GameState = GameError;

			SAFELY_CONTROL(if, TheFrontEnd, Init(ThePlatform, TheGameData, TheGameWindow, LocalTheGraphicsObject, LocalTheController, TheSoundSystem), ==, true)
			{
				if (TheGameData)
				{
					switch (TheGameData->GetGameType())
					{
					case PRACTICE:
						GameState = FrontEnd;
						break;

					case KNOCKOUT:
						TheFrontEnd->SetReturnState(EFES_Single_KnockOut_ComingOutOfGame);
						GameState = FrontEnd;
						break;

					case LEAGUE:
						TheFrontEnd->SetReturnState(EFES_Single_League_New_ComingOutOfGame);
						GameState = FrontEnd;
						break;

					case LEAGUE_TEAM_MANAGER:
						TheFrontEnd->SetReturnState(EFES_Single_League_TeamManager_ComingOutOfGame);
						GameState = FrontEnd;
						break;

					case CUP:
						TheFrontEnd->SetReturnState(EFES_Single_Cup_New_ComingOutOfGame);
						GameState = FrontEnd;
						break;

					case DEMO:
						GameState = FrontEnd;
						break;
					}
				}
			}
#else
			GameState = DemoExitScreens;
			ExitScreen = 0;
#endif
			break;

#ifdef DEMO_BUILD
		case DemoExitScreens:
			if (ExitScreen == 0)
			{
				// Load
				SAFELY_DELETE(SplashGraphic);
				NEW(SplashGraphic, CGraphicsData());
				SAFELY_CALL(SplashGraphic, Init(LocalTheGraphicsObject, &CString("stadpic"), ThePlatform));
				ExitScreen++;
			}
			else
			{
				// Render
				LocalTheGraphicsObject->StartDraw();
				RECT Region;
				Region.top = 0;
				Region.left = 0;
				Region.bottom = 320;
				Region.right = 240;
				LocalTheGraphicsObject->DrawSprite(	0,
													0,
													SplashGraphic,
													Region);
#define	DEMO_TEXT_YPOS		100
#define	DEMO_TEXT_YSPACE	8


				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("Speedball2"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													DEMO_TEXT_YPOS + DEMO_TEXT_YSPACE * 0,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("If you wish to purchase a full"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													DEMO_TEXT_YPOS + DEMO_TEXT_YSPACE * 2,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("copy of Speedball2 please visit"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													DEMO_TEXT_YPOS + DEMO_TEXT_YSPACE * 3,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("www.portable-games.com  or ask at"),// or ask at your nearest pocket pc retailer"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													DEMO_TEXT_YPOS + DEMO_TEXT_YSPACE * 4,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("your nearest Pocket PC retailer."), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													DEMO_TEXT_YPOS + DEMO_TEXT_YSPACE * 5,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("Broadsword Interactive"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													270,
													TEXT_CENTRED);

				
				// Copyright notice
				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("Copyright 1990 2001"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													280,
													TEXT_CENTRED);

				LocalTheGraphicsObject->DrawString(	ThePopUpFont, 
													&CString("The Bitmap Brothers"), 
													(LocalTheGraphicsObject->GetDisplay()->cxWidth >> 1),
													288,
													TEXT_CENTRED);

				LocalTheGraphicsObject->EndDraw();
				ExitScreen++;
			}

			if (ExitScreen > 150)
			{
				Continue = false;
				SAFELY_DELETE(SplashGraphic);
				SAFELY_DELETE(ThePopUpFont);
			}
			break;
#endif

		// ------------------------------------------------
		// GameError: The joy of being in a bad position!
		case GameError:
			// Okay, I'll leave this for now!
			Continue = false;

			SAFELY_CALL(LocalTheGraphicsObject, StartDraw());
			SAFELY_CALL(LocalTheGraphicsObject, ClearScreen(255, 0, 0));
			SAFELY_CALL(LocalTheGraphicsObject, EndDraw());
			SAFELY_CALL(LocalTheGraphicsObject, EndDraw());
			break;


		// ------------------------------------------------
		// default: Just in case I screw up!
		default:
			// How the hell did the code execute to here!!!
			Continue = false;

			SAFELY_CALL(LocalTheGraphicsObject, StartDraw());
			SAFELY_CALL(LocalTheGraphicsObject, ClearScreen(255, 0, 255));
			SAFELY_CALL(LocalTheGraphicsObject, EndDraw());
			break;
		}
			
//		SYSTEMTIME post;
//		GetSystemTime(&post);

		PostTickCount = GetTickCount();

#ifdef LIMIT_FRAME_RATE
		if (PreTickCount > PostTickCount)	// The count rolls over after 49 days, this avoids a lock state.
			PreTickCount = GetTickCount();

		// Most definate hack to remove frame rate limiting
		// during team manager if the use has 
		// 'DebugQuickExit' held down.
		if (!	(	(TheGameData->GetGameType() == LEAGUE_TEAM_MANAGER)
				&&	(LocalTheController->IsButtonPressed(DebugQuickExit))))
		{
			while ((PostTickCount - PreTickCount) < (FRAME_LIMIT * 1000))
			{
				PostTickCount = GetTickCount();	// Eat those clock cycles.
			}
		}
#endif
		
#ifdef NOT_FINAL_BUILD

		// Monitor free memory
		StartMem.dwLength = sizeof(StartMem);
		GlobalMemoryStatus(&StartMem);
		if ((int)StartMem.dwAvailPhys < (int)LowestMem)
		{
			LowestMem = StartMem.dwAvailPhys;

			wsprintf(	memconvert, 
					TEXT("Free mem: %d\n"), 
					StartMem.dwAvailPhys);
			OutputDebugString(memconvert);
		}
		

		TCHAR	bu[256];
		//swprintf(bu, TEXT("Whole game loop took %f second(s).\n"), GetTimeDifference(&post, &pre));
		swprintf(bu, TEXT("Whole game loop took %f second(s).\n"), float(PostTickCount - PreTickCount) / 1000.0f);

		OutputDebugString(bu);

		if (TheMatch)
		{
			/*if (	(ThePlatform)
				&&	(ThePlatform->GetMachineType() == EM_HP_Jornada545))
			{
				if (GetTimeDifference(&post, &pre) >= 1.0f)
				{
					if (FPS != 0)
						TheMatch->FrameTime = 1.0f / (float)FPS;
					FPS = 0;
				}
				else
				{
					FPS++;
				}
			}
			else
			{
				TheMatch->FrameTime = GetTimeDifference(&post, &pre);
			}*/
			TheMatch->FrameTime = float(PostTickCount - PreTickCount) / 1000.0f;
		}

		// Take a picture?
		if (	(LocalTheController) 
			&& 	(LocalTheController->IsButtonPressedDB(TakePicture)))
		{
			SYSTEMTIME picTime;
			GetSystemTime(&picTime);

			char	convert[256];
			sprintf(convert, 
					"%d%d%d-%d%d%d%d.tga", 
					picTime.wYear, 
					picTime.wMonth,
					picTime.wDay,
					picTime.wHour,
					picTime.wMinute,
					picTime.wSecond,
					picTime.wMilliseconds);
			CString	Path = ThePlatform->GetPath(EP_Snapshot);
			Path += convert;

			LocalTheGraphicsObject->StartDraw();
			LocalTheGraphicsObject->SaveScreen(&Path);
			LocalTheGraphicsObject->EndDraw();
		}
#endif

		/*
		// Has the game been inactive too long?
		if (	(TheGameData->GetPreferences()->AutoSleepTimeOut > 0)
			&&	(!LocalTheController->AreAnyKeysPressed())
			&&	(GetTimeDifference(&post, &LastPressed) > TheGameData->GetPreferences()->AutoSleepTimeOut))
		{	
			// There is next to nothing you can do here. 
			// To let windows sleep you need it let it 
			// block. If it blocks it doesn't give you a 
			// mesasage to wake back up again. Thus, the
			// program sits there, with full control of 
			// the screen and buttons, waiting for Windows
			// to tell it to wake up. 
		}
		*/
	}

//	MessageBox(NULL, TEXT("Running - on way out"), TEXT("Speedball2"), MB_OK);

	return true;
}

