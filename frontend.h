#ifndef _FRONTEND_H_
#define _FRONTEND_H_

#include	"SafeProgramming.h"
#include	"Object.h"
#include	<windows.h>
#include	"GameData.h"
#include	"GameWindow.h"
#include	"SoundSystem.h"
#include	"Sample.h"
#include	"Platform.h"
#include	"FrontEndCommonDefines.h"
										
#define		NUMBER_OF_SCANS				10
#define		STORY_SCREEN_GRAPHIC			"stadpic"

// Frontend state.
enum EFrontEndExitProcessState
{
	EFEEPS_Normal			= 0,		// Running around the menus. i.e. keep processing the frontend.
	EFEEPS_GoingInToGame,				// Need to start match.
	EFEEPS_Quiting,						// Quiting the program.

	EFEEPS_Error			= -1		// Memory all gone, etc.
};

// Modes for the menus
enum EFrontEndState
{
	// SPLASH SCREENS / TRANSITIONAL SCREENS.
	EFES_Starting								= 0,
	EFES_Splash,

	// GAME NAVIGATION
	EFES_Main,											// Main menu
	EFES_Single,										// Single player options
	EFES_Single_League,									// League options
	//EFES_Single_Cup,									// Cup options
	EFES_Single_Practice,								// Straight into game
	EFES_Demo,											// Demo.
	EFES_Load,											// Load game screen.
	EFES_Load_Failed,									// Load failed.
	EFES_Options,	
		EFES_Volume,													
		EFES_Buffer,
		EFES_Stylus,
		EFES_Difficulty,
	EFES_ConfirmAbandonCurrent,							// Quit the current game.
	EFES_ConfirmQuit,									// Confirm quit
	EFES_Credits,										// Credit screen.
	EFES_Register,										// NEW: Register product screen.
		EFES_Register_ThankYou,							//		Thanks for registering the game.
		EFES_Register_Mistake,							//		You made a mistake.
		EFES_Register_BadUserName,						//		You haven't set a username (that I can use).
	EFES_Quit,											// Actually quit
	EFES_Cheat,											// Cheat menu


	EFES_Help,											//Help menu and story
	EFES_Help_Overview,									//Overview of the game in text
	EFES_Help_Story,									//Game story -intro thingy
	EFES_Help_Controls,
	EFES_Help_Items,
	EFES_Help_Game_Modes,
	EFES_Help_Screens,
	EFES_Help_Pitch,


	// KNOCKOUT
	EFES_Single_KnockOut_GameOver,						// Been knocked out.
	EFES_Single_KnockOut_Fixtures,						// Fixtures screen.
	EFES_Single_KnockOut_Gym,							// Gym.
	EFES_Single_KnockOut_Game,							// Go into the game.
	EFES_Single_KnockOut_ComingOutOfGame,				// Return from the game.
	EFES_Single_KnockOut_Won,							// Won the knockout.


	// NEW LEAGUE 
	// Console screens.
	EFES_Single_League_New_Manager,						// Manager screen.
	EFES_Single_League_New_Gym_Fixtures,				// Fixtures screen from Gym.
	EFES_Single_League_New_Manager_Fixtures,			// Fixtures screen from Manager.
	EFES_Single_League_New_Transfer_Fixtures,			// Fixtures screen, from the Transfer screen.
	EFES_Single_League_New_Gym,							// Gym screen.
	EFES_Single_League_New_Statistics,					// Statistics
	EFES_Single_League_New_Tables,						// Divisions league rankings.
	EFES_Single_League_New_Transfer,					// Transfer screen.
														
	// Screens and modes to interface game				
	EFES_Single_League_New_Game,						// Go into the game.
	EFES_Single_League_New_ComingOutOfGame,				// Returning from the game. 
	EFES_Single_League_New_Results,						// Results screen, afterwhich the team's scores are updated.

	EFES_Single_League_New_PlayOff,						// Second to bottom in top div, or second to top in bottom div.
	EFES_Single_League_New_Promotion,					// Being promoted.
	EFES_Single_League_New_Relegate,					// Being relegated.
	EFES_Single_League_New_End_Season,					// Stay in same league.
	
	EFES_Single_League_New_Won,							// Won the league screen.
														
	// Misc	
	EFES_Single_League_New_Load_Successful,				// Load successful.
	EFES_Single_League_New_Save,						// Save.
	EFES_Single_League_New_Save_Failed,					// Save failed.
	EFES_Single_League_New_Save_Successful,				// Save successful.
	EFES_Single_League_New_Tables_Then_Test_Win,		// Test win state.
//	EFES_Single_League_New_Update,						// To change.


