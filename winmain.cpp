#include "CompilerEnvironment.h"

// Pre written headers
#include <windows.h>

// Our headers.
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameWindow.h"
#include	"GameControl.h"
#include	"MyString.h"

#include	"GameDefines.h"
#include	"RegistryFunctions.h"
#ifndef NOT_FINAL_BUILD
#include	"excpt.h"
#endif

#define		MSG_INFO	MB_OK


FILE	*DFH;
LPEXCEPTION_POINTERS	GlobalExecptionPointerStore;


// ----
int CatchAll(LPEXCEPTION_POINTERS _a)
{
	GlobalExecptionPointerStore = _a;

	return EXCEPTION_EXECUTE_HANDLER;
}


// ----------------------------------------------------------------------------
// Program entry point.
int WINAPI WinMain(	HINSTANCE	_hInstance,
					HINSTANCE	_hPrevInstance,
					LPTSTR		_lpCmdLine,
					int			_nCmdShow)
{
	// No execptions yet.
//#ifndef NOT_FINAL_BUILD
	GlobalExecptionPointerStore = NULL;
//#endif

DEBUG_OPEN();
DEBUG_LOCALE();

	CGameControl*	TheGame;

	bool	ErrorOccured	= false;
	bool	FileMissing		= false;
	bool	LowMemWarning	= false;

	MEMORYSTATUS	StartMem;
	StartMem.dwLength = sizeof(StartMem);
	GlobalMemoryStatus(&StartMem);

	TCHAR memconvert[80];
	wsprintf(memconvert, TEXT("Free mem: %d\n"), StartMem.dwAvailPhys );
	OutputDebugString(memconvert);

	if (StartMem.dwAvailPhys < 4089446) // Demain 3.9Mb - actually uses a max of 3.7Mb, but give a little room for movement.
	{
		DEBUG_LOCALE("Low on memory");
		LowMemWarning = true;
		// Don't show a warning dialog. 
		// Try to run (may reactivate old instance). 
		// If we fail, we fail.

		//if (MessageBox(	NULL, 
		//				TEXT("Speedball2 is a large game, and requires more program memory to run correctly. Please free memory (close applications or delete old files) in order to run Speedball2.\n\nDo you wish to continue on regardless?"),
		//				GAME_NAME, 
		//				MB_YESNO | MB_DEFBUTTON2 | MB_ICONEXCLAMATION | MB_TOPMOST)
		//			== IDNO)
		//{
		//	exit(0);
		//}
	}

#if	1 //ndef NOT_FINAL_BUILD
	__try
	{
#endif
		DEBUG_LOCALE("About to create 'TheGame'");
		NEW(TheGame, CGameControl());
		SAFELY_CONTROL(if, TheGame, Init(_hInstance, _lpCmdLine, _nCmdShow), ==, true)
		{
			DEBUG_LOCALE("TheGame->Run");
			SAFELY_CALL(TheGame, Run());
			ErrorOccured = TheGame->DidAnErrorOccur();
			DEBUG_LOCALE("Finished execution");
			if (ErrorOccured)
			{
				DEBUG_LOCALE("Error occurred");
			}
		}
		else
		{
			ErrorOccured = true;
			DEBUG_LOCALE("NEW failed!!!");
		}

		// See if it was a missing file that caused the problem
		if (ErrorOccured)
			FileMissing = SAFELY_RETURN(TheGame, WasFileMissingError());

#if	1//ndef NOT_FINAL_BUILD
	}
	__except(CatchAll(GetExceptionInformation()))
	{
		__try
		{

			if (GlobalExecptionPointerStore)
			{
				FILE* FH;
				SYSTEMTIME ExceptionTime;

				GetSystemTime(&ExceptionTime);

				TCHAR	convert[256];
				wsprintf(	convert, 
							TEXT("\\SB2Failure%d%d%d-%d%d%d%d.dat"), 
							ExceptionTime.wYear, 
							ExceptionTime.wMonth,
							ExceptionTime.wDay,
							ExceptionTime.wHour,
							ExceptionTime.wMinute,
							ExceptionTime.wSecond,
							ExceptionTime.wMilliseconds);
				
				// Open save file.
				FH = _wfopen(convert, TEXT("wb"));
				if (FH)
				{
					fwrite("CONTEXT\n", 8, 1, FH);
					fwrite(GlobalExecptionPointerStore->ContextRecord, sizeof(CONTEXT), 1, FH);
					EXCEPTION_RECORD	*ER = GlobalExecptionPointerStore->ExceptionRecord;
					while (ER)
					{
						fwrite("\nRECORD\n", 8, 1, FH);
						fwrite(ER->ExceptionRecord, sizeof(EXCEPTION_RECORD), 1, FH);
						ER = ER->ExceptionRecord;
					}

					fwrite("\nEND", 4, 1, FH);
					fclose(FH);
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			OutputDebugString(TEXT("\n\nFAILURE\nDied in first exeception handler. Giving up!\n"));
		}
		ErrorOccured = true;
	}
#endif
	
	// Take outside the exception handle.
	SAFELY_DELETE(TheGame);

	// Make this give the option to create an automatic error report.
	if (ErrorOccured)
	{
		if (LowMemWarning)
		{
			// Low memory warning ignored
			MessageBox(NULL, TEXT("Unfortunately SpeedBall2 failed as the system is low on free memory. Please close other applications or delete old files, then try running the game again."), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		}
		else if (FileMissing)
		{
			// File(s) missing
			//MessageBox(NULL, TEXT("Sorry, one (or more) file(s) required by SpeedBall2 are corrupt or missing.\n\nPlease check that the game is correctly installed.\n\nIf your problems persist, or you require further information then please email speedball2@broadsword.co.uk stating the nature of the problem."), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
			MessageBox(NULL, TEXT("Sorry, one (or more) file(s) required by SpeedBall2 are corrupt or missing.\n\nPlease check that the game is correctly installed."), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		}
		else
		{
			// Generic
			//MessageBox(NULL, TEXT("Sorry, an error occured whilst running SpeedBall2.\n\nPlease check that the game is correctly installed, and that the data files have not been moved or deleted.\n\nIf your problems persist, or you require further information then please email speedball2@broadsword.co.uk stating the nature of the problem."), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
			MessageBox(NULL, TEXT("Sorry, an error occured whilst running SpeedBall2.\n\nPlease check that the game is correctly installed, and that the data files have not been moved or deleted."), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		}
	}

#if 0
	// Print out a list of undeleted objects...
	OutputDebugString(TEXT("\n\n\nTAKE NOTE:\nA list of CObject based objects in existance at shutdown follows:"));
	CObject::OutputObjectList();

#ifdef NOT_FINAL_BUILD
#ifdef _UNICODE

	TCHAR buffer[256];
	swprintf(buffer, TEXT("%d objects not freed.\n\nA list of objects, and where they were created can be found by looking at the debug output.\n"), CObject::GetObjectCount());
	OutputDebugString(buffer);
	
	#ifdef NOT_FINAL_BUILD
	if (CObject::GetObjectCount())
		MessageBox(NULL, buffer, GAME_NAME, MB_OK);
	#endif
#endif
#endif

#endif
	DEBUG_CLOSE();

	return 0;
}