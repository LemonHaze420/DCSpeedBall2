#ifndef	_GAMECONTROL_H_
#define _GAMECONTROL_H_

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Match.h"
#include	"FrontEnd.h"
#include	"Platform.h"
#include	"SoundSystem.h"


// ----
// Enumerated type for game state.
#ifdef DEMO_BUILD
enum EGameState
{
	GameStarting		= 0,
	FrontEnd,
	GoingInToGame,
	ComingOutOfGame,
	DemoExitScreens,
	InGame,

	GameError			= -1
};
#else
enum EGameState
{
	GameStarting		= 0,
	FrontEnd,
	GoingInToGame,
	ComingOutOfGame,
	InGame,

	GameError			= -1
};
#endif


// ----
// Controls the entire show.
// Init to intialise data, run to start the game and cleanup to close.
// Other methods are messaged internally.
class CGameControl : public CObject
{
public:
	CGameControl();
	~CGameControl();

	// ----
	// Sets up game control data, opens application window.
	// Returns true only if everything succeeded.
	bool	Init(	HINSTANCE	_hInstance,
					LPTSTR		_lpCmdLine,
					int			_nCmdShow);

	// ----
	// Shutdown game, closing window, etc.
	void	CleanUp();

	// ----
	// Start the control game going. 
	// Doesn't return until user or the system quits the app.
	// Returns false to indicate that an error occured during execution.
	bool	Run();

	// ----
	// Returns true if the game stae was set to Game_Error
	bool	DidAnErrorOccur()	{return (GameState == GameError);}

	// ----
	bool	WasFileMissingError()	{return FileMissing;}

protected:

	// Windows data, etc.
	CGameWindow		*TheGameWindow;		// Game window.
	HINSTANCE		HInstance;

	CPlatform		*ThePlatform;		// Platform information.
	CSoundSystem	*TheSoundSystem;	// Sound system.
	CGameData		*TheGameData;		// Game data
	CMatch			*TheMatch;			// The actual in game match
	CFrontEnd		*TheFrontEnd;		// The frontend

	EGameState		GameState;

	bool			FileMissing;		// Set when a file goes missing.

#ifdef DEMO_BUILD
	CFont			*ThePopUpFont;
	CGraphicsData	*SplashGraphic;
	CGraphicsData	*BitmapBrothersLogoGraphic;
	CGraphicsData	*InfiniteVenturesLogoGraphic;
	CGraphicsData	*BroadswordLogoGraphic;
	int				ExitScreen;
#endif

};

#endif