	// TEAM MANAGER LEAGUE
	// Console screens.
	EFES_Single_League_TeamManager_Manager,				// Manager screen.
	EFES_Single_League_TeamManager_Gym_Fixtures,		// Fixtures screen from Gym.
	EFES_Single_League_TeamManager_Manager_Fixtures,	// Fixtures screen from Manager.
	EFES_Single_League_TeamManager_Transfer_Fixtures,	// Fixtures screen, from the Transfer screen.
	EFES_Single_League_TeamManager_Gym,					// Gym screen.
	EFES_Single_League_TeamManager_Statistics,			// Statistics
	EFES_Single_League_TeamManager_Tables,				// Divisions league rankings.
	EFES_Single_League_TeamManager_Transfer,			// Transfer screen.
			
	// Game interface		
	EFES_Single_League_TeamManager_Game,				// Go into the game.
	EFES_Single_League_TeamManager_ComingOutOfGame,		// Returning from the game. 
	EFES_Single_League_TeamManager_Results,				// Results screen, afterwhich the team's scores are updated

	EFES_Single_League_TeamManager_PlayOff,				// Second to bottom in top div, or second to top in bottom div.
	EFES_Single_League_TeamManager_Promotion,			// Being promoted.
	EFES_Single_League_TeamManager_Relegate,			// Being relegated.
	EFES_Single_League_TeamManager_End_Season,			// Stay in same league.

	EFES_Single_League_TeamManager_Won,					// Won the league screen.
			
	// Misc		
	EFES_Single_League_TeamManager_Load_Successful,
	EFES_Single_League_TeamManager_Save,				// Save.
	EFES_Single_League_TeamManager_Save_Failed,
	EFES_Single_League_TeamManager_Save_Successful,
	EFES_Single_League_TeamManager_Tables_Then_Test_Win,// Test win state.
//	EFES_Single_League_TeamManager_Update,				// To change.


	// LEAGUE - LOADED GAME
	// These will re route to one of the above.
	EFES_Single_League_LoadGame_Load,
	EFES_Single_League_LoadGame_InsertDisk, 


	// CUP
	// Console screens.
	EFES_Single_Cup_New_Manager,						// Manager screen.
	EFES_Single_Cup_New_RoundN,							// Round n info screen.
	EFES_Single_Cup_New_Gym_Fixtures,					// Fixtures screen from Gym.
	EFES_Single_Cup_New_Manager_Fixtures,				// Fixtures screen from Manager.
	EFES_Single_Cup_New_Transfer_Fixtures,				// Fixtures screen, from the Transfer screen.
	EFES_Single_Cup_New_Gym,							// Gym screen.
	EFES_Single_Cup_New_Statistics,						// Statistics
	EFES_Single_Cup_New_Tables,							// Divisions league rankings.
	EFES_Single_Cup_New_Transfer,						// Transfer screen.

	// Game interface
	EFES_Single_Cup_New_Game,							// Go into the game.
	EFES_Single_Cup_New_ComingOutOfGame,				// Returning from the game. 
	EFES_Single_Cup_New_Results,						// Results screen, afterwhich the team's scores are updated
	EFES_Single_Cup_Won,								// Won the league screen.

	// Misc	
	EFES_Single_Cup_New_Load_Successful,				// Load.
	EFES_Single_Cup_New_Save,							// Save.
	EFES_Single_Cup_New_Save_Failed,					// Save.
	EFES_Single_Cup_New_Save_Successful,				// Save.

	

	EFES_Single_Cup_New_Tables_Then_Test_Win,			// Test win state.
//	EFES_Single_Cup_New_Update,							// To change.
	
	// CUP - LOADED GAME
	// Re route to one of the modes above.
	EFES_Single_Cup_LoadGame_Load,
	EFES_Single_Cup_LoadGame_InsertDisk,

	EFES_Error									= -1	// If all else fails, drop into the error state.
};


// ----
// Modes for the gym screen to be in.
enum EGymMode
{
	EFEGM_Individual		= 0,
	EFEGM_Group,
	EFEGM_Team,

	EFEGM_Error				= -1
};

// ----
enum EPlayerGroup
{
	EFEPG_Defence			= 0,
	EFEPG_Midfield,
	EFEPG_Attack,
	EFEPG_Substitute,

	EFEPG_Error				= -1
};

// ----
// Modes for the table screen to be in.
// WARNING:	Some parts of the program rely on the integer value equivelents for the enumerated values. (If that makes sense).
enum ETableMode
{
	EFETM_Overview			= 2,

	EFETM_DivisionOne		= 0,
	EFETM_DivisionTwo		= 1,

	EFETM_Error				= -1
};



/*definitions for all the keys on the keypad.... YAWN*/
#define KEY_1			0
#define KEY_2			1
#define KEY_3			2
#define KEY_4			3
#define KEY_5			4
#define KEY_6			5
#define KEY_7			6
#define KEY_8			7
#define KEY_9			8
#define KEY_0			9
#define KEY_Q			11
#define KEY_W			12
#define KEY_E			13
#define KEY_R			14
#define KEY_T			15
#define KEY_Y			16
#define KEY_U			17
#define KEY_I			18		
#define KEY_O			19
#define KEY_P			20
#define KEY_A			22
#define KEY_S			23
#define KEY_D			24
#define KEY_F			25
#define KEY_G			26
#define KEY_H			27
#define KEY_J			28
#define KEY_K			29
#define KEY_L			30
#define KEY_Z			34
#define KEY_X			35
#define KEY_C			36
#define KEY_V			37
#define KEY_B			38
#define KEY_N			39
#define KEY_M			40
#define KEY_RETURN_1	31
#define KEY_RETURN_2	41
#define KEY_RETURN_3	42
#define KEY_DELETE_1	10
#define KEY_DELETE_2	21
#define KEY_BLANK_1		32
#define KEY_BLANK_2		33
#define KEY_BLANK_3		43

// ----
class CFrontEnd : public CObject
{
public:
	CFrontEnd();
	~CFrontEnd();

	// ----
	// Init the object.
	bool Init(	CPlatform*		_PlatformRef,
				CGameData*		_GameDataRef,
				CGameWindow*	_GameWindowRef,
				CGraphics*		_GraphicsObjectRef,
				CControls*		_ControllerRef,
				CSoundSystem*	_SoundSystemRef);

	// ----
	// Run the front end.
	EFrontEndExitProcessState	Process();
	

	// ----
	void						ForceRedraw() {RedrawCountDown = 3;}

	// ----
	// Set the game's state - useful after finishing a game.
	// Note:	Only the following states are accepted.
	//			EFES_Main				- main menu.
	//	
	bool	SetReturnState(EFrontEndState	_State);

protected:

	// ----
	void	ConserveMemory(bool _Force = false);

	// ----
	// Update the state information.
	void	SetState(EFrontEndState _State);

	// ----
	// Default navigation for up and down options.
	void	DefaultVerticalNav(int _MaxNavItems, bool _ResetDialogStartTime = false);

	/*
	// No longer needed (well, out dated in design).
	// ----
	// Load front end data
	bool	LoadData();

	// ----
	// Free data
	bool	FreeData();

	// ----
	// Restore state
	bool	LoadState();

	// ----
	// Save state
	bool	SaveState();
	*/

	// ----
	// Show splash screen
	bool	Splash();

	// ----
	// Main menu
	bool	Main();

	// ----
	// Single player screen.
	bool	Single();

	// ----
	// Single player sub-screens
	//bool	Single_KnockOut();
	bool	Single_League();
	//bool		Single_League_NewGame();		// Sub-sub screens.
	//bool		Single_League_TeamManager();
	bool		Single_League_LoadGame();
//	bool	Single_Cup();
//	bool		Single_Cup_NewGame();
	bool		Single_Cup_RoundN();
	bool		Single_Cup_LoadGame();

	// ----
	// Prepare for a practice game.
	bool	Practice();		

	// ----
	// Managers interface
	bool	Manager();

	// ----
	// Transfer interface
	bool	Transfer();

	// ----
	// Shows division scores, etc.
	bool	Tables();
	bool	CupTable();

	// ----
	// Previous game stats
	bool	Stats();

	// ----
	// The training room
	bool	Gym();

	// ----
	// The fixtures screen
	bool	Fixtures();

	// ----
	// Results screen
	bool	Results();
	bool	CupResults();

	// ----
	// Load screen (not to be confused with LoadingScreen - which tells the user that the game is loading).
	bool	Load();

	// ----
	// Save the game
	bool	Save();

	// ----
	// Ready for the real game
	bool	Game();

	// ----
	// Demo game. (Cound be fun as need to go into game).
	bool	Demo();

	// ----
	bool	Options();

	bool	Volume();
	bool	Buffer();
	bool	Stylus();
	bool	Difficulty();

	// ----
	// Shows some credits.
	bool	Credits();

	// ----
	// Confirm abandon current game.
	bool	ConfirmAbandon();

	// ----
	// Confirm quit dialog.
	bool	ConfirmQuit();

	// ----
	bool	LoadingScreen();

	// ---
	// Help screen and game point
	bool	HelpMenu();

	//screen inside help menu
	bool	HelpMenuOverview();

	//Story intro thingy in help
	bool	HelpMenuStory();

	bool	HelpMenuControls();

	bool	HelpMenuItems();

	bool	HelpMenuGameModes();

	bool	HelpMenuScreens();

	bool	HelpMenuPitch();

	// ----
	// Processes a transaction.
	bool	PurchaseAssistant(	int	_InitialPlayer, 
								int	_PlayerCount);

	// ----
	// Draws the average stats for the players indexed.
	bool	DrawStatBars(	int	_InitialPlayer, 
							int	_PlayerCount);

	// ----
	// Draws bars for a star player
	bool	DrawStarPlayerStatBars(int	_StarPlayer);


	// ----
	// Register the product
	bool	Register();

	// ----
	// Registeration result screens
	bool	RegisterThankYou();
	bool	RegisterMistake();
	bool	RegisterBadUserName();

	// ----
	// For development use only of course!
	bool	Cheat();

	// ----
	// Get the front end back on track
	bool	ComingOutOfGame();

	// ----
	// Generic message screen.
	bool	Message();

	// ----
	// Update scores, etc...
	//bool	UpdateScoresAndReadyNextMatch();

	// ----
	// Render won screen
	bool	Won();

	// ----
	// Temp
	CString			TempString; // A class wide temp variable, saves overheads!
	CString			TempStringArray[4];
#define	MAX_NUM_STRING	40
	CString			NumStringArray[MAX_NUM_STRING];

	// ----
	// Menu state info.
	int				CurrentMenuPos;
	int				PreviousMenuPos;
	bool			DialogInitialised;
	SYSTEMTIME		TimeNow;
	SYSTEMTIME		DialogStartTime;
	float			DialogElapseTime;
	int				RedrawCountDown;
	
	// Used across several screens
	CString			MenuTextArray[MAX_MENU_ITEMS];		
	int				KeyMatrix[MAX_XKEYS][MAX_YKEYS];	// Key matrix for manager / gym screens
	int				SelectedPlayer;						
	CString			PositionCaptionArray[12];

	// Manager specific stuff
	int				FaceOffset;
	int				SelectedFace; // The highlighted face on the right hand panel.

	// Gym specific items
	EGymMode		GymMode;
	EPlayerGroup	SelectedGroup;
	int				SelectedBodyPart;			
	int				PartsList[8];
	int				AllSelect;

	// Tables specific items
	ETableMode		TableMode;		// See note with ETableMode declaration


	EFrontEndState	FrontEndState;
	EFrontEndState	PreviousFEState;
	CPlatform		*PlatformRef;
	CGameData		*GameDataRef;
	CGameWindow		*GameWindowRef;
	CGraphics		*GraphicsObjectRef;
	CControls		*ControllerRef;
	CSoundSystem	*SoundSystemRef;

	// Graphical resources
	CFont			*MainMenuFont;
	CFont			*TopSmallFont;
	CFont			*BottomSmallFont;
	CFont			*FixturesFont;
	CFont			*SmallLightFont;
	CFont			*SmallDarkFont;
	CFont			*LargeGymFont;
	CFont			*ThePopUpFont;
	CFont			*RegFont;

	CGraphicsData	*BackgroundGraphic;
	CGraphicsData	*BitmapBrothersLogoGraphic;
	CGraphicsData	*InfiniteVenturesLogoGraphic;
	CGraphicsData	*BroadswordLogoGraphic;
	CGraphicsData	*FacesGraphic;			// Faces
	CGraphicsData	*KeysGraphic;			// Keys
	CGraphicsData	*ArmourGraphic;			// Body
	CGraphicsData	*ShieldGraphic;			// Shield on its own.
	CGraphicsData	*ShieldLetterGraphic;	// Shield group overlays.
	CGraphicsData	*ThePickupGraphic;
	CGraphicsData	*ThePitchGraphic;		// for the help screen
	CGraphicsData	*TheBonusGraphic;		// p.t.o
	CGraphicsData	*LoadOverlay;			// Overlay for the loading screen
	CGraphicsData	*KeyboardGraphic;		// A keyboard image.
	CGraphicsData	*CreditsGraphic;		//
	CGraphicsData	*StoryScreenBackground;
	CGraphicsData	*ManagerGraphic;
	CGraphicsData	*GymGraphic;

#ifdef GYM_FUZZ
	// Misc graphics stuff.
	RECT			ScanArray[NUMBER_OF_SCANS];
#endif

	// Audio data
	CSample			*MovementSound;
	CSample			*SelectSound;
//	CSample			*CancelSound;

	CSample			*BackgroundSound;
};

#endif