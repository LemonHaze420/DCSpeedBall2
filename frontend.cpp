#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"
#include	"RegistryFunctions.h"

// General observations:
//	are: I have assumed that the number of teams in a division is always an even (positive) number.
//	are: The layout has only been tested at 240, 320. 
//	are: Short team names must not exceed 10 chars in length. 


#define MAIN_MENU_FONT					"mainmenufont"
#define TOP_SMALL_FONT					"topsmallfont"
#define BOTTOM_SMALL_FONT				"bottomsmallfont"
#define	SMALL_LIGHT_FONT				"smalllightfont"
#define	SMALL_DARK_FONT					"smalldarkfont"
#define	REG_FONT						"regfont"
#define	ARMOUR_GRAPHIC					"armour"
#define	FACES_GRAPHIC					"faces"
#define	KEYS_GRAPHIC					"keys"
#define	SHIELD_GRAPHIC					"shield"
#define	SHIELD_LETTER_GRAPHIC			"shieldletters"
#define	PICKUP_IMAGE					"helppickup"
#define PITCH_GRAPHIC					"pitchhelp"
#define BONUS_GRAPHIC					"bonushelp"
#define	KEYBOARD_GRAPHIC				"keyboard"


#define	SPLASH_SCREEN_GRAPHIC			"splash"
#define	MANAGER_SCREEN_GRAPHIC			"manager"
#define	GYM_SCREEN_GRAPHIC				"gym"
#define	CUPS_SCREEN_GRAPHIC				"cups"
#define	PROMOTION_SCREEN_GRAPHIC		"promotion"
												
#define	TEXT_MENU_TITLE_MAIN			"SpeedBall II"
#define	TEXT_MENU_TITLE_SINGLE			"Select Match"

#define	TEXT_MENU_TITLE_SINGLE_KNOCKOUT	"Knockout"
#define	TEXT_MENU_TITLE_QUIT			"Quit"
#define	TEXT_MENU_TITLE_TABLES			"Leagues"
#define	TEXT_MENU_TITLE_RESULTS			"Results"
#define	TEXT_MENU_TITLE_OPTIONS			"Options"
#define	TEXT_MENU_TITLE_NROUND			"Cup Round"
#define	TEXT_MENU_TITLE_LEAGUE			"League"

// ----
// Front end sound name definitions.
#define	FES_MENU_MOVEMENT				"menumovement.uwd"
#define	FES_MENU_SELECT					"select.uwd"
#define	FES_MENU_BACKUP					"backup.uwd"
#define	FES_SPLASH						"splash.uwd"

// Macros to ease the playing of the samples.
#define	PLAY_SELECT						SAFELY_CALL(SoundSystemRef, PlaySample(SelectSound, 1, 0.0f, true));
#define	PLAY_BACKUP						SAFELY_CALL(SoundSystemRef, PlaySample(SelectSound, 1, 0.0f, true));
#define	PLAY_MOVEMENT					SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound, 1, 0.0f, true));


// Add this value to one of the values above to get the 'pressed' key image.
#define	LIT_KEY_ADDER					10

// ----
CFrontEnd::CFrontEnd()
{
	FrontEndState		= EFES_Starting;
	PlatformRef			= NULL;
	GameDataRef			= NULL;
	GameWindowRef		= NULL;
	GraphicsObjectRef	= NULL;
	ControllerRef		= NULL;
	GymMode				= EFEGM_Error;	// Must initialise correctly later!!!
	SelectedGroup		= EFEPG_Error;
	SelectedBodyPart	= -1;
	SelectedPlayer		= -1;
	TableMode			= EFETM_Error;	

	// Position captions.
	PositionCaptionArray[0]		= "CFwd";
	PositionCaptionArray[1]		= "RFwd";
	PositionCaptionArray[2]		= "LFwd";
	PositionCaptionArray[3]		= "CMid";
	PositionCaptionArray[4]		= "RMid";
	PositionCaptionArray[5]		= "LMid";
	PositionCaptionArray[6]		= "RDef";
	PositionCaptionArray[7]		= "LDef";
	PositionCaptionArray[8]		= "Goal";
	PositionCaptionArray[9]		= "Sub1";
	PositionCaptionArray[10]	= "Sub2";
	PositionCaptionArray[11]	= "Sub3";

	// Use index to get the 'word' form of a number.
	NumStringArray[0]			= "Zero";
	NumStringArray[1]			= "One";
	NumStringArray[2]			= "Two";
	NumStringArray[3]			= "Three";
	NumStringArray[4]			= "Four";
	NumStringArray[5]			= "Five";
	NumStringArray[6]			= "Six";
	NumStringArray[7]			= "Seven";
	NumStringArray[8]			= "Eight";
	NumStringArray[9]			= "Nine";

	// Clear the price list.
	memset((void*)&PartsList, 0, sizeof(PartsList));

	// Fonts
	MainMenuFont				= NULL;
	TopSmallFont				= NULL;
	BottomSmallFont				= NULL;
	FixturesFont				= NULL;
	SmallLightFont				= NULL;
	SmallDarkFont				= NULL;
	ThePopUpFont				= NULL;
	LargeGymFont				= NULL;
	RegFont						= NULL;

	// Graphics
	BackgroundGraphic			= NULL;
	BitmapBrothersLogoGraphic	= NULL;
	InfiniteVenturesLogoGraphic	= NULL;
	BroadswordLogoGraphic		= NULL;
	StoryScreenBackground		= NULL;
	FacesGraphic				= NULL;
	KeysGraphic					= NULL;
	ArmourGraphic				= NULL;
	ShieldGraphic				= NULL;
	ShieldLetterGraphic			= NULL;
	ThePickupGraphic			= NULL;
	ThePitchGraphic				= NULL;
	TheBonusGraphic				= NULL;
	LoadOverlay					= NULL;
	KeyboardGraphic				= NULL;
	CreditsGraphic				= NULL;
	ManagerGraphic				= NULL;
	GymGraphic					= NULL;

	// Sounds
	MovementSound				= NULL;
	SelectSound					= NULL;
//	CancelSound					= NULL;
								
	BackgroundSound				= NULL;
								
	CurrentMenuPos				= 0;
	PreviousMenuPos				= 0;
	DialogInitialised			= false;
	GetSystemTime(&TimeNow);
	DialogStartTime				= TimeNow;
	DialogElapseTime			= 0.0f;
}

// ----
CFrontEnd::~CFrontEnd()
{
	// Stop all sounds before deleting the samples.
	SAFELY_CALL(SoundSystemRef, StopAllSamples());

	// Fonts
	SAFELY_DELETE(MainMenuFont);
	SAFELY_DELETE(TopSmallFont);
	SAFELY_DELETE(BottomSmallFont);
	SAFELY_DELETE(FixturesFont);
	SAFELY_DELETE(SmallLightFont);
	SAFELY_DELETE(SmallDarkFont);
	SAFELY_DELETE(ThePopUpFont);
	SAFELY_DELETE(LargeGymFont);
	SAFELY_DELETE(RegFont);

	// Graphics
	SAFELY_DELETE(BackgroundGraphic);
	SAFELY_DELETE(StoryScreenBackground);
	SAFELY_DELETE(BitmapBrothersLogoGraphic);
	SAFELY_DELETE(InfiniteVenturesLogoGraphic);
	SAFELY_DELETE(BroadswordLogoGraphic);
	SAFELY_DELETE(FacesGraphic);
	SAFELY_DELETE(KeysGraphic);
	SAFELY_DELETE(ArmourGraphic);
	SAFELY_DELETE(ShieldGraphic);
	SAFELY_DELETE(ShieldLetterGraphic);
	SAFELY_DELETE(ThePickupGraphic);
	SAFELY_DELETE(ThePitchGraphic);
	SAFELY_DELETE(TheBonusGraphic);
	SAFELY_DELETE(LoadOverlay);
	SAFELY_DELETE(KeyboardGraphic);
	SAFELY_DELETE(CreditsGraphic);
	SAFELY_DELETE(ManagerGraphic);
	SAFELY_DELETE(GymGraphic);

	// Sounds
	SAFELY_DELETE(MovementSound);
	SAFELY_DELETE(SelectSound);
//	SAFELY_DELETE(CancelSound);

	SAFELY_DELETE(BackgroundSound);
}

// ----
bool CFrontEnd::Init(	CPlatform*		_PlatformRef,
						CGameData*		_GameDataRef,
						CGameWindow*	_GameWindowRef,
						CGraphics*		_GraphicsObjectRef,
						CControls*		_ControllerRef,
						CSoundSystem*	_SoundSystemRef)
{
	// ----
	// Store imported references
	PlatformRef			= _PlatformRef;
	GameDataRef			= _GameDataRef;
	GameWindowRef		= _GameWindowRef;
	GraphicsObjectRef	= _GraphicsObjectRef;
	ControllerRef		= _ControllerRef;
	SoundSystemRef		= _SoundSystemRef;


	// ----
	// Fonts
	SAFELY_DELETE(MainMenuFont);
	NEW(MainMenuFont, CFont());
	SAFELY_CONTROL(if, MainMenuFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(MAIN_MENU_FONT)), ==, false)
		return false;

	SAFELY_DELETE(TopSmallFont);
	NEW(TopSmallFont, CFont());
	SAFELY_CONTROL(if, TopSmallFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(TOP_SMALL_FONT)), ==, false)
		return false;

	SAFELY_DELETE(BottomSmallFont);
	NEW(BottomSmallFont, CFont());
	SAFELY_CONTROL(if, BottomSmallFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(BOTTOM_SMALL_FONT)), ==, false)
		return false;

	SAFELY_DELETE(SmallLightFont);
	NEW(SmallLightFont, CFont());
	SAFELY_CONTROL(if, SmallLightFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(SMALL_LIGHT_FONT)), ==, false)
		return false;

	SAFELY_DELETE(SmallDarkFont);
	NEW(SmallDarkFont, CFont());
	SAFELY_CONTROL(if, SmallDarkFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(SMALL_DARK_FONT)), ==, false)
		return false;

	SAFELY_DELETE(ThePopUpFont);
	NEW(ThePopUpFont, CFont());
	SAFELY_CONTROL(if, ThePopUpFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(POPUP_FONT)), ==, false)
		return false;

	SAFELY_DELETE(RegFont);
	NEW(RegFont, CFont());
	SAFELY_CONTROL(if, RegFont, Init(_PlatformRef, _GraphicsObjectRef, &CString(REG_FONT)), ==, false)
		return false;

	// ----
	// Images that are to live for the entire length of the front end object.

	SAFELY_DELETE(BackgroundGraphic);
	NEW(BackgroundGraphic, CGraphicsData());
	SAFELY_CONTROL(if, BackgroundGraphic, Init(_GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(BitmapBrothersLogoGraphic);
	NEW(BitmapBrothersLogoGraphic, CGraphicsData());
	SAFELY_CONTROL(if, BitmapBrothersLogoGraphic, Init(_GraphicsObjectRef, &CString(BITMAP_BROTHERS_LOGO_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(InfiniteVenturesLogoGraphic);
	NEW(InfiniteVenturesLogoGraphic, CGraphicsData());
	SAFELY_CONTROL(if, InfiniteVenturesLogoGraphic, Init(_GraphicsObjectRef, &CString(INFINITE_VENTURES_LOGO_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(BroadswordLogoGraphic);
	NEW(BroadswordLogoGraphic, CGraphicsData());
	SAFELY_CONTROL(if, BroadswordLogoGraphic, Init(_GraphicsObjectRef, &CString(BROADSWORD_LOGO_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(KeysGraphic);
	NEW(KeysGraphic, CGraphicsData());
	SAFELY_CONTROL(if, KeysGraphic, Init(_GraphicsObjectRef, &CString(KEYS_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(ArmourGraphic);
	NEW(ArmourGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ArmourGraphic, Init(_GraphicsObjectRef, &CString(ARMOUR_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(FacesGraphic);
	NEW(FacesGraphic, CGraphicsData());
	SAFELY_CONTROL(if, FacesGraphic, Init(_GraphicsObjectRef, &CString(FACES_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(ShieldGraphic);
	NEW(ShieldGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ShieldGraphic, Init(_GraphicsObjectRef, &CString(SHIELD_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(ShieldLetterGraphic);
	NEW(ShieldLetterGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ShieldLetterGraphic, Init(_GraphicsObjectRef, &CString(SHIELD_LETTER_GRAPHIC), _PlatformRef), ==, false)
		return false;

	SAFELY_DELETE(ThePickupGraphic);
	NEW(ThePickupGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ThePickupGraphic, Init(GraphicsObjectRef, &CString(PICKUP_IMAGE), PlatformRef), ==, false)
		return false;

// Done
//	SAFELY_DELETE(ThePitchGraphic);
//	NEW(ThePitchGraphic, CGraphicsData());
//	SAFELY_CONTROL(if, ThePitchGraphic, Init(GraphicsObjectRef, &CString(PITCH_GRAPHIC), PlatformRef), ==, false)
//		return false;

	SAFELY_DELETE(TheBonusGraphic);
	NEW(TheBonusGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheBonusGraphic, Init(GraphicsObjectRef, &CString(BONUS_GRAPHIC), PlatformRef), ==, false)
		return false;

// Done
//	SAFELY_DELETE(LoadOverlay);
//	NEW(LoadOverlay, CGraphicsData());
//	SAFELY_CONTROL(if, LoadOverlay, Init(GraphicsObjectRef, &CString(LOAD_GRAPHIC), PlatformRef), ==, false)
//		return false;

	SAFELY_DELETE(KeyboardGraphic);
	NEW(KeyboardGraphic, CGraphicsData());
	SAFELY_CONTROL(if, KeyboardGraphic, Init(GraphicsObjectRef, &CString(KEYBOARD_GRAPHIC), PlatformRef), ==, false)
		return false;

// Done
//	SAFELY_DELETE(CreditsGraphic);
//	NEW(CreditsGraphic, CGraphicsData());
//	SAFELY_CONTROL(if, CreditsGraphic, Init(GraphicsObjectRef, &CString(CREDITS_GRAPHIC), PlatformRef), ==, false)
//		return false;

// Done
//	SAFELY_DELETE(StoryScreenBackground);
//	NEW(StoryScreenBackground, CGraphicsData());
//	SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
//		return false;

// Done
//	SAFELY_DELETE(ManagerGraphic);
//	NEW(ManagerGraphic, CGraphicsData());
//	SAFELY_CONTROL(if, ManagerGraphic, Init(_GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), _PlatformRef), ==, false)
//		return false;

	SAFELY_DELETE(GymGraphic);
	NEW(GymGraphic, CGraphicsData());
	SAFELY_CONTROL(if, GymGraphic, Init(_GraphicsObjectRef, &CString(GYM_SCREEN_GRAPHIC), _PlatformRef), ==, false)
		return false;
	

	// ----
	// Audio
	SAFELY_DELETE(MovementSound);
	NEW(MovementSound, CSample());
	SAFELY_CONTROL(if, MovementSound, Init(_PlatformRef, &CString(FES_MENU_MOVEMENT)), ==, false)
		return false;
		//return true;

	SAFELY_DELETE(SelectSound);
	NEW(SelectSound, CSample());
	SAFELY_CONTROL(if, SelectSound, Init(_PlatformRef, &CString(FES_MENU_SELECT)), ==, false)
		return false;
		//return true;

// not used
//	SAFELY_DELETE(CancelSound);
//	NEW(CancelSound, CSample());
//	SAFELY_CONTROL(if, CancelSound, Init(_PlatformRef, &CString(FES_MENU_BACKUP)), ==, false)
//		return false;
		//return true;

#ifdef PLAY_MUSIC
	if (GameDataRef->GetPreferences()->MusicEnabled)
	{
		//if (!
			SoundSystemRef->LoadMod(&CString("music.umd"), true);//)
			SoundSystemRef->ModPlaying = true;
//		{
//			MessageBox(NULL, TEXT("PlayMod failed"), GAME_NAME, MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
//		}
	}
	else
	{
			SoundSystemRef->LoadMod(&CString("music.umd"), true);//)
			SoundSystemRef->ModPlaying = false;
	}
#endif

	// All loaded okay...
	return true;
}


// ----
// Update the state information.
void	CFrontEnd::SetState(EFrontEndState _State)
{
	SAFELY_CALL(SoundSystemRef, Process()); 

	PreviousFEState		= FrontEndState;
	FrontEndState		= _State;

	DialogInitialised	= false;
	DialogStartTime		= TimeNow;

	PreviousMenuPos		= CurrentMenuPos;
	CurrentMenuPos		= 0;

	SAFELY_CALL(GameWindowRef, InvalidateMouse());
}

// ----
// Set the state the game comes back into.
// Only certain modes are allowed. True indicates success.
bool	CFrontEnd::SetReturnState(EFrontEndState _State)
{
	switch (_State)
	{
	case EFES_Main:
	case EFES_Single_KnockOut_ComingOutOfGame:
	case EFES_Single_League_New_ComingOutOfGame:
	case EFES_Single_League_TeamManager_ComingOutOfGame:
	case EFES_Single_Cup_New_ComingOutOfGame:
		SetState(_State);
		return true;
	default:
		return false;
	}
}



/*
// Out dated.

// ----
// Load front end data
bool	CFrontEnd::LoadData()
{
	return true;
}


// ----
// Free data
bool	CFrontEnd::FreeData()
{
	return true;
}


// ----
// Restore state
bool	CFrontEnd::LoadState()
{
	return true;
}


// ----
// Save state
bool	CFrontEnd::SaveState()
{
	return true;
}
*/

// ----
// Default navigation for up and down options.
void	CFrontEnd::DefaultVerticalNav(int _MaxNavItems, bool _ResetDialogStartTime)
{
	IfUpDB					// Macros
	{
		CurrentMenuPos--;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));

		if (_ResetDialogStartTime)
			DialogStartTime = TimeNow;
	}

	IfDownDB
	{
		CurrentMenuPos++;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
		if (_ResetDialogStartTime)
			DialogStartTime = TimeNow;
	}

	if (CurrentMenuPos < 0)	
		CurrentMenuPos = _MaxNavItems - 1;
	if (CurrentMenuPos > (_MaxNavItems - 1))
		CurrentMenuPos = 0;
}


// ----
// Run the front end
EFrontEndExitProcessState	CFrontEnd::Process()
{
	// Update timers, etc.
	GetSystemTime(&TimeNow);
	DialogElapseTime = GetTimeDifference(&TimeNow, &DialogStartTime);

	// Free memory if possible. 
	ConserveMemory();

	switch (FrontEndState)
	{
	case EFES_Starting:
		// Init data, whatever here!
		SetState(EFES_Splash);
		return EFEEPS_Normal;

	case EFES_Splash:
		if (Splash())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// MAIN
	case EFES_Main:
		if (Main())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// SINGLE
	case EFES_Single:
		if (Single())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;



//	case EFES_Single_League_New_Update:
//	case EFES_Single_League_TeamManager_Update:
//	case EFES_Single_Cup_New_Update:
//		if (UpdateScoresAndReadyNextMatch())
//			return EFEEPS_Normal;
//		else
//			SetState(EFES_Error);
//		break;


	// WON - All won states
	case EFES_Single_KnockOut_Won:
	case EFES_Single_League_New_Won:
	case EFES_Single_League_TeamManager_Won:
	case EFES_Single_Cup_Won:
		if (Won())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// KNOCKOUT
	case EFES_Single_KnockOut_GameOver:
	case EFES_Single_League_New_Promotion:
	case EFES_Single_League_TeamManager_Promotion:
	case EFES_Single_League_New_Relegate:
	case EFES_Single_League_TeamManager_Relegate:
	case EFES_Single_League_New_PlayOff:
	case EFES_Single_League_TeamManager_PlayOff:
	case EFES_Single_League_New_End_Season:
	case EFES_Single_League_TeamManager_End_Season:
	case EFES_Single_League_New_Save_Failed:
	case EFES_Single_League_TeamManager_Save_Failed:
	case EFES_Single_Cup_New_Save_Failed:
	case EFES_Single_League_New_Save_Successful:
	case EFES_Single_League_TeamManager_Save_Successful:
	case EFES_Single_Cup_New_Save_Successful:

	case EFES_Load_Failed:
	case EFES_Single_League_New_Load_Successful:
	case EFES_Single_League_TeamManager_Load_Successful:
	case EFES_Single_Cup_New_Load_Successful:
		if (Message())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_KnockOut_Fixtures:
		if (Fixtures())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_KnockOut_Gym:
		if (Gym())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_KnockOut_Game:
		if (Game())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_KnockOut_ComingOutOfGame:
		if (ComingOutOfGame())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// LEAGUE
	case EFES_Single_League:
		if (Single_League())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// LEAGUE - NEW GAME
	case EFES_Single_League_New_Manager:
		if (Manager())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Results page
	case EFES_Single_League_New_Results:
		if (Results())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	case EFES_Single_League_New_ComingOutOfGame:
		if (ComingOutOfGame())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// R1
	case EFES_Single_League_New_Transfer:
		if (Transfer())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_New_Save:
		if (Save())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_New_Game:
		if (Game())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;

	// R2
	case EFES_Single_League_New_Tables:
	case EFES_Single_League_New_Tables_Then_Test_Win:
		if (Tables())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	case EFES_Single_League_New_Gym:
		if (Gym())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_New_Manager_Fixtures:
	case EFES_Single_League_New_Gym_Fixtures:
	case EFES_Single_League_New_Transfer_Fixtures:
		if (Fixtures())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// R3
	case EFES_Single_League_New_Statistics:
		if (Stats())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Sub does not need a function.
	

	// LEAGUE - TEAM MANAGER GAME
	case EFES_Single_League_TeamManager_Manager:
		if (Manager())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Results page
	case EFES_Single_League_TeamManager_Results:
		if (Results())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;
		
	case EFES_Single_League_TeamManager_ComingOutOfGame:
		if (ComingOutOfGame())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// R1
	case EFES_Single_League_TeamManager_Transfer:
		if (Transfer())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_TeamManager_Save:
		if (Save())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_TeamManager_Game:
		if (Game())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;

	// R2
	case EFES_Single_League_TeamManager_Tables:
	case EFES_Single_League_TeamManager_Tables_Then_Test_Win:
		if (Tables())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_TeamManager_Gym:
		if (Gym())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_League_TeamManager_Manager_Fixtures:
	case EFES_Single_League_TeamManager_Gym_Fixtures:
	case EFES_Single_League_TeamManager_Transfer_Fixtures:
		if (Fixtures())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// R3
	case EFES_Single_League_TeamManager_Statistics:
		if (Stats())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Sub does not need a function.
	


	// LEAGUE - LOAD GAME
	case EFES_Single_League_LoadGame_Load:
		SetState(EFES_Main);
		return EFEEPS_Normal;
		
	case EFES_Single_League_LoadGame_InsertDisk:
		SetState(EFES_Main);
		return EFEEPS_Normal;


	// CUP
/*	case EFES_Single_Cup:
		if (Single_Cup())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;
*/


	// CUP - NEW GAME
	case EFES_Single_Cup_New_RoundN:
		if (Single_Cup_RoundN())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_ComingOutOfGame:
		if (ComingOutOfGame())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_Manager:
		if (Manager())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Results page
	case EFES_Single_Cup_New_Results:
		if (CupResults())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// R1
	case EFES_Single_Cup_New_Transfer:
		if (Transfer())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_Save:
		if (Save())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_Game:
		if (Game())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;

	// R2
	case EFES_Single_Cup_New_Tables:
		if (CupTable())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_Gym:
		if (Gym())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Single_Cup_New_Manager_Fixtures:
	case EFES_Single_Cup_New_Gym_Fixtures:
	case EFES_Single_Cup_New_Transfer_Fixtures:
		if (Fixtures())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	// R3
	case EFES_Single_Cup_New_Statistics:
		if (Stats())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// Sub does not need a function.


	// CUP - LOAD GAME
	case EFES_Single_Cup_LoadGame_Load:
		SetState(EFES_Main);
		return EFEEPS_Normal;

	case EFES_Single_Cup_LoadGame_InsertDisk:
		SetState(EFES_Main);
		return EFEEPS_Normal;


	// PRACTICE
	case EFES_Single_Practice:		
		if (Practice())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;

	case EFES_Demo:
		if (Demo())
			return EFEEPS_GoingInToGame;
		else
			SetState(EFES_Error);
		break;
		
	case EFES_Load:
		if (Load())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Options:
		if (Options())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Volume:
		if (Volume())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Buffer:
		if (Buffer())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Stylus:
		if (Stylus())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Difficulty:
		if (Difficulty())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	case EFES_ConfirmAbandonCurrent:
		if (ConfirmAbandon())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	case EFES_ConfirmQuit:
		if (ConfirmQuit())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	//Put in by Anthony
	case EFES_Help:
		if (HelpMenu())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Overview:
		if (HelpMenuOverview())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Story:
		if (HelpMenuStory())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Controls:
		if (HelpMenuControls())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Items:
		if (HelpMenuItems())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Game_Modes:
		if (HelpMenuGameModes())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Screens:
		if (HelpMenuScreens())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Help_Pitch:
		if (HelpMenuPitch())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Credits:
		if (Credits())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	// New registration code
	case EFES_Register:
		if (Register())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Register_ThankYou:
		if (RegisterThankYou())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Register_Mistake:
		if (RegisterMistake())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Register_BadUserName:
		if (RegisterBadUserName())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;

	case EFES_Cheat:
		if (Cheat())
			return EFEEPS_Normal;
		else
			SetState(EFES_Error);
		break;


	case EFES_Quit:
		return EFEEPS_Quiting;

	case EFES_Error:
		return EFEEPS_Error;

	default:
		NODEFAULT;
	}

	return EFEEPS_Error;
}

// ----
void	CFrontEnd::ConserveMemory(bool _Force)
{
	MEMORYSTATUS	StartMem;
	StartMem.dwLength = sizeof(StartMem);
	GlobalMemoryStatus(&StartMem);

	if (	(_Force) 
		||	(StartMem.dwAvailPhys < 1048576)) // Collect memory when less than one meg left.
	{
		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_Won:		
		case EFES_Single_KnockOut_Fixtures:
		case EFES_Single_KnockOut_Gym:
		case EFES_Single_KnockOut_Game:
		case EFES_Single_KnockOut_ComingOutOfGame:
		case EFES_Single_KnockOut_GameOver:
			// Can't see manager screen in this mode.
			SAFELY_DELETE(ManagerGraphic);

		case EFES_Starting:
		case EFES_Splash:
		case EFES_Main:
		case EFES_Single:
		case EFES_Single_League_New_Won:
		case EFES_Single_League_TeamManager_Won:
		case EFES_Single_Cup_Won:
		case EFES_Single_League_New_Promotion:
		case EFES_Single_League_TeamManager_Promotion:
		case EFES_Single_League_New_Relegate:
		case EFES_Single_League_TeamManager_Relegate:
		case EFES_Single_League_New_PlayOff:
		case EFES_Single_League_TeamManager_PlayOff:
		case EFES_Single_League_New_End_Season:
		case EFES_Single_League_TeamManager_End_Season:
		case EFES_Single_League:
		case EFES_Single_League_New_Manager:
		case EFES_Single_League_New_Results:
		case EFES_Single_League_New_ComingOutOfGame:
		case EFES_Single_League_New_Transfer:
		case EFES_Single_League_New_Game:
		case EFES_Single_League_New_Tables:
		case EFES_Single_League_New_Tables_Then_Test_Win:
		case EFES_Single_League_New_Gym:
		case EFES_Single_League_New_Manager_Fixtures:
		case EFES_Single_League_New_Gym_Fixtures:
		case EFES_Single_League_New_Transfer_Fixtures:
		case EFES_Single_League_New_Statistics:
		case EFES_Single_League_TeamManager_Manager:
		case EFES_Single_League_TeamManager_Results:
		case EFES_Single_League_TeamManager_ComingOutOfGame:
		case EFES_Single_League_TeamManager_Transfer:
		case EFES_Single_League_TeamManager_Game:
		case EFES_Single_League_TeamManager_Tables:
		case EFES_Single_League_TeamManager_Tables_Then_Test_Win:
		case EFES_Single_League_TeamManager_Gym:
		case EFES_Single_League_TeamManager_Manager_Fixtures:
		case EFES_Single_League_TeamManager_Gym_Fixtures:
		case EFES_Single_League_TeamManager_Transfer_Fixtures:
		case EFES_Single_League_TeamManager_Statistics:
		case EFES_Single_Cup_New_RoundN:
		case EFES_Single_Cup_New_ComingOutOfGame:
		case EFES_Single_Cup_New_Manager:
		case EFES_Single_Cup_New_Results:
		case EFES_Single_Cup_New_Transfer:
		case EFES_Single_Cup_New_Game:
		case EFES_Single_Cup_New_Tables:
		case EFES_Single_Cup_New_Gym:
		case EFES_Single_Cup_New_Manager_Fixtures:
		case EFES_Single_Cup_New_Gym_Fixtures:
		case EFES_Single_Cup_New_Transfer_Fixtures:
		case EFES_Single_Cup_New_Statistics:
		case EFES_Single_Practice:		
		case EFES_Options:
		case EFES_Volume:
		case EFES_Buffer:
		case EFES_Stylus:
		case EFES_Difficulty:
		case EFES_ConfirmAbandonCurrent:
		case EFES_ConfirmQuit:	

			// Don't need load screen.
			SAFELY_DELETE(LoadOverlay);

		case EFES_Load:
		case EFES_Load_Failed:
		case EFES_Single_League_LoadGame_Load:
		case EFES_Single_League_LoadGame_InsertDisk:
		case EFES_Single_Cup_LoadGame_Load:
		case EFES_Single_League_New_Load_Successful:
		case EFES_Single_League_TeamManager_Load_Successful:
		case EFES_Single_Cup_New_Load_Successful:
		case EFES_Single_League_New_Save_Failed:
		case EFES_Single_League_TeamManager_Save_Failed:
		case EFES_Single_Cup_New_Save_Failed:
		case EFES_Single_League_New_Save_Successful:
		case EFES_Single_League_TeamManager_Save_Successful:
		case EFES_Single_Cup_New_Save_Successful:
		case EFES_Single_League_New_Save:
		case EFES_Single_League_TeamManager_Save:
		case EFES_Single_Cup_New_Save:
		case EFES_Single_Cup_LoadGame_InsertDisk:

			// Help / credit screen bits
			SAFELY_DELETE(ThePitchGraphic);
			SAFELY_DELETE(CreditsGraphic);
			SAFELY_DELETE(StoryScreenBackground);

		case EFES_Help:
		case EFES_Help_Overview:
		case EFES_Help_Story:
		case EFES_Help_Controls:
		case EFES_Help_Items:
		case EFES_Help_Game_Modes:
		case EFES_Help_Screens:
		case EFES_Help_Pitch:
		case EFES_Credits:
			// Don't delete anything - there all in use

		case EFES_Demo:
		case EFES_Cheat:
		case EFES_Quit:
		case EFES_Error:
		default:
			// No need
			break;
		}
	}
}

// ----
// Show splash screen
bool	CFrontEnd::Splash()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
//	SAFELY_CALL(SoundSystemRef, PlaySample(&CString(FES_SPLASH)));
	
	if (!DialogInitialised)
	{	
		RedrawCountDown = 0;
		// Load splash screen.
		SAFELY_CALL(SoundSystemRef, Process());
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());

		SAFELY_CALL(SoundSystemRef, Process());
		SAFELY_CALL(BackgroundGraphic, Init(GraphicsObjectRef, &CString("splash"), PlatformRef));

//		SAFELY_DELETE(BackgroundSound);
//		NEW(BackgroundSound, CSample());
//		SAFELY_CALL(BackgroundSound, Init(PlatformRef, &CString(FES_SPLASH)));

//		SAFELY_CALL(SoundSystemRef, PlaySample(BackgroundSound));

		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (DialogElapseTime > 3.0f)
	{
		// Delete resource
		SAFELY_CALL(SoundSystemRef, Process());
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		
		SAFELY_CALL(SoundSystemRef, Process());
		SAFELY_CALL(BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef));

		SAFELY_CALL(SoundSystemRef, Process());
		SetState(EFES_Main);
	}
	else
	{
		if (GraphicsObjectRef)
		{
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = 240;
			all.bottom = 320;	

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&CString("Copyright 1990-2001"), 
											(GraphicsObjectRef->GetDisplay()->cxWidth >> 1),
											8,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&CString("The Bitmap Brothers"),
											(GraphicsObjectRef->GetDisplay()->cxWidth >> 1),
											16,
											TEXT_CENTRED);

			// Draw bitmap brothers logo.
			all.left = 0;
			all.top = 0;
			all.right = 57;
			all.bottom = 56;	
			SAFELY_CALL(GraphicsObjectRef, DrawSprite(4, 256, BitmapBrothersLogoGraphic, all));

			// Draw broadsword logo.
			all.left = 0;
			all.top = 0;
			all.right = 42;
			all.bottom = 56;	
			SAFELY_CALL(GraphicsObjectRef, DrawSprite(240 - 4 - all.right, 256, BroadswordLogoGraphic, all));

			// Draw infinite ventures logo.
			all.left = 0;
			all.top = 0;
			all.right = 90;
			all.bottom = 56;	
			SAFELY_CALL(GraphicsObjectRef, DrawSprite(120-45, 256, InfiniteVenturesLogoGraphic, all));
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
// Main menu
bool	CFrontEnd::Main()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 8;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Single Player";
		MenuTextArray[1]	= "Load Game";
		MenuTextArray[2]	= "Options";
		MenuTextArray[3]	= "Demo";
		MenuTextArray[4]	= "Help";
		MenuTextArray[5]	= "Credits";
		if (GameDataRef->GetPreferences()->Registered)
			MenuTextArray[6]	= " ";
		else
			MenuTextArray[6]	= "Register";
		MenuTextArray[7]	= "Quit";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;

		RedrawCountDown = 0;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
	/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
	*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}


	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP - MENU_ITEM_SPACING;
		region.right = 240;
		region.bottom = region.top + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP - MENU_ITEM_SPACING, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_MAIN),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP - MENU_ITEM_SPACING + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP - MENU_ITEM_SPACING + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP - MENU_ITEM_SPACING + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SOUND_SELECT;
			SetState(EFES_Single);
			break;
		case 1:
			SOUND_SELECT;
			SetState(EFES_Load);
			break;
		case 2:
			SOUND_SELECT;
			SetState(EFES_Options);
			break;
		case 3:
			SOUND_SELECT;
			SetState(EFES_Demo);
			break;
		case 4:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;
		case 5:
			SOUND_SELECT;
			SetState(EFES_Credits);
			break;
		case 6:
			if (!GameDataRef->GetPreferences()->Registered)
			{
				SOUND_SELECT;
				SetState(EFES_Register);
			}
			// Otherwise, we are registered, and this does nothing!!!
			break;
		case 7:
			SOUND_SELECT;
			SetState(EFES_ConfirmQuit);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	
	// Has the dialog been inactive for longer than 30* seconds? (* Value can be modified via the registry). 
	if (	(GameDataRef->GetDemoTimeOut() > MIN_DEMO_TIMEOUT)
		&& 	(GetTimeDifference(&TimeNow, &DialogStartTime) > GameDataRef->GetDemoTimeOut()))
	{
		// Start demo
		SetState(EFES_Demo);
	}
	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}


// ----
// Single player screen.
bool	CFrontEnd::Single()
{
	if (	(!GraphicsObjectRef)
		||	(!GameDataRef)
		||	(!ControllerRef))
		return false;

	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 5;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Knockout";
		MenuTextArray[1]	= "League";
		MenuTextArray[2]	= "Cup";
		MenuTextArray[3]	= "Practice";
		MenuTextArray[4]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			GameDataRef->Init(KNOCKOUT, PlatformRef);
			SetState(EFES_Single_KnockOut_Fixtures);
			break;
		case 1:
			SetState(EFES_Single_League);
			break;
		case 2:
			GameDataRef->Init(CUP, PlatformRef);
			SetState(EFES_Single_Cup_New_RoundN);
			break;
		case 3:
			GameDataRef->Init(PRACTICE, PlatformRef);
			SetState(EFES_Single_Practice);
			break;
		case 4:
			SetState(EFES_Main);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

/* Replaced by the new and all powerful Fixtures method!
// ----
bool	CFrontEnd::Single_KnockOut()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}


	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_KNOCKOUT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}*/


// ----
bool	CFrontEnd::Single_League()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 3;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		
		RedrawCountDown = 0;
		MenuTextArray[0]	= "New Game";
		MenuTextArray[1]	= "Team Manager";
//		MenuTextArray[2]	= "Load Game";
		MenuTextArray[2]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			GameDataRef->Init(LEAGUE, PlatformRef);
			SetState(EFES_Single_League_New_Manager_Fixtures);
			break;
		case 1:
			GameDataRef->Init(LEAGUE_TEAM_MANAGER, PlatformRef);
			SetState(EFES_Single_League_TeamManager_Manager_Fixtures);
			break;
		case 2:
			SetState(EFES_Single_League_LoadGame_Load);
			break;
		case 3:
			SetState(EFES_Single);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

/* Replaced with Fixtures screen
// ----

bool	CFrontEnd::Single_League_NewGame()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}


	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_KNOCKOUT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single_League);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
bool	CFrontEnd::Single_League_TeamManager()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}


	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_KNOCKOUT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single_League);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
*/

/*
// ----
bool	CFrontEnd::Single_League_LoadGame()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
// / *
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
// * /
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_LOAD),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single_League);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
*/
/*
// ----
bool	CFrontEnd::Single_Cup()
{
	// No longer any need for an options screen here. 
	// Instead generate data here...
	
	SetState(EFES_Single_Cup_New_RoundN);

	
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 3;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "New Game";
		MenuTextArray[1]	= "Load Game";
		MenuTextArray[2]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}


	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SetState(EFES_Single_Cup_New_RoundN);
			break;
		case 1:
			SetState(EFES_Single_Cup_LoadGame_Load);
			break;
		case 2:
			SetState(EFES_Single);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
	
}*/

/*
// ----
bool	CFrontEnd::Single_Cup_NewGame()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "Gym";
		MenuTextArray[1]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}


	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_KNOCKOUT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single_Cup_New_Game);
			break;
		case 1:		
			SetState(EFES_Single_Cup);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}*/

/*
// ----
bool	CFrontEnd::Single_Cup_LoadGame()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Back";
		RedrawCountDown = 0;
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
// / *
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
// * /
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_SINGLE_KNOCKOUT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Single_Cup_New_RoundN);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
*/

// ----
bool	CFrontEnd::Practice()
{
	if (!DialogInitialised)
	{
		LoadingScreen();
		RedrawCountDown = 0;
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	SAFELY_CALL(GameDataRef, Init(PRACTICE, PlatformRef));
	return true;
}

// ----
// The fixtures screen
#define FIXTURE_BORDER					USABLE_COMMON_BORDER
#define	FIXTURES_TEAM_A_NAME_X_POS		(USABLE_LEFT + (USABLE_WIDTH >> 1))
#define	FIXTURES_TEAM_A_NAME_Y_POS		(USABLE_TOP + FIXTURE_BORDER)
#define	FIXTURES_TEAM_B_NAME_X_POS		FIXTURES_TEAM_A_NAME_X_POS
#define	FIXTURES_TEAM_B_NAME_Y_POS		(USABLE_TOP + USABLE_HEIGHT - (SmallLightFont->GetLineHeight() * 13) - FIXTURE_BORDER)

#define	FIXTURES_TEAM_A_ATTRIBUTE_X_POS	(USABLE_LEFT + FIXTURE_BORDER)
#define	FIXTURES_TEAM_A_ATTRIBUTE_Y_POS	(FIXTURES_TEAM_A_NAME_Y_POS + SmallLightFont->GetLineHeight() * (i + 1))
#define	FIXTURES_TEAM_B_ATTRIBUTE_X_POS	(USABLE_LEFT + FIXTURE_BORDER)
#define	FIXTURES_TEAM_B_ATTRIBUTE_Y_POS	(FIXTURES_TEAM_B_NAME_Y_POS + SmallLightFont->GetLineHeight() * (i + 1))
#define	FIXTURES_TEAM_A_FIELD_X_POS		(USABLE_LEFT + USABLE_WIDTH - FIXTURE_BORDER)
#define	FIXTURES_TEAM_A_FIELD_Y_POS		FIXTURES_TEAM_A_ATTRIBUTE_Y_POS
#define	FIXTURES_TEAM_B_FIELD_X_POS		(USABLE_LEFT + USABLE_WIDTH - FIXTURE_BORDER)
#define	FIXTURES_TEAM_B_FIELD_Y_POS		FIXTURES_TEAM_B_ATTRIBUTE_Y_POS

bool	CFrontEnd::Fixtures()
{
	RECT	Region;
	char	convert[24];
	int		i, j;

	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef))
		return false;

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		
		RedrawCountDown = 0;
		// Clear screen
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		// Draw background image
		Region.left = 0;
		Region.top = 0;
		Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, Region));

		// Draw background colour
		Region.top		= USABLE_TOP;
		Region.bottom	= Region.top + USABLE_HEIGHT;
		Region.left		= USABLE_LEFT;
		Region.right	= Region.left + USABLE_WIDTH;
		SAFELY_CALL(GraphicsObjectRef, DrawRect(&Region, 39, 64, 64));

		// Screen title
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_FIXTURE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);

		
		// ----
		// Draw all the other information.

		// -- 
		for (j = 0; j < 2; j++)
		{
			for (i = 0; i < 12; i++)
			{
				switch (i)
				{
				case 0:
					TempStringArray[0] = "Played";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetPlayed());
					break;
				case 1:
					TempStringArray[0] = "Won";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetWon());
					break;
				case 2:
					TempStringArray[0] = "Drawn";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetDrawn());
					break;
				case 3:
					TempStringArray[0] = "Lost";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetLost());
					break;
				case 4:
					TempStringArray[0] = "Score";
					if (FrontEndState == EFES_Single_Cup_New_Manager_Fixtures)
						sprintf(convert, "N/A");
					else
						sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetPoints());
					break;
				case 5:
					TempStringArray[0] = "League placing";
					if (	(FrontEndState == EFES_Single_Cup_New_Manager_Fixtures)
						||	(FrontEndState == EFES_Single_KnockOut_Fixtures))
						sprintf(convert, "N/A");
					else
						sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetPlacing());
					break;
				case 6:
					TempStringArray[0] = "Points for";
					if (	(FrontEndState == EFES_Single_Cup_New_Manager_Fixtures)
						||	(FrontEndState == EFES_Single_KnockOut_Fixtures))
						sprintf(convert, "N/A");
					else
						sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetFor());
					break;
				case 7:
					TempStringArray[0] = "Points against";
					if (	(FrontEndState == EFES_Single_Cup_New_Manager_Fixtures)
						||	(FrontEndState == EFES_Single_KnockOut_Fixtures))
						sprintf(convert, "N/A");
					else
						sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetAgainst());
					break;
				case 8:
					TempStringArray[0] = "Defence";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetAverageDefenceStats());
					break;
				case 9:
					TempStringArray[0] = "Midfield";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetAverageMidfieldStats());
					break;
				case 10:
					TempStringArray[0] = "Attack";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetAverageAttackStats());
					break;
				case 11:
					TempStringArray[0] = "Substitute";
					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(j)->GetAverageSubsStats());
					break;
				default:
					SAFELY_CALL(GraphicsObjectRef, EndDraw());
					return false;
				}

				TempStringArray[1] = convert;
				if (j == 0)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[0],
													FIXTURES_TEAM_A_ATTRIBUTE_X_POS, 
													FIXTURES_TEAM_A_ATTRIBUTE_Y_POS,
													TEXT_LEFT);

					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[1],
													FIXTURES_TEAM_A_FIELD_X_POS, 
													FIXTURES_TEAM_A_FIELD_Y_POS,
													TEXT_RIGHT);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[0],
													FIXTURES_TEAM_B_ATTRIBUTE_X_POS, 
													FIXTURES_TEAM_B_ATTRIBUTE_Y_POS,
													TEXT_LEFT);

					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[1],
													FIXTURES_TEAM_B_FIELD_X_POS, 
													FIXTURES_TEAM_B_FIELD_Y_POS,
													TEXT_RIGHT);
				}
			}
		}

		// Centre bar
		i = 14;
		Region.top		= FIXTURES_TEAM_A_ATTRIBUTE_Y_POS;
		Region.bottom	= FIXTURES_TEAM_B_NAME_Y_POS - SmallLightFont->GetLineHeight() * 2;
		Region.left		= FIXTURES_TEAM_A_ATTRIBUTE_X_POS;
		Region.right	= FIXTURES_TEAM_B_FIELD_X_POS;
		GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

		TempString = "V's";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										FIXTURES_TEAM_A_NAME_X_POS ,
										(Region.top + ((Region.bottom - Region.top - SmallLightFont->GetLineHeight()) >> 1)),
										TEXT_CENTRED);

		GraphicsObjectRef->DrawString(	SmallLightFont, 
										GameDataRef->GetActiveTeamByIndex(0)->GetTeamName(),
										FIXTURES_TEAM_A_NAME_X_POS, 
										FIXTURES_TEAM_A_NAME_Y_POS,
										TEXT_CENTRED);


		GraphicsObjectRef->DrawString(	SmallLightFont, 
										GameDataRef->GetActiveTeamByIndex(1)->GetTeamName(),
										FIXTURES_TEAM_B_NAME_X_POS, 
										FIXTURES_TEAM_B_NAME_Y_POS,
										TEXT_CENTRED);
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	if (	(ControllerRef) 
		&&	(/*	(ControllerRef->IsButtonPressedDB(Up))
			||	(ControllerRef->IsButtonPressedDB(Down))
			||	(ControllerRef->IsButtonPressedDB(Left))
			||	(ControllerRef->IsButtonPressedDB(Right))
			||*/(ControllerRef->IsButtonPressedDB(Select))
			||	(ControllerRef->IsButtonPressedDB(MouseSelect))
			)
		)
	{
		// Set state depending on how we got here.
		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_Fixtures:		
			SetState(EFES_Single_KnockOut_Gym);
			break;

		// League - New
		case EFES_Single_League_New_Manager_Fixtures:		
			SetState(EFES_Single_League_New_Manager);
			break;
		case EFES_Single_League_New_Gym_Fixtures:		
			SetState(EFES_Single_League_New_Gym);
			break;
		case EFES_Single_League_New_Transfer_Fixtures:		
			SetState(EFES_Single_League_New_Transfer);
			break;

		// League - Team manager
		case EFES_Single_League_TeamManager_Transfer_Fixtures:		
			SetState(EFES_Single_League_TeamManager_Transfer);
			break;
		case EFES_Single_League_TeamManager_Gym_Fixtures:		
			SetState(EFES_Single_League_TeamManager_Gym);
			break;
		case EFES_Single_League_TeamManager_Manager_Fixtures:		
			SetState(EFES_Single_League_TeamManager_Manager);
			break;

		// Cup - New
		case EFES_Single_Cup_New_Transfer_Fixtures:		
			SetState(EFES_Single_Cup_New_Transfer);
			break;
		case EFES_Single_Cup_New_Gym_Fixtures:		
			SetState(EFES_Single_Cup_New_Gym);
			break;
		case EFES_Single_Cup_New_Manager_Fixtures:		
			SetState(EFES_Single_Cup_New_Manager);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
// The training room
#define	X_KEY_SPACING				23
#define	Y_KEY_SPACING				23

#define KEY_MATRIX_X_OFFSET			9
#define	KEY_MATRIX_Y_OFFSET			((Y_KEY_SPACING * MAX_YKEYS) + 11)
									
#define	X_LIGHT_OFFSET				1
#define	Y_LIGHT_OFFSET				1
#define	KEY_LIGHT_WIDTH				(X_KEY_SPACING - 1)
#define	KEY_LIGHT_HEIGHT			(Y_KEY_SPACING - 1)
									
#define	KEY_PITCH					10
#define	KEY_WIDTH					32
#define	KEY_HEIGHT					32
									
// Overall body pos.				
#define	BODY_X_OFFSET				25
#define BODY_Y_OFFSET				34

#define	BODY_HEIGHT					164
#define	BODY_WIDTH					86
#define	BODY_BG_BORDER				4
									
// Image info						
#define	PITCH22						10
#define	WIDTH22						32
#define HEIGHT22					32
#define	Y_IMAGE_OFFSET42			128
#define	PITCH42						5
#define	WIDTH42						64
#define HEIGHT42					32

// Part highlighting info.
#define	BP_COUNT					8
#define	BP_HELMET					0
#define	BP_EYES						1
#define	BP_SHOULDERS				2
#define	BP_UPPER_ARMS				3
#define	BP_LOWER_ARMS				4
#define	BP_CHEST					5
#define	BP_UPPER_LEGS				6
#define	BP_LOWER_LEGS				7
#define	BP_DEFAULT					BP_EYES

#define INT_GROUP					(BP_HELMET)
#define AGR_GROUP					(BP_EYES)
#define ATT_GROUP					(BP_SHOULDERS)
#define THR_GROUP					(BP_UPPER_ARMS)
#define POW_GROUP					(BP_LOWER_ARMS)
#define DEF_GROUP					(BP_CHEST)
#define	STA_GROUP					(BP_UPPER_LEGS)
#define SPD_GROUP					(BP_LOWER_LEGS)

#define PART_NAV					(MAX_XKEYS * MAX_YKEYS)

// Body image info.
#define	UPPER_LEFT_LEG				7
#define UPPER_RIGHT_LEG				8
#define LOWER_LEFT_LEG1				9
#define LOWER_LEFT_LEG2				10
#define LOWER_RIGHT_LEG1			11
#define LOWER_RIGHT_LEG2			12
#define LEFT_FOOT					5
#define RIGHT_FOOT					6
#define	TORSO						16
#define UPPER_LEFT_ARM				3
#define UPPER_RIGHT_ARM				4
#define LOWER_LEFT_ARM				13
#define LOWER_RIGHT_ARM				14
#define CHEST						0
#define LEFT_SHOULDER				1
#define RIGHT_SHOULDER				2
#define EYES						15
#define	HELMET						0

// Part positioning info.
#define	UPPER_LEFT_LEG_X_OFFSET		18
#define	UPPER_LEFT_LEG_Y_OFFSET		69
#define UPPER_RIGHT_LEG_X_OFFSET	42
#define UPPER_RIGHT_LEG_Y_OFFSET	UPPER_LEFT_LEG_Y_OFFSET
#define LOWER_LEFT_LEG1_X_OFFSET	16
#define LOWER_LEFT_LEG1_Y_OFFSET	95
#define LOWER_LEFT_LEG2_X_OFFSET	LOWER_LEFT_LEG1_X_OFFSET
#define LOWER_LEFT_LEG2_Y_OFFSET	(LOWER_LEFT_LEG1_Y_OFFSET + HEIGHT22)
#define LOWER_RIGHT_LEG1_X_OFFSET	43
#define LOWER_RIGHT_LEG1_Y_OFFSET	LOWER_LEFT_LEG1_Y_OFFSET
#define LOWER_RIGHT_LEG2_X_OFFSET	LOWER_RIGHT_LEG1_X_OFFSET
#define LOWER_RIGHT_LEG2_Y_OFFSET	LOWER_LEFT_LEG2_Y_OFFSET
#define LEFT_FOOT_X_OFFSET			8
#define LEFT_FOOT_Y_OFFSET			135
#define RIGHT_FOOT_X_OFFSET			47
#define RIGHT_FOOT_Y_OFFSET			LEFT_FOOT_Y_OFFSET
#define	TORSO_X_OFFSET				28
#define	TORSO_Y_OFFSET				51
#define UPPER_LEFT_ARM_X_OFFSET		8
#define UPPER_LEFT_ARM_Y_OFFSET		41
#define UPPER_RIGHT_ARM_X_OFFSET	55
#define UPPER_RIGHT_ARM_Y_OFFSET	UPPER_LEFT_ARM_Y_OFFSET
#define LOWER_LEFT_ARM_X_OFFSET		0
#define LOWER_LEFT_ARM_Y_OFFSET		62
#define LOWER_RIGHT_ARM_X_OFFSET	61
#define LOWER_RIGHT_ARM_Y_OFFSET	LOWER_LEFT_ARM_Y_OFFSET
#define CHEST_X_OFFSET				24
#define CHEST_Y_OFFSET				29
#define LEFT_SHOULDER_X_OFFSET		7
#define LEFT_SHOULDER_Y_OFFSET		24
#define RIGHT_SHOULDER_X_OFFSET		47
#define RIGHT_SHOULDER_Y_OFFSET		LEFT_SHOULDER_Y_OFFSET
#define EYES_X_OFFSET				27
#define EYES_Y_OFFSET				2
#define	HELMET_X_OFFSET				27
#define	HELMET_Y_OFFSET				2

// ----
#define	AREA1_TOP					12
#define	AREA1_BOTTOM				320 - ((Y_KEY_SPACING * MAX_YKEYS) + 30)//190
#define	AREA1_LEFT					8
#define	AREA1_RIGHT					98

// Faces and shields
#define	FACE_X						8
#define	FACE_Y						12
#define	FACE_WIDTH					48
#define	FACE_HEIGHT					48
#define	FACE_PITCH					6

#define	SHIELD_X					16					
#define	SHIELD_Y					16
#define SHIELD_WIDTH				32
#define SHIELD_HEIGHT				48

#define	SHIELD_LETTERS_X			24
#define	SHIELD_LETTERS_Y			24
#define	SHIELD_LETTERS_WIDTH		32
#define	SHIELD_LETTERS_HEIGHT		16

#define	GYM_INDIVIDUAL_TEXT_Y_RAMP	0		/* The number of line to push the text up by from the bottom of the face image */
#define	GYM_INDIVIDUAL_TEXT_SPACING	(SmallLightFont->GetLineHeight() + 2)	/* Space between the top of one line and the next */
//#define	POSITION_TEXT_X				(FACE_X + FACE_WIDTH)
#define	POSITION_TEXT_X				(AREA1_RIGHT - 2)
#define	POSITION_TEXT_Y				(FACE_Y + FACE_HEIGHT - 3 * GYM_INDIVIDUAL_TEXT_SPACING - GYM_INDIVIDUAL_TEXT_Y_RAMP)
#define	WORTH_TEXT_X				(AREA1_RIGHT - 2)
#define	WORTH_TEXT_Y				(FACE_Y + FACE_HEIGHT - 2 * GYM_INDIVIDUAL_TEXT_SPACING - GYM_INDIVIDUAL_TEXT_Y_RAMP)
#define	NAME_TEXT_X					(AREA1_RIGHT - 2)
#define	NAME_TEXT_Y					(FACE_Y + FACE_HEIGHT - 1 * GYM_INDIVIDUAL_TEXT_SPACING - GYM_INDIVIDUAL_TEXT_Y_RAMP)
#define	BALANCE_TEXT_X				40
#define BALANCE_TEXT_Y				(GraphicsObjectRef->GetDisplay()->cyHeight - 30)
#define	BALANACE_TEXT_WIDTH			38
#define	BALANACE_TEXT_HEIGHT		8

#define	UPGRADE_EFFECT				10	/* The amount that is added to someone's attribute following a purchase */
#define	UPGRADE_BUFFER				((GameDataRef->GetGameType() == LEAGUE_TEAM_MANAGER)?40:30)
#define	MAX_STAT					250

#define	INFO_AREA_LEFT				120
#define INFO_AREA_TOP				238
#define	INFO_AREA_WIDTH				108
#define	INFO_AREA_HEIGHT			72
#define INFO_ITEM_X_OFFSET			4
#define	INFO_ITEM_Y_OFFSET			6
#define	INFO_DESC_X_OFFSET			INFO_ITEM_X_OFFSET
#define	INFO_DESC_Y_OFFSET			(INFO_ITEM_Y_OFFSET + 36)
#define	INFO_PRICE_X_OFFSET			INFO_DESC_X_OFFSET
#define	INFO_PRICE_Y_OFFSET			(INFO_DESC_Y_OFFSET + 12)

// I'm not doing all that again!!!

bool	CFrontEnd::Gym()
{
	// Do we have a problem?
	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef)
		||	(!GameDataRef)
		||	(!GameDataRef->GetActiveTeamByIndex(0)))
		return false;
	
	// Quit game
	if (ControllerRef->IsButtonPressedDB(Backup))
	{
		PLAY_BACKUP;
		SetState(EFES_ConfirmAbandonCurrent);
		return true;
	}
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i, j, k;
	int		MouseAt = -1;
	int		BodyXPos	=	GraphicsObjectRef->GetDisplay()->cxWidth - BODY_WIDTH - BODY_X_OFFSET;
	int		BodyYPos	=	BODY_Y_OFFSET;
	RECT Region;
		
	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(0, 0, 0));
/*
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(GYM_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, GymGraphic, all));
		
		// Keypad definition.
		for (int xkey = MAX_XKEYS; xkey--;)
		{
			for (int ykey = MAX_YKEYS; ykey--;)
			{
				KeyMatrix[xkey][ykey] = BLANK_KEY;
			}
		}

		// Top row.
		KeyMatrix[0][0]			= I_KEY;
		// Blank				
		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_Gym:			// play key when in knock out mode.
			KeyMatrix[2][0]			= PLY_KEY;
			break;
		default:
			KeyMatrix[2][0]			= ESC_KEY;
			break;
		}
		KeyMatrix[3][0]			= UP_KEY;
								
		// Middle row.			
		KeyMatrix[0][1]			= G_KEY;
		KeyMatrix[1][1]			= ALL_KEY;
		KeyMatrix[2][1]			= FIX_KEY;
		KeyMatrix[3][1]			= DOWN_KEY;
								
		// Bottom row.			
		KeyMatrix[0][2]			= T_KEY;
		// Blank			
		// Blank			
		KeyMatrix[3][2]			= BUY_KEY;

		// Select matrix key 2, 1
		CurrentMenuPos			= 2 + (MAX_XKEYS * 1);

		// Select team mode
		GymMode					= EFEGM_Team;
		SelectedBodyPart		= BP_DEFAULT;
		SelectedPlayer			= 0;
		SelectedGroup			= EFEPG_Defence;
		AllSelect				= -1;

		// ----
		// Set the parts cost
		PartsList[INT_GROUP]	= INT_COST;	// Helmet
		PartsList[AGR_GROUP]	= AGR_COST;	// Eyes
		PartsList[ATT_GROUP]	= ATT_COST;	// Shoulder
		PartsList[THR_GROUP]	= THR_COST;	// Upper arms
		PartsList[POW_GROUP]	= POW_COST;	// Lower arms
		PartsList[DEF_GROUP]	= DEF_COST;	// Chest
		PartsList[STA_GROUP]	= STA_COST;	// Upper legs
		PartsList[SPD_GROUP]	= SPD_COST;	// Lower legs

#ifdef GYM_FUZZ
		// ----
		// Fuzzy scan thing for body background.
		for (int i = NUMBER_OF_SCANS; i--;)
		{
			ScanArray[i].top	=	BodyYPos
								+	(((rand() >> 5) * (BODY_HEIGHT + BODY_BG_BORDER)) >> 10);
			ScanArray[i].bottom = ScanArray[i].top + (((rand() >> 5) * (BODY_HEIGHT + BODY_BG_BORDER)) >> 10);

			ScanArray[i].left	= BodyXPos - BODY_BG_BORDER;
			ScanArray[i].right	= BodyXPos + BODY_WIDTH + BODY_BG_BORDER;
		}
#endif

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(0, 0, 0));
/*
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(GYM_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, GymGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	if (AllSelect < 0)
	{
		// Navigation
		IfUpDB					// Macro
		{
			switch (CurrentMenuPos)
			{
			// Top row
			case 0:
				CurrentMenuPos = 0 + MAX_XKEYS * 2;
				break;
			case 1:
				CurrentMenuPos = 1 + MAX_XKEYS * 1;
				break;
			case 2:
				CurrentMenuPos = 2 + MAX_XKEYS * 1;
				break;
			case 3:
				CurrentMenuPos = 3 + MAX_XKEYS * 2;
				break;

			// Second row
			case 4:
			case 5:
			case 6:
			case 7:
			// Third
			case 8:
			case 11:
				CurrentMenuPos -= MAX_XKEYS;
				break;

			// Navigate the body (?)
			case 12:
				CurrentMenuPos = 19;
				break;
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				CurrentMenuPos--;
				break;

			// Keys shouldn't be able to take any other values.
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
		}

		IfDownDB
		{
			switch (CurrentMenuPos)
			{
			// Top row
			case 0:
			case 1:
			case 2:
			case 3:
			// Second row
			case 4:
			case 7:
				CurrentMenuPos += MAX_XKEYS;
				break;

			case 5:
				CurrentMenuPos = 1;
				break;
			case 6:
				CurrentMenuPos = 2;
				break;
			case 8:
				CurrentMenuPos = 0;
				break;
			case 11:
				CurrentMenuPos = 3;
				break;

			// Navigate the body (?)
			case 12:			// MAX_XKEYS * MAX_YKEYS
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
				CurrentMenuPos++;
				break;
			case 19:
				CurrentMenuPos = MAX_XKEYS * MAX_YKEYS;
				break;

			// Keys shouldn't be able to take any other values.
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
		}

		IfLeftDB
		{
			switch (CurrentMenuPos)
			{
			// Left col
			case 0:
			case 4:
			case 8:
				CurrentMenuPos += MAX_XKEYS - 1;
				break;

			case 1:
			case 2:
			case 3:
			case 5:
			case 6:
			case 7:
				CurrentMenuPos--;
				break;
			
			case 11:
				CurrentMenuPos = 8;
				break;

			// Navigate off body
			case 12:	// MAX_XKEYS * MAX_YKEYS
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				CurrentMenuPos = MAX_XKEYS - 1;
				break;

			// Keys shouldn't be able to take any other values.
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
		}

		IfRightDB
		{
			switch (CurrentMenuPos)
			{
			// Right col
			case 3:
			case 7:
			case 11:
				//CurrentMenuPos -= MAX_XKEYS - 1;
				CurrentMenuPos = MAX_XKEYS * MAX_YKEYS + SelectedBodyPart; // Return to the highlighted body part.
				break;

			case 0:
			case 1:
			case 2:
			case 4:
			case 5:
			case 6:
				CurrentMenuPos++;
				break;
			
			case 8:
				CurrentMenuPos = 11;
				break;

			case 12:	// MAX_XKEYS * MAX_YKEYS
			case 13:
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
				break;

			// Keys shouldn't be able to take any other values.
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}

			SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
		}
	}

	// ----
	// RENDER
	// ----

	if (AllSelect >= 0)
		SelectedBodyPart = AllSelect;

	// ----
	// Draw the human (thing).

	// Draw background
#ifdef GYM_FUZZ

	Region.top		= BodyYPos - BODY_BG_BORDER;
	Region.bottom	= BodyYPos + BODY_HEIGHT + BODY_BG_BORDER;
	Region.left		= BodyXPos - BODY_BG_BORDER;
	Region.right	= BodyXPos + BODY_WIDTH + BODY_BG_BORDER;
	GraphicsObjectRef->DrawRect(&Region, 0, 20, 0);

	for (i = NUMBER_OF_SCANS; i--;)
	{
		ScanArray[i].top += (rand() >> 11);
		ScanArray[i].top -= (rand() >> 11);
		ScanArray[i].bottom += (rand() >> 13) - 2;

		// Keep top in bounds.
		if (ScanArray[i].top < (BodyYPos - BODY_BG_BORDER))
			ScanArray[i].top = BodyYPos - BODY_BG_BORDER;
		if (ScanArray[i].top > (BodyYPos + BODY_HEIGHT + BODY_BG_BORDER))
			ScanArray[i].top = BodyYPos + BODY_HEIGHT + BODY_BG_BORDER;

		// Keep bottom below top.
		if ((ScanArray[i].bottom - ScanArray[i].top) < 0)
			ScanArray[i].bottom = ScanArray[i].top + (((rand() >> 5) * (BODY_HEIGHT + BODY_BG_BORDER)) >> 10);

		// Keep bottom in region.
		if (ScanArray[i].bottom > (BodyYPos + BODY_HEIGHT + BODY_BG_BORDER))
			ScanArray[i].bottom = BodyYPos + BODY_HEIGHT + BODY_BG_BORDER;

		GraphicsObjectRef->DrawRect(&ScanArray[i], i * 1.5, (10 + 3 * i) * 1.5, (20 - i) + 4);
	}
#endif

	// ----
	// Torso
	Region.top		=	((2 * TORSO) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * TORSO) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + TORSO_X_OFFSET, 
									BodyYPos + TORSO_Y_OFFSET, 
									ArmourGraphic,
									Region);

	// Upper leg - left
	Region.top		=	((2 * UPPER_LEFT_LEG + ((SelectedBodyPart == STA_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * UPPER_LEFT_LEG + ((SelectedBodyPart == STA_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + UPPER_LEFT_LEG_X_OFFSET, 
									BodyYPos + UPPER_LEFT_LEG_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + UPPER_LEFT_LEG_X_OFFSET;
	Region.top		= BodyYPos + UPPER_LEFT_LEG_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = STA_GROUP;
	

	// Upper leg - right
	Region.top		=	((2 * UPPER_RIGHT_LEG + ((SelectedBodyPart == STA_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * UPPER_RIGHT_LEG + ((SelectedBodyPart == STA_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + UPPER_RIGHT_LEG_X_OFFSET, 
									BodyYPos + UPPER_RIGHT_LEG_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + UPPER_RIGHT_LEG_X_OFFSET;
	Region.top		= BodyYPos + UPPER_RIGHT_LEG_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = STA_GROUP;

	// ----
	// Lower leg - left (part1)
	Region.top		=	((2 * LOWER_LEFT_LEG1 + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_LEFT_LEG1 + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_LEFT_LEG1_X_OFFSET, 
									BodyYPos + LOWER_LEFT_LEG1_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_LEFT_LEG1_X_OFFSET;
	Region.top		= BodyYPos + LOWER_LEFT_LEG1_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// Lower leg - left (part2)
	Region.top		=	((2 * LOWER_LEFT_LEG2 + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_LEFT_LEG2 + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_LEFT_LEG2_X_OFFSET, 
									BodyYPos + LOWER_LEFT_LEG2_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_LEFT_LEG2_X_OFFSET;
	Region.top		= BodyYPos + LOWER_LEFT_LEG2_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// Lower leg - right (part1)
	Region.top		=	((2 * LOWER_RIGHT_LEG1 + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_RIGHT_LEG1 + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_RIGHT_LEG1_X_OFFSET, 
									BodyYPos + LOWER_RIGHT_LEG1_Y_OFFSET, 
									ArmourGraphic,
									Region);
	
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_RIGHT_LEG1_X_OFFSET;
	Region.top		= BodyYPos + LOWER_RIGHT_LEG1_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// Lower leg - right (part2)
	Region.top		=	((2 * LOWER_RIGHT_LEG2 + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_RIGHT_LEG2 + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_RIGHT_LEG2_X_OFFSET, 
									BodyYPos + LOWER_RIGHT_LEG2_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_RIGHT_LEG2_X_OFFSET;
	Region.top		= BodyYPos + LOWER_RIGHT_LEG2_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// Feet - left
	Region.top		=	((2 * LEFT_FOOT + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LEFT_FOOT + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LEFT_FOOT_X_OFFSET, 
									BodyYPos + LEFT_FOOT_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LEFT_FOOT_X_OFFSET;
	Region.top		= BodyYPos + LEFT_FOOT_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// Feet - right
	Region.top		=	((2 * RIGHT_FOOT + ((SelectedBodyPart == SPD_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * RIGHT_FOOT + ((SelectedBodyPart == SPD_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + RIGHT_FOOT_X_OFFSET, 
									BodyYPos + RIGHT_FOOT_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + RIGHT_FOOT_X_OFFSET;
	Region.top		= BodyYPos + RIGHT_FOOT_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = SPD_GROUP;

	// ----
	// Upper arm - left
	Region.top		=	((2 * UPPER_LEFT_ARM + ((SelectedBodyPart == THR_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * UPPER_LEFT_ARM + ((SelectedBodyPart == THR_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + UPPER_LEFT_ARM_X_OFFSET, 
									BodyYPos + UPPER_LEFT_ARM_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + UPPER_LEFT_ARM_X_OFFSET;
	Region.top		= BodyYPos + UPPER_LEFT_ARM_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = THR_GROUP;

	// Upper arm - right
	Region.top		=	((2 * UPPER_RIGHT_ARM + ((SelectedBodyPart == THR_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * UPPER_RIGHT_ARM + ((SelectedBodyPart == THR_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + UPPER_RIGHT_ARM_X_OFFSET, 
									BodyYPos + UPPER_RIGHT_ARM_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + UPPER_RIGHT_ARM_X_OFFSET;
	Region.top		= BodyYPos + UPPER_RIGHT_ARM_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = THR_GROUP;

	// ----
	// Lower arm - left
	Region.top		=	((2 * LOWER_LEFT_ARM + ((SelectedBodyPart == POW_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_LEFT_ARM + ((SelectedBodyPart == POW_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_LEFT_ARM_X_OFFSET, 
									BodyYPos + LOWER_LEFT_ARM_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_LEFT_ARM_X_OFFSET;
	Region.top		= BodyYPos + LOWER_LEFT_ARM_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = POW_GROUP;

	// Lower arm - right
	Region.top		=	((2 * LOWER_RIGHT_ARM + ((SelectedBodyPart == POW_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LOWER_RIGHT_ARM + ((SelectedBodyPart == POW_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LOWER_RIGHT_ARM_X_OFFSET, 
									BodyYPos + LOWER_RIGHT_ARM_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + LOWER_RIGHT_ARM_X_OFFSET;
	Region.top		= BodyYPos + LOWER_RIGHT_ARM_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = POW_GROUP;

	// ----
	// Chest plate
	Region.top		=	Y_IMAGE_OFFSET42 + ((2 * CHEST + ((SelectedBodyPart == DEF_GROUP)?1:0)) / PITCH42) * HEIGHT42;
	Region.bottom	=	Region.top + HEIGHT42;
	Region.left		=	((2 * CHEST + ((SelectedBodyPart == DEF_GROUP)?1:0)) % PITCH42) * WIDTH42;
	Region.right	=	Region.left + WIDTH42;
	GraphicsObjectRef->DrawSprite(	BodyXPos + CHEST_X_OFFSET, 
									BodyYPos + CHEST_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.right	/= 2;
	Region.left		= BodyXPos + CHEST_X_OFFSET;
	Region.top		= BodyYPos + CHEST_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = DEF_GROUP;
	
	// ----
	// Shoulder - left
	Region.top		=	((2 * LEFT_SHOULDER + ((SelectedBodyPart == ATT_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * LEFT_SHOULDER + ((SelectedBodyPart == ATT_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + LEFT_SHOULDER_X_OFFSET, 
									BodyYPos + LEFT_SHOULDER_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.bottom	/= 2;
	Region.left		= BodyXPos + LEFT_SHOULDER_X_OFFSET;
	Region.top		= BodyYPos + LEFT_SHOULDER_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = ATT_GROUP;

	// Shoulder - right
	Region.top		=	((2 * RIGHT_SHOULDER + ((SelectedBodyPart == ATT_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * RIGHT_SHOULDER + ((SelectedBodyPart == ATT_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + RIGHT_SHOULDER_X_OFFSET, 
									BodyYPos + RIGHT_SHOULDER_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.bottom	/= 2;
	Region.left		= BodyXPos + RIGHT_SHOULDER_X_OFFSET;
	Region.top		= BodyYPos + RIGHT_SHOULDER_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = ATT_GROUP;

	// ----
	// Eyes
	Region.top		=	((2 * EYES + ((SelectedBodyPart == AGR_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * EYES + ((SelectedBodyPart == AGR_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + EYES_X_OFFSET, 
									BodyYPos + EYES_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.left		= BodyXPos + EYES_X_OFFSET;
	Region.top		= BodyYPos + EYES_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = AGR_GROUP;

	// ----
	// Helmet
	Region.top		=	((2 * HELMET + ((SelectedBodyPart == INT_GROUP)?1:0)) / PITCH22) * HEIGHT22;
	Region.bottom	=	Region.top + HEIGHT22;
	Region.left		=	((2 * HELMET + ((SelectedBodyPart == INT_GROUP)?1:0)) % PITCH22) * WIDTH22;
	Region.right	=	Region.left + HEIGHT22;
	GraphicsObjectRef->DrawSprite(	BodyXPos + HELMET_X_OFFSET, 
									BodyYPos + HELMET_Y_OFFSET, 
									ArmourGraphic,
									Region);
	Region.bottom	-= Region.top;	Region.right	-= Region.left;
	Region.bottom	/= 2;
	Region.left		= BodyXPos + HELMET_X_OFFSET;
	Region.top		= BodyYPos + HELMET_Y_OFFSET;
	Region.bottom	+= Region.top;	Region.right	+= Region.left;
	if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		MouseAt = INT_GROUP;

#ifdef GYM_FUZZ
	// Frame
	Region.top		= BodyYPos - BODY_BG_BORDER;
	Region.bottom	= BodyYPos + BODY_HEIGHT + BODY_BG_BORDER;
	Region.left		= BodyXPos - BODY_BG_BORDER;
	Region.right	= BodyXPos + BODY_WIDTH + BODY_BG_BORDER;
	SAFELY_CALL(GraphicsObjectRef, DrawSprite(BodyXPos - BODY_BG_BORDER, BodyYPos - BODY_BG_BORDER, GymGraphic, Region));
#endif


	// ----
	// Draw selected item info
	Region.top		= INFO_AREA_TOP;
	Region.bottom	= Region.top + INFO_AREA_HEIGHT;
	Region.left		= INFO_AREA_LEFT;
	Region.right	= Region.left + INFO_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

	// Determine what info to show
	switch (SelectedBodyPart)
	{
	case INT_GROUP:
		Region.top		= ((2 * HELMET) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * HELMET) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Brain boost";
		TempStringArray[1] = "Intel.";
		TempStringArray[2] = "Boost";
		break;
	case AGR_GROUP:
		Region.top		= ((2 * EYES) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * EYES) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Bitmap shades";
		TempStringArray[1] = "Agrssion";
		TempStringArray[2] = "Boost";
		break;
	case ATT_GROUP:
		Region.top		= ((2 * LEFT_SHOULDER) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * LEFT_SHOULDER) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Barge pads";
		TempStringArray[1] = "Attack";
		TempStringArray[2] = "Boost";
		break;
	case THR_GROUP:
		Region.top		= ((2 * UPPER_LEFT_ARM) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * UPPER_LEFT_ARM) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Power elbows";
		TempStringArray[1] = "Throwing";
		TempStringArray[2] = "Boost";
		break;
	case POW_GROUP:
		Region.top		= ((2 * LOWER_LEFT_ARM) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * LOWER_LEFT_ARM) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Power gloves";
		TempStringArray[1] = "Power";
		TempStringArray[2] = "Boost";
		break;
	case DEF_GROUP:
		Region.top		= Y_IMAGE_OFFSET42 + ((2 * CHEST) / PITCH42) * HEIGHT42;
		Region.bottom	= Region.top + HEIGHT42;
		Region.left		= ((2 * CHEST) % PITCH42) * WIDTH42;
		Region.right	= Region.left + WIDTH42;
		TempString		= "Chest guard";
		TempStringArray[1] = "Defence";
		TempStringArray[2] = "Boost";
		break;
	case STA_GROUP:
		Region.top		= ((2 * UPPER_LEFT_LEG) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * UPPER_LEFT_LEG) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Thunder Thighs";
		TempStringArray[1] = "Stamina";
		TempStringArray[2] = "Boost";
		break;
	case SPD_GROUP:
		Region.top		= ((2 * LEFT_FOOT) / PITCH22) * HEIGHT22;
		Region.bottom	= Region.top + HEIGHT22;
		Region.left		= ((2 * LEFT_FOOT) % PITCH22) * WIDTH22;
		Region.right	= Region.left + WIDTH22;
		TempString		= "Speed boots";
		TempStringArray[1] = "Speed";
		TempStringArray[2] = "Boost";
		break;
	default:
		SAFELY_CALL(GraphicsObjectRef, EndDraw());
		return false;
		break;
	}

	// Draw item logo.
	GraphicsObjectRef->DrawSprite(	INFO_AREA_LEFT + INFO_ITEM_X_OFFSET,
									INFO_AREA_TOP + INFO_ITEM_Y_OFFSET,
									ArmourGraphic,
									Region);

	// Draw description.
	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempString,
									INFO_AREA_LEFT + INFO_DESC_X_OFFSET,
									INFO_AREA_TOP + INFO_DESC_Y_OFFSET);

	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempStringArray[1],
									INFO_AREA_LEFT + INFO_ITEM_Y_OFFSET + 45,
									INFO_AREA_TOP + 15);

	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempStringArray[2],
									INFO_AREA_LEFT + INFO_ITEM_Y_OFFSET + 45,
									INFO_AREA_TOP + 25);

	
	// Calculate price
	switch (GymMode)
	{
	case EFEGM_Individual:
		i = 1;
		break;
	case EFEGM_Group:
		i = 3;
		break;
	case EFEGM_Team:
		i = 12;
		break;
	}

	char	convert[24];
	sprintf(convert, "%03d Crd", PartsList[SelectedBodyPart] * i);
	TempString		= convert;
	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempString,
									INFO_AREA_LEFT + INFO_PRICE_X_OFFSET,
									INFO_AREA_TOP + INFO_PRICE_Y_OFFSET);


	
	// Draw selected item info
	Region.top		= INFO_AREA_TOP;
	Region.bottom	= Region.top + INFO_AREA_HEIGHT;
	Region.left		= INFO_AREA_LEFT;
	Region.right	= Region.left + INFO_AREA_WIDTH;
	GraphicsObjectRef->DrawSprite(	INFO_AREA_LEFT,
									INFO_AREA_TOP, 
									GymGraphic,
									Region);
	

	// If the user presses fire - key only - when selecting
	// a body part it will purchase that part for the 
	// selected group.
	if (	(CurrentMenuPos >= 12)
		&&	(ControllerRef->IsButtonPressedDB(Select)))
	{
		PLAY_SELECT;	// Play select sample
		switch (GymMode)
		{
		case EFEGM_Individual:
			PurchaseAssistant(SelectedPlayer, 1);
			break;

		case EFEGM_Group:
			switch (SelectedGroup)
			{
			case EFEPG_Defence:
				k = 6;
				break;
			case EFEPG_Midfield:
				k = 3;
				break;
			case EFEPG_Attack:
				k = 0;
				break;
			case EFEPG_Substitute:
				k = 9;
				break;
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
			PurchaseAssistant(k, 3);
			break;

		case EFEGM_Team:
			PurchaseAssistant(0, 12);
			break;

		default:
			break;
		}
	}



	// ----
	// All button hack (work around).
	if (AllSelect < 0)
	{
		// Update menu pos if the mouse has done something useful.
		if (MouseAt != -1)
		{
			if (CurrentMenuPos != (MouseAt + PART_NAV))
			{
				SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
			}

			CurrentMenuPos = MouseAt + PART_NAV;
			MouseAt = -1;
		}

		// ----
		// Keypad

		// Detect mouse pointer over keypad.
		for (i = MAX_XKEYS; i--;)
		{
			for (j = MAX_YKEYS; j--;)
			{
				if (j == (MAX_YKEYS - 1))
				{
					if (	(i == 1) 
						||	(i == 2))
						continue;	// No keys here!!! (The bank balance is shown here).
				}

				// check mouse pointer location against key area.
				POINT	*MP = GameWindowRef->GetMousePoint();
				if (	(MP) 
					&&	(MP->x >= (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i))
					&& 	(MP->x <  (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * (i + 1)))
					&& 	(MP->y >= ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j))
					&& 	(MP->y <  ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * (j + 1)))
					)
					MouseAt = i + j * MAX_XKEYS;
			}
		}


		// Update menu pos if the mouse has done something useful.
		if (MouseAt != -1)
		{
			if (CurrentMenuPos != MouseAt)
			{
				SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
			}

			CurrentMenuPos = MouseAt;
		}

		// If the user has selected a body part then update the SelectedBodyPart.
		if (CurrentMenuPos >= PART_NAV)
			SelectedBodyPart = CurrentMenuPos - PART_NAV;


		// ----
		// Draw keys
		for (i = MAX_XKEYS; i--;)
		{
			for (j = MAX_YKEYS; j--;)
			{
				int		Key;

				if (j == (MAX_YKEYS - 1))
				{
					if (	(i == 1) 
						||	(i == 2))
						continue;	// No keys here!!! (The bank balance is shown here).
				}

				Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
				Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
				Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
				Region.right	= Region.left + KEY_LIGHT_WIDTH;
				GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

				Key = KeyMatrix[i][j];

				if (Key < 0)
					Key = BLANK_KEY;	
					
				if (	(Key != BLANK_KEY) 
					&&	(DialogInitialised)
					&&	(i == (CurrentMenuPos % MAX_XKEYS))
					&&	(j == (CurrentMenuPos / MAX_XKEYS)))
				{
					if (	ControllerRef->IsButtonPressed(Select)
						||	(	(MouseAt != -1) 
							&&	(ControllerRef->IsButtonPressed(MouseSelect))
							)
						)
					{
						// Now check the debounce status.
						if (	ControllerRef->IsButtonPressedDB(Select)
							||	(	(MouseAt != -1) 
								&&	(ControllerRef->IsButtonPressedDB(MouseSelect))
								)
							)
						{
							switch (Key)
							{
							// Modify the scope of the changes selected
							case I_KEY:
								PLAY_SELECT;	// Play select sample
								if (GymMode != EFEGM_Individual) 
									GymMode = EFEGM_Individual;
								else
								{
									SelectedPlayer++;
									if (SelectedPlayer > 11)
										SelectedPlayer = 0;
								}
								break;
							case G_KEY:
								PLAY_SELECT;	// Play select sample
								if (GymMode != EFEGM_Group)
									GymMode = EFEGM_Group;
								else
								{
									switch (SelectedGroup)
									{
									case EFEPG_Defence:
										SelectedGroup = EFEPG_Midfield;
										break;
									case EFEPG_Midfield:
										SelectedGroup = EFEPG_Attack;
										break;
									case EFEPG_Attack:
										SelectedGroup = EFEPG_Substitute;
										break;
									case EFEPG_Substitute:
										SelectedGroup = EFEPG_Defence;
										break;
									default:
										SAFELY_CALL(GraphicsObjectRef, EndDraw());
										return false;
									}
								}

								break;
							case T_KEY:
								PLAY_SELECT;	// Play select sample
								GymMode = EFEGM_Team;
								break;

							// Buy one of everything for selected group.
							case ALL_KEY:
								AllSelect	= BP_COUNT - 1;
								PLAY_SELECT;	// Play select sample
								break;

							// Cycle through players, etc
							case UP_KEY:
								PLAY_SELECT;	// Play select sample
								switch (GymMode)
								{
								case EFEGM_Individual:
									SelectedPlayer++;
									if (SelectedPlayer > 11)
										SelectedPlayer = 0;
									break;
								case EFEGM_Group:
									switch (SelectedGroup)
									{
									case EFEPG_Defence:
										SelectedGroup = EFEPG_Midfield;
										break;
									case EFEPG_Midfield:
										SelectedGroup = EFEPG_Attack;
										break;
									case EFEPG_Attack:
										SelectedGroup = EFEPG_Substitute;
										break;
									case EFEPG_Substitute:
										SelectedGroup = EFEPG_Defence;
										break;
									default:
										SAFELY_CALL(GraphicsObjectRef, EndDraw());
										return false;
									}
									break;
								case EFEGM_Team:
									// Does nothing in this mode
									break;
								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
								}
								break;
							case DOWN_KEY:
								PLAY_SELECT;	// Play select sample
								switch (GymMode)
								{
								case EFEGM_Individual:
									SelectedPlayer--;
									if (SelectedPlayer < 0)
										SelectedPlayer = 11;
									break;
								case EFEGM_Group:
									switch (SelectedGroup)
									{
									case EFEPG_Defence:
										SelectedGroup = EFEPG_Substitute;
										break;
									case EFEPG_Midfield:
										SelectedGroup = EFEPG_Defence;
										break;
									case EFEPG_Attack:
										SelectedGroup = EFEPG_Midfield;
										break;
									case EFEPG_Substitute:
										SelectedGroup = EFEPG_Attack;
										break;
									default:
										SAFELY_CALL(GraphicsObjectRef, EndDraw());
										return false;
									}
									break;
								case EFEGM_Team:
									// Does nothing in this mode
									break;
								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
								}
								break;

							// Go to fixtures screen
							case FIX_KEY:
								PLAY_SELECT;	// Play select sample
								switch (FrontEndState)
								{
								case EFES_Single_KnockOut_Gym:
									SetState(EFES_Single_KnockOut_Fixtures);
									break;
								case EFES_Single_League_New_Gym:
									SetState(EFES_Single_League_New_Gym_Fixtures);
									break;
								case EFES_Single_League_TeamManager_Gym:
									SetState(EFES_Single_League_TeamManager_Gym_Fixtures);
									break;
								case EFES_Single_Cup_New_Gym:
									SetState(EFES_Single_Cup_New_Gym_Fixtures);
									break;
								default:
									NODEFAULT;
									break;
								}
								break;


							// Leave this screen (varies depending on mode).
							case PLY_KEY:
							case ESC_KEY:
								PLAY_SELECT;	// Play select sample
								switch (FrontEndState)
								{
								case EFES_Single_KnockOut_Gym:
									SetState(EFES_Single_KnockOut_Game);
									break;
								case EFES_Single_League_New_Gym:
									SetState(EFES_Single_League_New_Manager);
									break;
								case EFES_Single_League_TeamManager_Gym:
									SetState(EFES_Single_League_TeamManager_Manager);
									break;
								case EFES_Single_Cup_New_Gym:
									SetState(EFES_Single_Cup_New_Manager);
									break;
								default:
									NODEFAULT;
									break;
								}
								break;

							// Buy an item
							case BUY_KEY:
								PLAY_SELECT;	// Play select sample
								switch (GymMode)
								{
								case EFEGM_Individual:
									PurchaseAssistant(SelectedPlayer, 1);
									break;

								case EFEGM_Group:
									switch (SelectedGroup)
									{
									case EFEPG_Defence:
										k = 6;
										break;
									case EFEPG_Midfield:
										k = 3;
										break;
									case EFEPG_Attack:
										k = 0;
										break;
									case EFEPG_Substitute:
										k = 9;
										break;
									default:
										SAFELY_CALL(GraphicsObjectRef, EndDraw());
										return false;
									}
									PurchaseAssistant(k, 3);
									break;

								case EFEGM_Team:
									PurchaseAssistant(0, 12);
									break;

								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
								}
								break;

							default:
								break;
							}
						}

						Key += LIT_KEY_ADDER;
					}
					else
					{
						// Fill the region of the key with a 
						// 'highlight' colour, which will show 
						// through the transparent part of the 
						// key.
						Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
						Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
						Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
						Region.right	= Region.left + KEY_LIGHT_WIDTH;
						GraphicsObjectRef->DrawRect(&Region, 0, 140, 0);
					}
				}

				// Calculate key 
				Region.top		=	(Key / KEY_PITCH) * KEY_HEIGHT;
				Region.bottom	=	Region.top + KEY_HEIGHT;
				Region.left		=	(Key % KEY_PITCH) * KEY_WIDTH;
				Region.right	=	Region.left + KEY_WIDTH;
			
				GraphicsObjectRef->DrawSprite(	KEY_MATRIX_X_OFFSET  + X_KEY_SPACING * i, 
												(int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j, 
												KeysGraphic,
												Region);	

			}
		}
	}
	else
	{
		// The interface is locked whilst doing the all 
		// purchase, there for the mode will not change.
		switch (GymMode)
		{
		case EFEGM_Individual:
			PurchaseAssistant(SelectedPlayer, 1);
			break;
		case EFEGM_Group:
			switch (SelectedGroup)
			{
			case EFEPG_Defence:
				k = 6;
				break;
			case EFEPG_Midfield:
				k = 3;
				break;
			case EFEPG_Attack:
				k = 0;
				break;
			case EFEPG_Substitute:
				k = 9;
				break;
			default:
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
			PurchaseAssistant(k, 3);
			break;
		case EFEGM_Team:
			PurchaseAssistant(0, 12);
			break;
		}
			
		AllSelect--;
	}

	// ----
	// Draw player / group stats, info, etc
	Region.top		= AREA1_TOP;
	Region.bottom	= AREA1_BOTTOM;
	Region.left		= AREA1_LEFT;
	Region.right	= AREA1_RIGHT;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);
	
	int			Face;
	CTeamMember	*TMRef;
	switch (GymMode)
	{
	case EFEGM_Individual:
		DrawStatBars(SelectedPlayer, 1);
		TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(SelectedPlayer);
		if (TMRef)
		{
			// Faces
			Face = TMRef->GetPictureNumber();
			Region.top		= (Face / FACE_PITCH) * FACE_HEIGHT;
			Region.bottom	= Region.top + FACE_HEIGHT;
			Region.left		= (Face % FACE_PITCH) * FACE_WIDTH;
			Region.right	= Region.left + FACE_WIDTH;
			GraphicsObjectRef->DrawSprite(	FACE_X,
											FACE_Y,
											FacesGraphic, 
											Region);

			// Position
			GraphicsObjectRef->DrawString(	SmallLightFont,
											&PositionCaptionArray[TMRef->getPosition()],
											POSITION_TEXT_X,
											POSITION_TEXT_Y,
											TEXT_RIGHT);
			
			// Worth
			char	convert[24];
			sprintf(convert, "%04d", TMRef->getWorth());
			TempString = convert;
			GraphicsObjectRef->DrawString(	SmallLightFont,
											&TempString,
											WORTH_TEXT_X,
											WORTH_TEXT_Y,
											TEXT_RIGHT);

			// Player name
			GraphicsObjectRef->DrawString(	SmallLightFont,
											TMRef->getName(),
											NAME_TEXT_X,
											NAME_TEXT_Y,
											TEXT_RIGHT);


		}
		break;
	
	case EFEGM_Group:
		// Draw stats bars
		switch (SelectedGroup)
		{
		case EFEPG_Defence:
			DrawStatBars(6, 3);
			break;
		case EFEPG_Midfield:
			DrawStatBars(3, 3);
			break;
		case EFEPG_Attack:
			DrawStatBars(0, 3);
			break;
		case EFEPG_Substitute:
			DrawStatBars(9, 3);
			break;
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		// Shield!!!
		Region.top		= 0;
		Region.bottom	= SHIELD_HEIGHT;
		Region.left		= 0;
		Region.right	= SHIELD_WIDTH;
		GraphicsObjectRef->DrawSprite(	SHIELD_X,
										SHIELD_Y,
										ShieldGraphic, 
										Region);

		Region.top		= 0;
		Region.bottom	= Region.top + SHIELD_LETTERS_HEIGHT;
		Region.left		= (((int)SelectedGroup) + 1) * SHIELD_LETTERS_WIDTH;
		Region.right	= Region.left + SHIELD_LETTERS_WIDTH;
		GraphicsObjectRef->DrawSprite(	SHIELD_LETTERS_X,
										SHIELD_LETTERS_Y,
										ShieldLetterGraphic, 
										Region);
		break;

	case EFEGM_Team:
		DrawStatBars(0, 12);

		// Draw shield and Team inlay.
		// Shield!!!
		Region.top		= 0;
		Region.bottom	= SHIELD_HEIGHT;
		Region.left		= 0;
		Region.right	= SHIELD_WIDTH;
		GraphicsObjectRef->DrawSprite(	SHIELD_X,
										SHIELD_Y,
										ShieldGraphic, 
										Region);

		Region.top		= 0;
		Region.bottom	= Region.top + SHIELD_LETTERS_HEIGHT;
		Region.left		= 0;
		Region.right	= Region.left + SHIELD_LETTERS_WIDTH;
		GraphicsObjectRef->DrawSprite(	SHIELD_LETTERS_X,
										SHIELD_LETTERS_Y,
										ShieldLetterGraphic, 
										Region);

		break;

	case EFEGM_Error:
	default:
		SAFELY_CALL(GraphicsObjectRef, EndDraw());
		return false;
		break;
	}

	// Print the balance, after clearing the area.
	Region.top		= BALANCE_TEXT_Y;
	Region.bottom	= Region.top + BALANACE_TEXT_HEIGHT;
	Region.left		= BALANCE_TEXT_X;
	Region.right	= Region.left + BALANACE_TEXT_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

	sprintf(convert, "%04d", GameDataRef->GetActiveTeamByIndex(0)->GetBalance());
	TempString = convert;
	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempString,
									BALANCE_TEXT_X + BALANACE_TEXT_WIDTH,	// Notice that the balance is right aligned!!!
									BALANCE_TEXT_Y,
									TEXT_RIGHT);

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
// Managers interface
#define	MANAGER_PLAYER_AREA_LEFT			127
#define	MANAGER_PLAYER_AREA_TOP				20
#define	MANAGER_PLAYER_AREA_WIDTH			91
#define	MANAGER_PLAYER_AREA_HEIGHT			280

#define	FACE_SPACE							(FACE_HEIGHT + 8)
#define	FACE_SELECTED_SPACE					(FACE_SPACE - 4)

#define	MANAGER_FACE_X_OFFSET				0
#define	MANAGER_FACE_Y_OFFSET				(12 + 18)

#define	MANAGER_FACE_POSITON_LABEL_X_OFFSET	(-2)

#define	MANAGER_SCROLL_HEIGHT				16
bool	CFrontEnd::Manager()
{
	// Do we have a problem?
	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef)
		||	(!GameDataRef)
		||	(!GameDataRef->GetActiveTeamByIndex(0)))
		return false;
	
	if (!ManagerGraphic)
	{
		NEW(ManagerGraphic, CGraphicsData());
		SAFELY_CONTROL(if, ManagerGraphic, Init(GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), PlatformRef), ==, false)
			return false;
	}

	// Quit game
	if (ControllerRef->IsButtonPressed(Backup))
	{
		PLAY_BACKUP;
		SetState(EFES_ConfirmAbandonCurrent);
		return true;
	}
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i, j;//, k;
	int		MouseAt = -1;
	RECT Region;
		
	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		// Keypad definition.
		for (int xkey = MAX_XKEYS; xkey--;)
		{
			for (int ykey = MAX_YKEYS; ykey--;)
			{
				KeyMatrix[xkey][ykey] = BLANK_KEY;
			}
		}

		// Top row.
		KeyMatrix[0][0]			= TRA_KEY;
		KeyMatrix[1][0]			= SAV_KEY;				
		KeyMatrix[2][0]			= PLY_KEY;
		KeyMatrix[3][0]			= UP_KEY;
								
		// Middle row.			
		KeyMatrix[0][1]			= TAB_KEY;
		KeyMatrix[1][1]			= GYM_KEY;
		KeyMatrix[2][1]			= FIX_KEY;
		KeyMatrix[3][1]			= DOWN_KEY;
								
		// Bottom row.			
		KeyMatrix[0][2]			= STA_KEY;
		// Blank			
		// Blank			
		KeyMatrix[3][2]			= SUB_KEY;

		// Select matrix key 2, 1
		CurrentMenuPos			= 2 + (MAX_XKEYS * 1);	// Fix key.
		SelectedPlayer			= 0;
		SelectedFace			= 0;
		FaceOffset				= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		GraphicsObjectRef->ClearScreen(0,0,0);
/*
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, ManagerGraphic, all));
		
	}	

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// Navigation
	IfUpDB					// Macro
	{
		switch (CurrentMenuPos)
		{
		// Top row
		case 0:
			CurrentMenuPos = 0 + MAX_XKEYS * 2;
			break;
		case 1:
			CurrentMenuPos = 1 + MAX_XKEYS * 1;
			break;
		case 2:
			CurrentMenuPos = 2 + MAX_XKEYS * 1;
			break;
		case 3:
			CurrentMenuPos = 3 + MAX_XKEYS * 2;
			break;

		// Second row
		case 4:
		case 5:
		case 6:
		case 7:
		// Third
		case 8:
		case 11:
			CurrentMenuPos -= MAX_XKEYS;
			break;

		
		// Navigate players
		case 12:
			//CurrentMenuPos = 23;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			CurrentMenuPos--;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfDownDB
	{
		switch (CurrentMenuPos)
		{
		// Top row
		case 0:
		case 1:
		case 2:
		case 3:
		// Second row
		case 4:
		case 7:
			CurrentMenuPos += MAX_XKEYS;
			break;

		case 5:
			CurrentMenuPos = 1;
			break;
		case 6:
			CurrentMenuPos = 2;
			break;
		case 8:
			CurrentMenuPos = 0;
			break;
		case 11:
			CurrentMenuPos = 3;
			break;

		// Navigate the players
		case 12:			// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			CurrentMenuPos++;
			break;
		case 23:
			//CurrentMenuPos = MAX_XKEYS * MAX_YKEYS;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfLeftDB
	{
		switch (CurrentMenuPos)
		{
		// Left col
		case 0:
		case 4:
		case 8:
			CurrentMenuPos += MAX_XKEYS - 1;
			break;

		case 1:
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
			CurrentMenuPos--;
			break;
		
		case 11:
			CurrentMenuPos = 8;
			break;

		// Navigate off player
		case 12:	// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			CurrentMenuPos = MAX_XKEYS - 1;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfRightDB
	{
		switch (CurrentMenuPos)
		{
		// Right col
		case 3:
		case 7:
		case 11:
			CurrentMenuPos = MAX_XKEYS * MAX_YKEYS + SelectedFace;
			break;

		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
			CurrentMenuPos++;
			break;
		
		case 8:
			CurrentMenuPos = 11;
			break;

		case 12:	// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}


	// RENDER PLAYERS HERE. DO MOUSE SELECTION STUFF TOO!!!
	
	// Draw background
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_PLAYER_AREA_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 20, 0);


	// Do mouse scroll up 
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_SCROLL_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 10, 100, 10);
	if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
		&&	(IsPointInRect(GameWindowRef->GetMousePoint(), &Region)))
	{
		PLAY_SELECT;
		GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);

		if (SelectedFace > 0)
			SelectedFace--;

		MouseAt = PART_NAV + SelectedFace;
	}

	Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_PLAYER_AREA_HEIGHT - MANAGER_SCROLL_HEIGHT;
	Region.bottom	= Region.top + MANAGER_SCROLL_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 10, 100, 10);
	if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
		&&	(IsPointInRect(GameWindowRef->GetMousePoint(), &Region)))
	{
		PLAY_SELECT;
		GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);

		if (SelectedFace < 11)
			SelectedFace++;

		MouseAt = PART_NAV + SelectedFace;
	}

	// Do direct mouse detect bit
	for (i = 0; i < 4; i++)
	{
		Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE;
		Region.bottom	= Region.top + FACE_SELECTED_SPACE;
		Region.left		= MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET;
		Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;

		if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		{
			if (CurrentMenuPos != PART_NAV + FaceOffset + i)
			{
				PLAY_SELECT;
				MouseAt = PART_NAV + FaceOffset + i;
			}
			break;
		}
	}

	// Detect mouse pointer over keypad.
	for (i = MAX_XKEYS; i--;)
	{
		for (j = MAX_YKEYS; j--;)
		{
			if (j == (MAX_YKEYS - 1))
			{
				if (	(i == 1) 
					||	(i == 2))
					continue;	// No keys here!!! (The bank balance is shown here).
			}

			// check mouse pointer location against key area.
			POINT	*MP = GameWindowRef->GetMousePoint();
			if (	(MP) 
				&&	(MP->x >= (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i))
				&& 	(MP->x <  (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * (i + 1)))
				&& 	(MP->y >= ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j))
				&& 	(MP->y <  ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * (j + 1)))
				)
				MouseAt = i + j * MAX_XKEYS;
		}
	}

	// Update mouse if it has been useful.
	if (MouseAt >= 0)
	{
		CurrentMenuPos = MouseAt;
	}

	// All input has been takeinto account - now relate new position to rendering.
	if (CurrentMenuPos >= PART_NAV)
		SelectedFace = CurrentMenuPos - PART_NAV;

	if (SelectedFace < FaceOffset)
		FaceOffset = SelectedFace;

	if (SelectedFace >= (FaceOffset + 4))
		FaceOffset = SelectedFace - 3;
	
	for (i = 0; i < 4; i++)
	{
		if ((FaceOffset + i) == SelectedFace)
		{
			Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE;
			Region.bottom	= Region.top + FACE_SELECTED_SPACE;
			Region.left		= MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET;
			Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
			GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);
		}

		CTeamMember*	TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(FaceOffset + i);

		// Faces
		int Face = TMRef->GetPictureNumber();
		Region.top		= (Face / FACE_PITCH) * FACE_HEIGHT;
		Region.bottom	= Region.top + FACE_HEIGHT;
		Region.left		= (Face % FACE_PITCH) * FACE_WIDTH;
		Region.right	= Region.left + FACE_WIDTH;
		GraphicsObjectRef->DrawSprite(	MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + 1,
										FacesGraphic,
										Region);

		// Position
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&PositionCaptionArray[TMRef->getPosition()],
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 3 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

		// Worth
		char convert[24];
		sprintf(convert, "%04d", TMRef->getWorth());
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&TempString,
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 2 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

		// Name
		GraphicsObjectRef->DrawString(	SmallLightFont,
										TMRef->getName(),
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 1 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

	}

	// Draw frame
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_PLAYER_AREA_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawSprite(	MANAGER_PLAYER_AREA_LEFT,
									MANAGER_PLAYER_AREA_TOP,
									ManagerGraphic,
									Region);
	
	// ----
	// Draw keys
	for (i = MAX_XKEYS; i--;)
	{
		for (j = MAX_YKEYS; j--;)
		{
			int		Key;

			if (j == (MAX_YKEYS - 1))
			{
				if (	(i == 1) 
					||	(i == 2))
					continue;	// No keys here!!! (The bank balance is shown here).
			}

			Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
			Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
			Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
			Region.right	= Region.left + KEY_LIGHT_WIDTH;
			GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

			Key = KeyMatrix[i][j];

			if (Key < 0)
				Key = BLANK_KEY;	
				
			if (	(Key != BLANK_KEY) 
				&&	(DialogInitialised)
				&&	(i == (CurrentMenuPos % MAX_XKEYS))
				&&	(j == (CurrentMenuPos / MAX_XKEYS)))
			{
				if (	ControllerRef->IsButtonPressed(Select)
					||	(	(MouseAt != -1) 
						&&	(ControllerRef->IsButtonPressed(MouseSelect))
						)
					)
				{
					// Now check the debounce status.
					if (	ControllerRef->IsButtonPressedDB(Select)
						||	(	(MouseAt != -1) 
							&&	(ControllerRef->IsButtonPressedDB(MouseSelect))
							)
						)
					{
						switch (Key)
						{
						case TRA_KEY:	// TRANSFER SCREEN SELECTED
							PLAY_SELECT;	// Play select sample

							// Only proceed to transfer screen if there are players for sale.
							if (GameDataRef->GetNextStarPlayerIndex(-1) != -1)
							{
								// New mode depends on current mode
								switch (FrontEndState)	
								{
								case EFES_Single_Cup_New_Manager:
									SetState(EFES_Single_Cup_New_Transfer);
									break;
								case EFES_Single_League_TeamManager_Manager:
									SetState(EFES_Single_League_TeamManager_Transfer);
									break;
									
								case EFES_Single_League_New_Manager:
									SetState(EFES_Single_League_New_Transfer);
									break;
								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
									break;
								}
							}
							break;

						// SAVE
						case SAV_KEY:
							PLAY_SELECT;	// Play select sample
						
							switch (FrontEndState)
							{
							case EFES_Single_League_New_Manager:
								SetState(EFES_Single_League_New_Save);
								break;
							case EFES_Single_League_TeamManager_Manager:
								SetState(EFES_Single_League_TeamManager_Save);
								break;
							case EFES_Single_Cup_New_Manager:
								SetState(EFES_Single_Cup_New_Save);
								break;
							default:
								NODEFAULT;
								break;
							}
							break;

						// ESC
						case PLY_KEY:
							PLAY_SELECT;	// Play select sample
						
							switch (FrontEndState)
							{
							case EFES_Single_League_New_Manager:
								SetState(EFES_Single_League_New_Game);
								break;
							case EFES_Single_League_TeamManager_Manager:
								SetState(EFES_Single_League_TeamManager_Game);
								break;
							case EFES_Single_Cup_New_Manager:
								SetState(EFES_Single_Cup_New_Game);
								break;
							default:
								NODEFAULT;
								break;
							}
							break;
						
						// UP
						case UP_KEY:
							PLAY_SELECT;	// Play select sample
							SelectedPlayer--;
							if (SelectedPlayer < 0)
								SelectedPlayer = 11;
							break;


						// ----
						// NEXT ROW

						// TABLES
						case TAB_KEY:
							PLAY_SELECT;	// Play select sample
							
							// Not going to allow table in playoff mode.
							if (!GameDataRef->IsThisALeagueHumanPlayOff())
							{
								switch (FrontEndState)	
								{
								case EFES_Single_Cup_New_Manager:
									SetState(EFES_Single_Cup_New_Tables);
									break;
								case EFES_Single_League_TeamManager_Manager:
									SetState(EFES_Single_League_TeamManager_Tables);
									break;
									
								case EFES_Single_League_New_Manager:
									SetState(EFES_Single_League_New_Tables);
									break;
								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
									break;
								}
							}
							break;

						// GYM
						case GYM_KEY:
							PLAY_SELECT;	// Play select sample
							
							switch (FrontEndState)	
							{
							case EFES_Single_Cup_New_Manager:
								SetState(EFES_Single_Cup_New_Gym);
								break;
							case EFES_Single_League_TeamManager_Manager:
								SetState(EFES_Single_League_TeamManager_Gym);
								break;
								
							case EFES_Single_League_New_Manager:
								SetState(EFES_Single_League_New_Gym);
								break;
							default:
								SAFELY_CALL(GraphicsObjectRef, EndDraw());
								return false;
								break;
							}
							break;

						// FIXTURES
						case FIX_KEY:
							PLAY_SELECT;	// Play select sample
							
							switch (FrontEndState)	
							{
							case EFES_Single_Cup_New_Manager:
								SetState(EFES_Single_Cup_New_Manager_Fixtures);
								break;
							case EFES_Single_League_TeamManager_Manager:
								SetState(EFES_Single_League_TeamManager_Manager_Fixtures);
								break;
								
							case EFES_Single_League_New_Manager:
								SetState(EFES_Single_League_New_Manager_Fixtures);
								break;
							default:
								SAFELY_CALL(GraphicsObjectRef, EndDraw());
								return false;
								break;
							}
							break;


						// DOWN
						case DOWN_KEY:
							PLAY_SELECT;	// Play select sample
							SelectedPlayer++;
							if (SelectedPlayer > 11)
								SelectedPlayer = 0;
							break;

						// ----
						// NEXT ROW

						// STATS
						case STA_KEY:
							PLAY_SELECT;	// Play select sample
							
							if (	(GameDataRef->GetTeamFromDivision(GET_HUMAN_TEAM, SELECT_FROM_HUMAN_PLAYERS_DIVISION))
								&&	(GameDataRef->GetLastTeamPlayed()))
							{
								switch (FrontEndState)	
								{
								case EFES_Single_Cup_New_Manager:
									SetState(EFES_Single_Cup_New_Statistics);
									break;
								case EFES_Single_League_TeamManager_Manager:
									SetState(EFES_Single_League_TeamManager_Statistics);
									break;
									
								case EFES_Single_League_New_Manager:
									SetState(EFES_Single_League_New_Statistics);
									break;
								default:
									SAFELY_CALL(GraphicsObjectRef, EndDraw());
									return false;
									break;
								}
							}
							break;

						// SUB
						case SUB_KEY:
							PLAY_SELECT;	// Play select sample
							GameDataRef->GetActiveTeamByIndex(0)->SwapTeamMembers(SelectedPlayer, SelectedFace);
							break;

						default:
							break;
						}
					}

					Key += LIT_KEY_ADDER;
				}
				else
				{
					// Fill the region of the key with a 
					// 'highlight' colour, which will show 
					// through the transparent part of the 
					// key.
					Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
					Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
					Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
					Region.right	= Region.left + KEY_LIGHT_WIDTH;
					GraphicsObjectRef->DrawRect(&Region, 0, 140, 0);
				}
			}

			// Calculate key 
			Region.top		=	(Key / KEY_PITCH) * KEY_HEIGHT;
			Region.bottom	=	Region.top + KEY_HEIGHT;
			Region.left		=	(Key % KEY_PITCH) * KEY_WIDTH;
			Region.right	=	Region.left + KEY_WIDTH;
		
			GraphicsObjectRef->DrawSprite(	KEY_MATRIX_X_OFFSET  + X_KEY_SPACING * i, 
											(int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j, 
											KeysGraphic,
											Region);	

		}
	}

	// ----
	// Draw player / group stats, info, etc
	Region.top		= AREA1_TOP;
	Region.bottom	= AREA1_BOTTOM;
	Region.left		= AREA1_LEFT;
	Region.right	= AREA1_RIGHT;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);
	
	int			Face;
	CTeamMember	*TMRef;

	DrawStatBars(SelectedPlayer, 1);
	TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(SelectedPlayer);
	if (TMRef)
	{
		// Faces
		Face = TMRef->GetPictureNumber();
		Region.top		= (Face / FACE_PITCH) * FACE_HEIGHT;
		Region.bottom	= Region.top + FACE_HEIGHT;
		Region.left		= (Face % FACE_PITCH) * FACE_WIDTH;
		Region.right	= Region.left + FACE_WIDTH;
		GraphicsObjectRef->DrawSprite(	FACE_X,
										FACE_Y,
										FacesGraphic, 
										Region);

		// Position
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&PositionCaptionArray[TMRef->getPosition()],
										POSITION_TEXT_X,
										POSITION_TEXT_Y,
										TEXT_RIGHT);
		
		// Worth
		char	convert[24];
		sprintf(convert, "%04d", TMRef->getWorth());
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&TempString,
										WORTH_TEXT_X,
										WORTH_TEXT_Y,
										TEXT_RIGHT);

		// Player name
		GraphicsObjectRef->DrawString(	SmallLightFont,
										TMRef->getName(),
										NAME_TEXT_X,
										NAME_TEXT_Y,
										TEXT_RIGHT);
	}
	
	// ----
	// Print the balance, after clearing the area.
	Region.top		= BALANCE_TEXT_Y;
	Region.bottom	= Region.top + BALANACE_TEXT_HEIGHT;
	Region.left		= BALANCE_TEXT_X;
	Region.right	= Region.left + BALANACE_TEXT_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

	char	convert[24];
	sprintf(convert, "%04d", GameDataRef->GetActiveTeamByIndex(0)->GetBalance());
	TempString = convert;
	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempString,
									BALANCE_TEXT_X + BALANACE_TEXT_WIDTH,	// Notice that the balance is right aligned!!!
									BALANCE_TEXT_Y,
									TEXT_RIGHT);
		
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}


// ----
// Ready for the real game
bool	CFrontEnd::Game()
{
	if (!GameDataRef)
		return false;

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		LoadingScreen();
	
		// Lets set the data 
		switch (GameDataRef->GetGameType())
		{
		case LEAGUE:
		case LEAGUE_TEAM_MANAGER:
			GameDataRef->LeagueGenerateScores();
			break;
		case CUP:
			GameDataRef->CupGenerateScores();	// This method is currently null bodied
			break;
		case KNOCKOUT:
			// Don't need to do anything, other than reset the scores. :o)
			GameDataRef->GetActiveTeamByIndex(0)->Score	= 0;	// Human
			GameDataRef->GetActiveTeamByIndex(1)->Score	= 0;	// Opponents.
			break;
		case PRACTICE:
			GameDataRef->GetActiveTeamByIndex(0)->Score	= 0;	// Only human players
			break;
		default:
			return false;
		}

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	return true;
}


// ----
// Demo game. (Cound be fun as need to go into game).
bool	CFrontEnd::Demo()
{
	if (!DialogInitialised)
	{
		// Bit of a hack, but things are getting called in the wrong order. 
		// LoadingScreen is relient on GameType having been set. 
		// It is set a few lines down. 
		// Thus, the music would have been stopped the first time into a 
		// demo after another game type.
		GameDataRef->GameType = DEMO;	

		LoadingScreen();
		RedrawCountDown = 0;
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	SAFELY_CALL(GameDataRef, Init(DEMO, PlatformRef));
	return true;
}

// ----
// Confirm abandon current game.
bool	CFrontEnd::ConfirmAbandon()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Yes";
		MenuTextArray[1]	= "No";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}
		
	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions) + 40;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_QUIT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// "Are you sure you want to abandon game?" message.
		TempString = "Are you sure";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + -2 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		TempString = "you want to";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + -1 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		TempString = "abandon game?";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + 0 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											(MENU_TOP + i * MENU_ITEM_SPACING) + 40,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										(MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING) + 40,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										(MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING) + 40,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Main);
			break;
		case 1:		
			SetState(PreviousFEState);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
bool	CFrontEnd::ConfirmQuit()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Yes";
		MenuTextArray[1]	= "No";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

	}
		
	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions) + 40;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_QUIT),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		TempString = "Are you sure";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + -2 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		// "Are you sure you want to quit?" message.
		TempString = "you want";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + -1 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		TempString = "to quit?";
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP + 0 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											(MENU_TOP + i * MENU_ITEM_SPACING) + 40,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										(MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING) + 40,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										(MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING) + 40,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SetState(EFES_Quit);
			break;
		case 1:		
			SetState(EFES_Main);
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}


// ----
bool CFrontEnd::LoadingScreen()
{
	if (GraphicsObjectRef)
	{
		if (	(GameDataRef) 
			&&	(GameDataRef->GetGameType() != DEMO))
		{
			SAFELY_CALL(SoundSystemRef, StopMod());
		}
			
		SAFELY_CALL(SoundSystemRef, StopAllSamples());
		SAFELY_CALL(GraphicsObjectRef, StartDraw());

		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_MAIN),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

	
		SAFELY_CALL(GraphicsObjectRef, DrawString(	MainMenuFont,	
													&CString(TEXT_LOADING_MESSAGE),
													(GraphicsObjectRef->GetDisplay()->cxWidth >> 1), 
													((GraphicsObjectRef->GetDisplay()->cyHeight - MainMenuFont->GetLineHeight()) >> 1),
													TEXT_CENTRED
													)
													
					);

		SAFELY_CALL(GraphicsObjectRef, EndDraw());

		return true;
	}
	return false;
}


// ----
// Processes a transaction.
// A 'false' return value indicates that the purchase failed.
bool	CFrontEnd::PurchaseAssistant(	int	_InitialPlayer, 
										int	_PlayerCount)
{
	int k, l, temp;
	k = _InitialPlayer;
	int upper = Round(GameDataRef->GetActiveTeamByIndex(0)->GetAverageStats() + UPGRADE_BUFFER);
	
	// Determine if there is sufficient money to purchase the equipment for the requested number of players.
	if ((PartsList[SelectedBodyPart] * _PlayerCount) <= GameDataRef->GetActiveTeamByIndex(0)->GetBalance())
	{
		// Enough money... now loop through players seeing if they need the item.
		// If their stats can be increased then do so and deduct money.
		for (l = _PlayerCount; l--;)
		{
			switch (SelectedBodyPart)
			{
			case INT_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getInt();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					// Take money
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					
					// Increase skill and worth.
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setInt((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case AGR_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getAgr();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setAgr((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case ATT_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getAtt();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setAtt((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case THR_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getThr();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setThr((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case POW_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getPow();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setPow((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case DEF_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getDef();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setDef((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case STA_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getSta();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setSta((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			case SPD_GROUP:
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getSpd();
				if (	(temp < upper)
					&&	(temp < MAX_STAT))
				{
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setSpd((int)min(temp + UPGRADE_EFFECT, MAX_STAT));
					GameDataRef->GetActiveTeamByIndex(0)->DebitAccount(PartsList[SelectedBodyPart]);
					GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->setWorth(GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getWorth() + PartsList[SelectedBodyPart]);
				}
				break;
			default:
				return false;
			}
		}
		return true;
	}
	return false;
}

// ----
#define	BAR_HEIGHT			8
#define	BAR_WIDTH			65
#define	BAR_SPACING			(3 + BAR_HEIGHT)
#define	BAR_LEFT			11	/* 32 */
#define	BAR_TOP				(FACE_Y + FACE_HEIGHT + 8)
#define	BAR_CAPTION_OFFSET	(0) /* -21 */
#define	BAR_VALUE_OFFSET	(BAR_WIDTH + 2)

// Draws the average stats for the players indexed.
bool	CFrontEnd::DrawStatBars(int	_InitialPlayer, 
								int	_PlayerCount)
{
	int k, l, sum;
	k = _InitialPlayer;
	int teamave = Round(GameDataRef->GetActiveTeamByIndex(0)->GetAverageStats());

	// Protection
	if (	(_InitialPlayer < 0) 
		||	(_InitialPlayer > 11) 
		||	(_PlayerCount < 1)
		||	(_PlayerCount > (12 - _InitialPlayer)))
		return false;

	// i determines which attribute we are summing.
	for (int i = 0; i < 8; i++)
	{
		sum = 0;
		for (l = _PlayerCount; l--;)
		{
			switch (i)
			{
			case INT_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getInt();
				TempString = "Intelligence";
				break;
			case AGR_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getAgr();
				TempString = "Agression";
				break;
			case ATT_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getAtt();
				TempString = "Attack";
				break;
			case THR_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getThr();
				TempString = "Throwing";
				break;
			case POW_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getPow();
				TempString = "Power";
				break;
			case DEF_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getDef();
				TempString = "Defence";
				break;
			case STA_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getSta();
				TempString = "Stamina";
				break;
			case SPD_GROUP:
				sum += GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(k + l)->getSpd();
				TempString = "Speed";
				break;
			default:
				return false;
			}
		}
		sum /= _PlayerCount;

		// Now draw the bars
		RECT Region;
		Region.top		= BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING) + BAR_HEIGHT;
		Region.bottom	= Region.top + BAR_HEIGHT;
		Region.left		= BAR_LEFT;
		Region.right	= Region.left + ((BAR_WIDTH * sum) / MAX_STAT);
		GraphicsObjectRef->DrawRect(&Region, 216, 144, 108);

		Region.left		= Region.right;
		Region.right	= BAR_LEFT + min((BAR_WIDTH * (teamave + UPGRADE_BUFFER)) / MAX_STAT, BAR_WIDTH);
		if (Region.right > Region.left)
		{
			GraphicsObjectRef->DrawRect(&Region, 144, 72, 72);
			Region.left		= Region.right;
		}

		Region.right	= BAR_LEFT + BAR_WIDTH;
		GraphicsObjectRef->DrawRect(&Region, 180, 180, 216);

		// Draw bar caption
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										BAR_LEFT + BAR_CAPTION_OFFSET,
										BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING));

		char convert[24];
		sprintf(convert, "%03d", sum);
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										BAR_LEFT + BAR_VALUE_OFFSET,
										BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING) + BAR_HEIGHT);

	}

	return true;
}

// ----
// Draws bars for a star player
bool	CFrontEnd::DrawStarPlayerStatBars(int	_StarPlayer)
{
	// Protection
	if (	(_StarPlayer < 0) 
		||	(_StarPlayer >= NUM_OF_STAR_PLAYERS))
		return false;

	CTeamMember*	TMRef = GameDataRef->GetStarPlayer(_StarPlayer);
	if (!TMRef)
		return false;

	// i determines which attribute we are summing.
	for (int i = 0; i < 8; i++)
	{
		int sum = 0;
		int teamave = Round(GameDataRef->GetActiveTeamByIndex(0)->GetAverageStats());

		switch (i)
		{
		case INT_GROUP:
			sum += TMRef->getInt();
			TempString = "Intelligence";
			break;

		case AGR_GROUP:
			sum += TMRef->getAgr();
			TempString = "Agression";
			break;
		case ATT_GROUP:
			sum += TMRef->getAtt();
			TempString = "Attack";
			break;
		case THR_GROUP:
			sum += TMRef->getThr();
			TempString = "Throwing";
			break;
		case POW_GROUP:
			sum += TMRef->getPow();
			TempString = "Power";
			break;
		case DEF_GROUP:
			sum += TMRef->getDef();
			TempString = "Defence";
			break;
		case STA_GROUP:
			sum += TMRef->getSta();
			TempString = "Stamina";
			break;
		case SPD_GROUP:
			sum += TMRef->getSpd();
			TempString = "Speed";
			break;
		default:
			return false;
		}

		// Now draw the bars
		RECT Region;
		Region.top		= BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING) + BAR_HEIGHT;
		Region.bottom	= Region.top + BAR_HEIGHT;
		Region.left		= BAR_LEFT;
		Region.right	= Region.left + ((BAR_WIDTH * sum) / MAX_STAT);
		GraphicsObjectRef->DrawRect(&Region, 216, 144, 108);

		Region.left		= Region.right;
		Region.right	= BAR_LEFT + min((BAR_WIDTH * (teamave + UPGRADE_BUFFER)) / MAX_STAT, BAR_WIDTH);
		if (Region.right > Region.left)
		{
			GraphicsObjectRef->DrawRect(&Region, 144, 72, 72);
			Region.left		= Region.right;
		}
		
		Region.right	= BAR_LEFT + BAR_WIDTH;
		GraphicsObjectRef->DrawRect(&Region, 180, 180, 216);

		// Draw bar caption
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										BAR_LEFT + BAR_CAPTION_OFFSET,
										BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING));

		char convert[24];
		sprintf(convert, "%03d", sum);
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										BAR_LEFT + BAR_VALUE_OFFSET,
										BAR_TOP + i * (BAR_HEIGHT + BAR_SPACING) + BAR_HEIGHT);
	}

	return true;
}


// ----
bool	CFrontEnd::Transfer()
{
	// Do we have a problem?
	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef)
		||	(!GameDataRef)
		||	(!GameDataRef->GetActiveTeamByIndex(0)))
		return false;
	
	if (!ManagerGraphic)
	{
		NEW(ManagerGraphic, CGraphicsData());
		SAFELY_CONTROL(if, ManagerGraphic, Init(GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), PlatformRef), ==, false)
			return false;
	}

	// Quit game
	if (ControllerRef->IsButtonPressed(Backup))
	{
		PLAY_BACKUP;
		SetState(EFES_ConfirmAbandonCurrent);
		return true;
	}
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i, j;//, k;
	int		MouseAt = -1;
	RECT Region;
		
	if (!DialogInitialised)
	{
		GraphicsObjectRef->ClearScreen(0,0,0);
		RedrawCountDown = 0;
/*		
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, ManagerGraphic, all));
		
		// Keypad definition.
		for (int xkey = MAX_XKEYS; xkey--;)
		{
			for (int ykey = MAX_YKEYS; ykey--;)
			{
				KeyMatrix[xkey][ykey] = BLANK_KEY;
			}
		}

		// Top row.
		KeyMatrix[0][0]			= BLANK_KEY;
		KeyMatrix[1][0]			= BLANK_KEY;				
		KeyMatrix[2][0]			= ESC_KEY;
		KeyMatrix[3][0]			= UP_KEY;
								
		// Middle row.			
		KeyMatrix[0][1]			= BLANK_KEY;
		KeyMatrix[1][1]			= BLANK_KEY;
		KeyMatrix[2][1]			= FIX_KEY;
		KeyMatrix[3][1]			= DOWN_KEY;
								
		// Bottom row.			
		KeyMatrix[0][2]			= BLANK_KEY;
		// Blank			
		// Blank			
		KeyMatrix[3][2]			= BUY_KEY;

		// Select matrix key 2, 1
		CurrentMenuPos			= 2 + (MAX_XKEYS * 1);	// Fix key.
		SelectedPlayer			= 0;	// Player on the left
		SelectedFace			= 0;	// Player on the right
		FaceOffset				= 0;	// For rendering purposes

		// Get the first star player up for sale
		SelectedPlayer			= GameDataRef->GetNextStarPlayerIndex(-1);

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		GraphicsObjectRef->ClearScreen(0,0,0);
/*
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MANAGER_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, ManagerGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// Navigation
	IfUpDB					// Macro
	{
		switch (CurrentMenuPos)
		{
		// Top row
		case 0:
			CurrentMenuPos = 0 + MAX_XKEYS * 2;
			break;
		case 1:
			CurrentMenuPos = 1 + MAX_XKEYS * 1;
			break;
		case 2:
			CurrentMenuPos = 2 + MAX_XKEYS * 1;
			break;
		case 3:
			CurrentMenuPos = 3 + MAX_XKEYS * 2;
			break;

		// Second row
		case 4:
		case 5:
		case 6:
		case 7:
		// Third
		case 8:
		case 11:
			CurrentMenuPos -= MAX_XKEYS;
			break;

		
		// Navigate players
		case 12:
			//CurrentMenuPos = 23;
			break;
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			CurrentMenuPos--;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfDownDB
	{
		switch (CurrentMenuPos)
		{
		// Top row
		case 0:
		case 1:
		case 2:
		case 3:
		// Second row
		case 4:
		case 7:
			CurrentMenuPos += MAX_XKEYS;
			break;

		case 5:
			CurrentMenuPos = 1;
			break;
		case 6:
			CurrentMenuPos = 2;
			break;
		case 8:
			CurrentMenuPos = 0;
			break;
		case 11:
			CurrentMenuPos = 3;
			break;

		// Navigate the players
		case 12:			// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			CurrentMenuPos++;
			break;
		case 23:
			//CurrentMenuPos = MAX_XKEYS * MAX_YKEYS;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfLeftDB
	{
		switch (CurrentMenuPos)
		{
		// Left col
		case 0:
		case 4:
		case 8:
			CurrentMenuPos += MAX_XKEYS - 1;
			break;

		case 1:
		case 2:
		case 3:
		case 5:
		case 6:
		case 7:
			CurrentMenuPos--;
			break;
		
		case 11:
			CurrentMenuPos = 8;
			break;

		// Navigate off player
		case 12:	// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			CurrentMenuPos = MAX_XKEYS - 1;
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfRightDB
	{
		switch (CurrentMenuPos)
		{
		// Right col
		case 3:
		case 7:
		case 11:
			CurrentMenuPos = MAX_XKEYS * MAX_YKEYS + SelectedFace;
			break;

		case 0:
		case 1:
		case 2:
		case 4:
		case 5:
		case 6:
			CurrentMenuPos++;
			break;
		
		case 8:
			CurrentMenuPos = 11;
			break;

		case 12:	// MAX_XKEYS * MAX_YKEYS
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			break;

		// Keys shouldn't be able to take any other values.
		default:
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}


	// RENDER PLAYERS HERE. DO MOUSE SELECTION STUFF TOO!!!
	
	// Draw background
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_PLAYER_AREA_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 20, 0);


	// Do mouse scroll up 
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_SCROLL_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 10, 100, 10);
	if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
		&&	(IsPointInRect(GameWindowRef->GetMousePoint(), &Region)))
	{
		PLAY_SELECT;
		GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);

		if (SelectedFace > 0)
			SelectedFace--;

		MouseAt = PART_NAV + SelectedFace;
	}

	Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_PLAYER_AREA_HEIGHT - MANAGER_SCROLL_HEIGHT;
	Region.bottom	= Region.top + MANAGER_SCROLL_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 10, 100, 10);
	if (	(ControllerRef->IsButtonPressedDB(MouseSelect))
		&&	(IsPointInRect(GameWindowRef->GetMousePoint(), &Region)))
	{
		PLAY_SELECT;
		GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);

		if (SelectedFace < 11)
			SelectedFace++;

		MouseAt = PART_NAV + SelectedFace;
	}

	// Do direct mouse detect bit
	for (i = 0; i < 4; i++)
	{
		Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE;
		Region.bottom	= Region.top + FACE_SELECTED_SPACE;
		Region.left		= MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET;
		Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;

		if (IsPointInRect(GameWindowRef->GetMousePoint(), &Region))
		{
			if (CurrentMenuPos != PART_NAV + FaceOffset + i)
			{
				PLAY_SELECT;
				MouseAt = PART_NAV + FaceOffset + i;
			}
			break;
		}
	}

	// Detect mouse pointer over keypad.
	for (i = MAX_XKEYS; i--;)
	{
		for (j = MAX_YKEYS; j--;)
		{
			if (j == (MAX_YKEYS - 1))
			{
				if (	(i == 1) 
					||	(i == 2))
					continue;	// No keys here!!! (The bank balance is shown here).
			}

			// check mouse pointer location against key area.
			POINT	*MP = GameWindowRef->GetMousePoint();
			if (	(MP) 
				&&	(MP->x >= (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i))
				&& 	(MP->x <  (KEY_MATRIX_X_OFFSET + X_KEY_SPACING * (i + 1)))
				&& 	(MP->y >= ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j))
				&& 	(MP->y <  ((int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * (j + 1)))
				)
				MouseAt = i + j * MAX_XKEYS;
		}
	}

	// Update mouse if it has been useful.
	if (MouseAt >= 0)
	{
		CurrentMenuPos = MouseAt;
	}

	// All input has been takeinto account - now relate new position to rendering.
	if (CurrentMenuPos >= PART_NAV)
		SelectedFace = CurrentMenuPos - PART_NAV;

	if (SelectedFace < FaceOffset)
		FaceOffset = SelectedFace;

	if (SelectedFace >= (FaceOffset + 4))
		FaceOffset = SelectedFace - 3;
	
	for (i = 0; i < 4; i++)
	{
		if ((FaceOffset + i) == SelectedFace)
		{
			Region.top		= MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE;
			Region.bottom	= Region.top + FACE_SELECTED_SPACE;
			Region.left		= MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET;
			Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
			GraphicsObjectRef->DrawRect(&Region, 50, 200, 50);
		}

		CTeamMember*	TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(FaceOffset + i);

		// Faces
		int Face = TMRef->GetPictureNumber();
		Region.top		= (Face / FACE_PITCH) * FACE_HEIGHT;
		Region.bottom	= Region.top + FACE_HEIGHT;
		Region.left		= (Face % FACE_PITCH) * FACE_WIDTH;
		Region.right	= Region.left + FACE_WIDTH;
		GraphicsObjectRef->DrawSprite(	MANAGER_PLAYER_AREA_LEFT + MANAGER_FACE_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + 1,
										FacesGraphic,
										Region);

		// Position
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&PositionCaptionArray[TMRef->getPosition()],
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 3 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

		// Worth
		char convert[24];
		sprintf(convert, "%04d", TMRef->getWorth());
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&TempString,
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 2 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

		// Name
		GraphicsObjectRef->DrawString(	SmallLightFont,
										TMRef->getName(),
										MANAGER_PLAYER_AREA_LEFT + MANAGER_PLAYER_AREA_WIDTH + MANAGER_FACE_POSITON_LABEL_X_OFFSET,
										MANAGER_PLAYER_AREA_TOP + MANAGER_FACE_Y_OFFSET + i * FACE_SPACE + FACE_HEIGHT - 1 * (SmallLightFont->GetLineHeight() + 1),
										TEXT_RIGHT);

	}

	// Draw frame
	Region.top		= MANAGER_PLAYER_AREA_TOP;
	Region.bottom	= Region.top + MANAGER_PLAYER_AREA_HEIGHT;
	Region.left		= MANAGER_PLAYER_AREA_LEFT;
	Region.right	= Region.left + MANAGER_PLAYER_AREA_WIDTH;
	GraphicsObjectRef->DrawSprite(	MANAGER_PLAYER_AREA_LEFT,
									MANAGER_PLAYER_AREA_TOP,
									ManagerGraphic,
									Region);
	
	// ----
	// Draw keys
	for (i = MAX_XKEYS; i--;)
	{
		for (j = MAX_YKEYS; j--;)
		{
			int		Key;

			if (j == (MAX_YKEYS - 1))
			{
				if (	(i == 1) 
					||	(i == 2))
					continue;	// No keys here!!! (The bank balance is shown here).
			}

			Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
			Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
			Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
			Region.right	= Region.left + KEY_LIGHT_WIDTH;
			GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

			Key = KeyMatrix[i][j];

			if (Key < 0)
				Key = BLANK_KEY;	
				
			if (	(Key != BLANK_KEY) 
				&&	(DialogInitialised)
				&&	(i == (CurrentMenuPos % MAX_XKEYS))
				&&	(j == (CurrentMenuPos / MAX_XKEYS)))
			{
				if (	ControllerRef->IsButtonPressed(Select)
					||	(	(MouseAt != -1) 
						&&	(ControllerRef->IsButtonPressed(MouseSelect))
						)
					)
				{
					// Now check the debounce status.
					if (	ControllerRef->IsButtonPressedDB(Select)
						||	(	(MouseAt != -1) 
							&&	(ControllerRef->IsButtonPressedDB(MouseSelect))
							)
						)
					{
						switch (Key)
						{
						// ESC
						case ESC_KEY:
							PLAY_SELECT;	// Play select sample
						
							switch (FrontEndState)
							{
							case EFES_Single_League_New_Transfer:
								SetState(EFES_Single_League_New_Manager);
								break;
							case EFES_Single_League_TeamManager_Transfer:
								SetState(EFES_Single_League_TeamManager_Manager);
								break;
							case EFES_Single_Cup_New_Transfer:
								SetState(EFES_Single_Cup_New_Manager);
								break;
							default:
								NODEFAULT;
								break;
							}
							break;
						
						// UP
						case UP_KEY:
							PLAY_SELECT;	// Play select sample
							SelectedPlayer = GameDataRef->GetNextStarPlayerIndex(SelectedPlayer);
							if (	(SelectedPlayer < 0)
								||	(SelectedPlayer > NUM_OF_STAR_PLAYERS))
								SelectedPlayer = -1;
							break;

						// ----
						// NEXT ROW
					

						// FIXTURES
						case FIX_KEY:
							PLAY_SELECT;	// Play select sample
							
							switch (FrontEndState)	
							{
							case EFES_Single_Cup_New_Transfer:
								SetState(EFES_Single_Cup_New_Transfer_Fixtures);
								break;
							case EFES_Single_League_TeamManager_Transfer:
								SetState(EFES_Single_League_TeamManager_Transfer_Fixtures);
								break;
								
							case EFES_Single_League_New_Transfer:
								SetState(EFES_Single_League_New_Transfer_Fixtures);
								break;
							default:
								SAFELY_CALL(GraphicsObjectRef, EndDraw());
								return false;
								break;
							}
							break;


						// DOWN
						case DOWN_KEY:
							PLAY_SELECT;	// Play select sample
							SelectedPlayer = GameDataRef->GetPreviousStarPlayerIndex(SelectedPlayer);
							if (	(SelectedPlayer < 0)
								||	(SelectedPlayer > NUM_OF_STAR_PLAYERS))
								SelectedPlayer =-1;
							break;

						// ----
						// NEXT ROW

						// BUY
						case BUY_KEY:
							PLAY_SELECT;	// Play select sample
							GameDataRef->Purchase(SelectedPlayer, SelectedFace);
							if (	(GameDataRef->GetNextStarPlayerIndex(SelectedPlayer) >= 0)
								&&	(GameDataRef->GetNextStarPlayerIndex(SelectedPlayer) < NUM_OF_STAR_PLAYERS))
							{
								SelectedPlayer = GameDataRef->GetNextStarPlayerIndex(SelectedPlayer);
							}
							else
								SelectedPlayer =-1;
							break;

						default:
							break;
						}
					}

					Key += LIT_KEY_ADDER;
				}
				else
				{
					// Fill the region of the key with a 
					// 'highlight' colour, which will show 
					// through the transparent part of the 
					// key.
					Region.top		= (int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j + Y_LIGHT_OFFSET;
					Region.bottom	= Region.top + KEY_LIGHT_HEIGHT;
					Region.left		= KEY_MATRIX_X_OFFSET + X_KEY_SPACING * i + X_LIGHT_OFFSET;
					Region.right	= Region.left + KEY_LIGHT_WIDTH;
					GraphicsObjectRef->DrawRect(&Region, 0, 140, 0);
				}
			}

			// Calculate key 
			Region.top		=	(Key / KEY_PITCH) * KEY_HEIGHT;
			Region.bottom	=	Region.top + KEY_HEIGHT;
			Region.left		=	(Key % KEY_PITCH) * KEY_WIDTH;
			Region.right	=	Region.left + KEY_WIDTH;
		
			GraphicsObjectRef->DrawSprite(	KEY_MATRIX_X_OFFSET  + X_KEY_SPACING * i, 
											(int)GraphicsObjectRef->GetDisplay()->cyHeight - KEY_MATRIX_Y_OFFSET + Y_KEY_SPACING * j, 
											KeysGraphic,
											Region);	

		}
	}

	// ----
	// Draw player / group stats, info, etc
	Region.top		= AREA1_TOP;
	Region.bottom	= AREA1_BOTTOM;
	Region.left		= AREA1_LEFT;
	Region.right	= AREA1_RIGHT;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);
	
	int			Face;
	CTeamMember	*TMRef;

	DrawStarPlayerStatBars(SelectedPlayer);
	TMRef = GameDataRef->GetStarPlayer(SelectedPlayer);
	if (TMRef)
	{
		// Faces
		Face = TMRef->GetPictureNumber();
		Region.top		= (Face / FACE_PITCH) * FACE_HEIGHT;
		Region.bottom	= Region.top + FACE_HEIGHT;
		Region.left		= (Face % FACE_PITCH) * FACE_WIDTH;
		Region.right	= Region.left + FACE_WIDTH;
		GraphicsObjectRef->DrawSprite(	FACE_X,
										FACE_Y,
										FacesGraphic, 
										Region);
		
		// Worth
		char	convert[24];
		sprintf(convert, "%04d", TMRef->getWorth());
		TempString = convert;
		GraphicsObjectRef->DrawString(	SmallLightFont,
										&TempString,
										WORTH_TEXT_X,
										WORTH_TEXT_Y,
										TEXT_RIGHT);

		// Player name
		GraphicsObjectRef->DrawString(	SmallLightFont,
										TMRef->getName(),
										NAME_TEXT_X,
										NAME_TEXT_Y,
										TEXT_RIGHT);
	}
	
	// ----
	// Print the balance, after clearing the area.
	Region.top		= BALANCE_TEXT_Y;
	Region.bottom	= Region.top + BALANACE_TEXT_HEIGHT;
	Region.left		= BALANCE_TEXT_X;
	Region.right	= Region.left + BALANACE_TEXT_WIDTH;
	GraphicsObjectRef->DrawRect(&Region, 0, 0, 0);

	char	convert[24];
	sprintf(convert, "%04d", GameDataRef->GetActiveTeamByIndex(0)->GetBalance());
	TempString = convert;
	GraphicsObjectRef->DrawString(	SmallLightFont,
									&TempString,
									BALANCE_TEXT_X + BALANACE_TEXT_WIDTH,	// Notice that the balance is right aligned!!!
									BALANCE_TEXT_Y,
									TEXT_RIGHT);
		
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

// ----
// These are all guesses
#define TABLE_BORDER				USABLE_COMMON_BORDER
#define	TABLE_DIVISION_LABEL_X		(USABLE_LEFT + (USABLE_WIDTH >> 1))
#define	TABLE_TEAM_X				(USABLE_LEFT + TABLE_BORDER)
#define	TABLE_PLAYED_X				(TABLE_TEAM_X + 73)
#define	TABLE_WON_X					(TABLE_TEAM_X + 88)
#define	TABLE_DRAWN_X				(TABLE_TEAM_X + 103)
#define	TABLE_LOST_X				(TABLE_TEAM_X + 118)
#define	TABLE_FOR_X					(TABLE_TEAM_X + 133)
#define	TABLE_AGAINST_X				(TABLE_TEAM_X + 155)
#define	TABLE_POINTS_X				(TABLE_TEAM_X + 177)

#define	TABLE_DIVISION_LABEL_Y		((j == 0)?(USABLE_TOP + TABLE_BORDER):(USABLE_TOP + USABLE_HEIGHT - TABLE_BORDER - 11 * (SmallLightFont->GetLineHeight() + 2)))
#define	TABLE_CAPTION_Y				(TABLE_DIVISION_LABEL_Y + (SmallLightFont->GetLineHeight() + 2) * 2)
#define	TABLE_POINTS_Y				(TABLE_CAPTION_Y + (SmallLightFont->GetLineHeight() + 2) * (i + 1))

#define	TEXT_TABLE_DIVISION_LABEL	"Division"
#define TEXT_TABLE_TEAM_CAPTION		"Team"
#define TEXT_TABLE_PLAYED_CAPTION	"PL"
#define TEXT_TABLE_WON_CAPTION		"WO"
#define TEXT_TABLE_DRAWN_CAPTION	"DR"
#define TEXT_TABLE_LOST_CAPTION		"LO"
#define	TEXT_TABLE_FOR_CAPTION		"FOR"
#define TEXT_TABLE_AGAINST_CAPTION	"AGT"
#define TEXT_TABLE_POINTS_CAPTION	"PTS"

bool	CFrontEnd::Tables()
{
	RECT	Region;
	char	convert[24];
	int		i, j;

	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef))
		return false;

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		// Assign mode based on current division.
		switch (FrontEndState)
		{
		case EFES_Single_League_New_Tables:		
		case EFES_Single_League_TeamManager_Tables:		
//		case EFES_Single_Cup_New_Tables:
		case EFES_Single_League_New_Tables_Then_Test_Win:		
		case EFES_Single_League_TeamManager_Tables_Then_Test_Win:		
//		case EFES_Single_Cup_New_Tables_Then_Test_Win:
			TableMode = (ETableMode)(GameDataRef->GetActiveTeamByIndex(0)->GetDivision());
			break;
		default:
			return false;
			break;
		}

		// Sort tables
		GameDataRef->SortLeagues();

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
/*
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&(CString("Table")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);
										*/

	}

	if(!RedrawCountDown)
	{
		SAFELY_CALL(GraphicsObjectRef, StartDraw());
		GraphicsObjectRef->ClearScreen(0, 0, 0);

		// Draw background image
		Region.left = 0;
		Region.top = 0;
		Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, Region));


		// ----
		// Draw background colour
		Region.top		= USABLE_TOP;
		Region.bottom	= Region.top + USABLE_HEIGHT;
		Region.left		= USABLE_LEFT;
		Region.right	= Region.left + USABLE_WIDTH;
		SAFELY_CALL(GraphicsObjectRef, DrawRect(&Region, 39, 64, 64));

		// Screen title
		TempString = TEXT_MENU_TITLE_TABLES;
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);


		// 
		for (j = 0; j < 2; j++)
		{
			// Draw bars behind tables.
			Region.top		= TABLE_CAPTION_Y -1;
			i = 8;
			Region.bottom	= TABLE_POINTS_Y - 1;
			Region.left		= TABLE_PLAYED_X - 1;
			Region.right	= TABLE_WON_X - 1;
			GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

			Region.left		= TABLE_DRAWN_X - 1;
			Region.right	= TABLE_LOST_X - 1;
			GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

			Region.left		= TABLE_FOR_X - 1;
			Region.right	= TABLE_AGAINST_X - 1;
			GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

			Region.left		= TABLE_POINTS_X - 1;
			TempString = "000";
			Region.right	= Region.left + SmallLightFont->GetStringWidth(&TempString) + 1;
			GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

			// Table label.
			sprintf(convert, "%s %d", TEXT_TABLE_DIVISION_LABEL, j + 1);
			TempString = convert;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_DIVISION_LABEL_X,
											TABLE_DIVISION_LABEL_Y,
											TEXT_CENTRED);

			// Header
			TempString = TEXT_TABLE_TEAM_CAPTION;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_TEAM_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_PLAYED_CAPTION;
			GraphicsObjectRef->DrawString(	ThePopUpFont, 
											&TempString,
											TABLE_PLAYED_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_WON_CAPTION;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_WON_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_DRAWN_CAPTION;
			GraphicsObjectRef->DrawString(	ThePopUpFont, 
											&TempString,
											TABLE_DRAWN_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_LOST_CAPTION;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_LOST_X,
											TABLE_CAPTION_Y);
			
			TempString = TEXT_TABLE_FOR_CAPTION;
			GraphicsObjectRef->DrawString(	ThePopUpFont, 
											&TempString,
											TABLE_FOR_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_AGAINST_CAPTION;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_AGAINST_X,
											TABLE_CAPTION_Y);

			TempString = TEXT_TABLE_POINTS_CAPTION;
			GraphicsObjectRef->DrawString(	ThePopUpFont, 
											&TempString,
											TABLE_POINTS_X,
											TABLE_CAPTION_Y);
												
			
			// Loop through all teams
			for (i = 0; i < NUM_OF_TEAMS_PER_DIVISION; i++)
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												GameDataRef->GetTeamFromSortedDivision(i, j)->GetShortTeamName(),
												TABLE_TEAM_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%02d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetPlayed());
				TempString = convert;
				if (TempString.GetStringLength() > 2)
					TempString = "--";
				GraphicsObjectRef->DrawString(	ThePopUpFont, 
												&TempString,
												TABLE_PLAYED_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%02d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetWon());
				TempString = convert;
				if (TempString.GetStringLength() > 2)
					TempString = "--";
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												TABLE_WON_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%02d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetDrawn());
				TempString = convert;
				if (TempString.GetStringLength() > 2)
					TempString = "--";
				GraphicsObjectRef->DrawString(	ThePopUpFont, 
												&TempString,
												TABLE_DRAWN_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%02d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetLost());
				TempString = convert;
				if (TempString.GetStringLength() > 2)
					TempString = "--";
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												TABLE_LOST_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%03d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetFor());
				TempString = convert;
				if (TempString.GetStringLength() > 3)
					TempString = "---";
				GraphicsObjectRef->DrawString(	ThePopUpFont, 
												&TempString,
												TABLE_FOR_X,
												TABLE_POINTS_Y);

				sprintf(convert, "%03d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetAgainst());
				TempString = convert;
				if (TempString.GetStringLength() > 3)
					TempString = "---";
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												TABLE_AGAINST_X,
												TABLE_POINTS_Y);
				
				sprintf(convert, "%03d", GameDataRef->GetTeamFromSortedDivision(i, j)->GetPoints());
				TempString = convert;
				if (TempString.GetStringLength() > 3)
					TempString = "---";
				GraphicsObjectRef->DrawString(	ThePopUpFont, 
												&TempString,
												TABLE_POINTS_X,
												TABLE_POINTS_Y);
			}
		}

		// Centre bar
		i = 14;
		Region.top		= FIXTURES_TEAM_A_ATTRIBUTE_Y_POS;
		Region.bottom	= FIXTURES_TEAM_B_NAME_Y_POS - SmallLightFont->GetLineHeight() * 2;
		Region.left		= USABLE_LEFT + TABLE_BORDER;
		Region.right	= USABLE_LEFT + USABLE_WIDTH - TABLE_BORDER;
		GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);


		SAFELY_CALL(GraphicsObjectRef, EndDraw());

	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;


	// Check for select button / mouse tap
	if (	(ControllerRef->IsButtonPressedDB(Select))
		||	(ControllerRef->IsButtonPressedDB(MouseSelect)))
	{
		// Set state depending on how we got here.
		switch (FrontEndState)
		{
		// League - New
		case EFES_Single_League_New_Tables:		
			SetState(EFES_Single_League_New_Manager);
			break;
		case EFES_Single_League_New_Tables_Then_Test_Win:

			// What can happen now...
			// 1. If the league isn't over then go straight to the manager screen.
			// 2. The league has JUST finished and is in one of the following states.
			//	a. Human has won entire league.			-> Won			-> Main
			//	b. Human has won promotion.				-> Promotion	-> New Season
			//	c. Human has been relegated				-> Relegation	-> New Season
			//	d. Human has to playoff for promotion	-> Playoff		-> Manager 
			//	e. Human has to playoff for relegation	-> Playoff		-> Manager
			//	f. Human is mid-division				-> New Season

			// So, check whether the league is over...
			if (!GameDataRef->IsLeagueOver())
			{
				// 1. No, straight to manager screen.
				SetState(EFES_Single_League_New_Manager);
				break;
			}
			else
			{		
				// 2a - Quick escape if human won the cup.
				if (GameDataRef->DidHumanWinLeague())
				{
					SetState(EFES_Single_League_New_Won);
					break;
				}

				// 2b-f.
				// Right, assuming that the league has just
				// finished we can now act on the info and 
				// see if we need to do a human playoff
				GameDataRef->PrepareForLeaguePlayOff();			// The divison data is all shot up after here!!!

				// Determine whether there is a playoff (involving the human).
				if (GameDataRef->IsHumanInLeaguePlayOff())
				{
					// Yes, so doing either 'd' or 'e'.
					GameDataRef->AssignPlayOffTeams();
					SetState(EFES_Single_League_New_PlayOff);
					break;
				}
				else
				{
					// No. Must be processing b, c or f.
					
					// Collect data before modifying the division data.
					bool GetRel = GameDataRef->DidHumanGetRelegated();
					bool GetPro = GameDataRef->DidHumanGetPromotion();

					// Resolve promotions / relegations.
					GameDataRef->ResolveCPUOnlyPlayOff();

					// 2c - Relegation.
					if (GetRel)
					{
						SetState(EFES_Single_League_New_Relegate);
						break;
					}

					// 2b - Promotion.
					if (GetPro)
					{
						SetState(EFES_Single_League_New_Promotion);
						break;
					}

					// 2f - Mid table. New season.
					SetState(EFES_Single_League_New_End_Season);
				}
			}	
			break;
			

		// League - Team manager
		case EFES_Single_League_TeamManager_Tables:		
			SetState(EFES_Single_League_TeamManager_Manager);
			break;
		case EFES_Single_League_TeamManager_Tables_Then_Test_Win:
			
			// What can happen now...
			// 1. If the league isn't over then go straight to the manager screen.
			// 2. The league has JUST finished and is in one of the following states.
			//	a. Human has won entire league.			-> Won			-> Main
			//	b. Human has won promotion.				-> Promotion	-> New Season
			//	c. Human has been relegated				-> Relegation	-> New Season
			//	d. Human has to playoff for promotion	-> Playoff		-> Manager 
			//	e. Human has to playoff for relegation	-> Playoff		-> Manager
			//	f. Human is mid-division				-> New Season

			// So, check whether the league is over...
			if (!GameDataRef->IsLeagueOver())
			{
				// 1. No, straight to manager screen.
				SetState(EFES_Single_League_TeamManager_Manager);
				break;
			}
			else
			{		
				// 2a - Quick escape if human won the cup.
				if (GameDataRef->DidHumanWinLeague())
				{
					SetState(EFES_Single_League_TeamManager_Won);
					break;
				}

				// 2b-f.
				// Right, assuming that the league has just
				// finished we can now act on the info and 
				// see if we need to do a human playoff
				GameDataRef->PrepareForLeaguePlayOff();

				// Determine whether there is a playoff (involving the human).
				if (GameDataRef->IsHumanInLeaguePlayOff())
				{
					// Yes, so doing either 'd' or 'e'.
					GameDataRef->AssignPlayOffTeams();
					SetState(EFES_Single_League_TeamManager_PlayOff);
					break;
				}
				else
				{
					// No. Must be processing b, c or f.
					
					// Collect data before modifying the division data.
					bool GetRel = GameDataRef->DidHumanGetRelegated();
					bool GetPro = GameDataRef->DidHumanGetPromotion();

					// Resolve promotions / relegations.
					GameDataRef->ResolveCPUOnlyPlayOff();

					// 2c - Relegation.
					if (GetRel)
					{
						SetState(EFES_Single_League_TeamManager_Relegate);
						break;
					}

					// 2b - Promotion.
					if (GetPro)
					{
						SetState(EFES_Single_League_TeamManager_Promotion);
						break;
					}

					// 2f - Mid table. New season.
					SetState(EFES_Single_League_TeamManager_End_Season);
				}
			}
			break;

		default:
			NODEFAULT;
			break;
		}
	}
	
	return true;
}

// ----
#define RESULTS_BORDER					USABLE_COMMON_BORDER
#define	RESULTS_DIVISION_LABEL_X		(USABLE_LEFT + (USABLE_WIDTH >> 1))
#define	RESULTS_TEAM_A_X				(USABLE_LEFT + RESULTS_BORDER)
#define	RESULTS_TEAM_B_X				(USABLE_LEFT + (USABLE_WIDTH >> 1))
#define	RESULTS_SCORE_OFFSET			95
#define	RESULTS_SCORE_A_X				(RESULTS_TEAM_A_X + RESULTS_SCORE_OFFSET)
#define	RESULTS_SCORE_B_X				(RESULTS_TEAM_B_X + RESULTS_SCORE_OFFSET)

#define	RESULTS_DIVISION_LABEL_Y		((j == 0)?(USABLE_TOP + RESULTS_BORDER):(USABLE_TOP + USABLE_HEIGHT - RESULTS_BORDER - 11 * (SmallLightFont->GetLineHeight() + 2)))
//#define	RESULTS_CAPTION_Y				(RESULTS_DIVISION_LABEL_Y + (SmallLightFont->GetLineHeight() + 2) * 2)
#define	RESULTS_POINTS_Y				(RESULTS_DIVISION_LABEL_Y + (SmallLightFont->GetLineHeight() + 2) * (i + 2))

#define	TEXT_RESULTS_DIVISION_LABEL		"Division"

// Results screen
bool	CFrontEnd::Results()
{
	RECT	Region;
	char	convert[24];
	int		i, j;

	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef))
		return false;

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		GraphicsObjectRef->ClearScreen(0, 0, 0);

		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
/*
		// Draw background image
		Region.left = 0;
		Region.top = 0;
		Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, Region));
	*/	
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		SAFELY_CALL(GraphicsObjectRef, StartDraw());
		GraphicsObjectRef->ClearScreen(0, 0, 0);

		// Draw background image
		Region.left = 0;
		Region.top = 0;
		Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, Region));


		// ----
		// Draw background colour
		Region.top		= USABLE_TOP;
		Region.bottom	= Region.top + USABLE_HEIGHT;
		Region.left		= USABLE_LEFT;
		Region.right	= Region.left + USABLE_WIDTH;
		SAFELY_CALL(GraphicsObjectRef, DrawRect(&Region, 39, 64, 64));

		// Screen title
		TempString = TEXT_MENU_TITLE_RESULTS;
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);


		// 
		for (j = 0; j < 2; j++)
		{
			// Table label.
			sprintf(convert, "%s %d", TEXT_RESULTS_DIVISION_LABEL, j + 1);
			TempString = convert;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											TABLE_DIVISION_LABEL_X,
											TABLE_DIVISION_LABEL_Y,
											TEXT_CENTRED);
										
			
			// Loop through all teams
			for (i = 0; i < (NUM_OF_TEAMS_PER_DIVISION / 2); i++)
			{
				IntCouple PlayingTeams;
				GameDataRef->GetFixture(&PlayingTeams, i);
				int TeamA	= PlayingTeams.A;
				int TeamB	= PlayingTeams.B;

				// is this right?
				if (	(TeamA < 0) 
					||	(TeamA > 7) 
					||	(TeamB < 0)
					||	(TeamB > 7))
					OutputDebugString(TEXT("Team index out of bounds!!!!\n"));


				// Team A
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												GameDataRef->GetTeamFromDivision(TeamA, j)->GetShortTeamName(),
												RESULTS_TEAM_A_X,
												RESULTS_POINTS_Y);

				// Team B
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												GameDataRef->GetTeamFromDivision(TeamB, j)->GetShortTeamName(),
												RESULTS_TEAM_B_X,
												RESULTS_POINTS_Y);


				// Score A
				sprintf(convert, "%03d", GameDataRef->GetTeamFromDivision(TeamA, j)->Score);
				TempString = convert;
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												RESULTS_SCORE_A_X,
												RESULTS_POINTS_Y,
												TEXT_RIGHT);

				// Score B
				sprintf(convert, "%03d", GameDataRef->GetTeamFromDivision(TeamB, j)->Score);
				TempString = convert;
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												RESULTS_SCORE_B_X,
												RESULTS_POINTS_Y,
												TEXT_RIGHT);
			}
		}

		// Centre bar
		i = 14;
		Region.top		= FIXTURES_TEAM_A_ATTRIBUTE_Y_POS;
		Region.bottom	= FIXTURES_TEAM_B_NAME_Y_POS - SmallLightFont->GetLineHeight() * 2;
		Region.left		= USABLE_LEFT + TABLE_BORDER;
		Region.right	= USABLE_LEFT + USABLE_WIDTH - TABLE_BORDER;
		GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

		// Finish drawing
		SAFELY_CALL(GraphicsObjectRef, EndDraw());

	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	

	// ----
	// Check for select button / mouse tap
	if (	(ControllerRef->IsButtonPressedDB(Select))
		||	(ControllerRef->IsButtonPressedDB(MouseSelect)))
	{
		// Determine which players are up for sale
		GameDataRef->ProcessStarPlayers();

		// Set state depending on how we got here.
		switch (FrontEndState)
		{
		case EFES_Single_League_New_Results:
			// Update league standings, and choose new opponent before going to tables.
			GameDataRef->UpdateLeagueState();
			SetState(EFES_Single_League_New_Tables_Then_Test_Win);
			break;

		case EFES_Single_League_TeamManager_Results:	
			// Update league standings, and choose new opponent before going to tables.
			GameDataRef->UpdateLeagueState();
			SetState(EFES_Single_League_TeamManager_Tables_Then_Test_Win);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	return true;
}

// ----
// Cheat menu
bool	CFrontEnd::Register()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	bool    AllSelectChanged	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!
	CString	Temp;

	#define	REGMESSAGELINES		11

	CString		*RegMessage[REGMESSAGELINES];
	RegMessage[0] = new CString("The gameplay in this demo version");
	RegMessage[1] = new CString("of Speedball2 is time limited. ");
	RegMessage[2] = new CString("To register Speedball2 and ");
	RegMessage[3] = new CString("experience the full game visit ");
	RegMessage[4] = new CString("www.portable-games.com");
	RegMessage[5] = new CString(" ");
	RegMessage[6] = new CString("Your username is:");
	RegMessage[7] = new CString(" ");
	RegMessage[8] = new CString(" ");
	RegMessage[9] = new CString("Enter the key exactly as it");
	RegMessage[10] = new CString("appears above when registering.");

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

#define KEY_SIZE			16
#define KEYBOARD_START_X	(120 - 6 * KEY_SIZE + (KEY_SIZE / 2))
#define KEYBOARD_START_Y	(206-24)

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Register";
		MenuTextArray[1]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;

		GameDataRef->SetRegistrationCode(0);

		// Jump straight to the bad user name screen.
		if (!GetRegistrationString(&Temp))
		{
			SetState(EFES_Register_BadUserName);
		}
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&CString("Registration"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										10,
										TEXT_CENTRED);

		for (int i = 0; i < REGMESSAGELINES; i++)
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											RegMessage[i],
											10, 
											40 + 10 * i,
											TEXT_LEFT);
		}

		GetRegistrationString(&Temp);
		GraphicsObjectRef->DrawString(	RegFont, 
										&Temp,
										120, 
										40 + 10 * 7,
										TEXT_CENTRED);


		/*
		all.left = 0;
		all.top = 0;
		all.right = 176;
		all.bottom = 64;
		*/

		// draw the numbers
		all.left = 0;
		all.top = 0;
		all.right = all.left + 10*16;
		all.bottom = all.top + 16;					
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(KEYBOARD_START_X, KEYBOARD_START_Y, KeyboardGraphic, all));

		all.left = 10 * 16;
		all.top = 0;
		all.right = all.left + 16;
		all.bottom = all.top + 32;		
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(KEYBOARD_START_X + 10 * KEY_SIZE, KEYBOARD_START_Y, KeyboardGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

#define NUM_OF_KEYS		22
	for( int n = 0; n < NUM_OF_KEYS; n++)
	{
		POINT*		_Point;
		bool		_PointIsInDrawRegion = true;

		RECT key;
		key.left	= KEYBOARD_START_X + ( (n%11) * KEY_SIZE);
		key.right	= key.left + KEY_SIZE;
		key.top		= KEYBOARD_START_Y + ((n / 11) * KEY_SIZE);
		key.bottom	= key.top + KEY_SIZE;

		_Point =  GameWindowRef->GetMousePoint();

		if (	(_Point)
		&&	(_PointIsInDrawRegion)
		)
		{
			if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)

				&&	(_Point->x > key.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < key.right) 
				&&	(_Point->y > key.top) 
				&&	(_Point->y < key.bottom) 
				)
				_PointIsInDrawRegion = true;	// Yes.
			else
				_PointIsInDrawRegion = false;	// No, to one of the above.
		}

		if(_PointIsInDrawRegion)
		{
			AllSelect = n;
			AllSelectChanged = true;
		}
	}

CString temp;
	// See if the mouse was pressed on the screen.
	if (	(ControllerRef)									
		&&	(ControllerRef->IsButtonPressedDB(MouseSelect))
		&&	(AllSelectChanged)
		)
	{
		unsigned long Tempcode = GameDataRef->GetRegistrationCode();

		if (	(AllSelect >= 0)
			&&	(AllSelect < 10)
			)
		{
			SOUND_SELECT;
			if (Tempcode < 10000)
			{
				Tempcode *= 10;
				Tempcode += (AllSelect + 1) % 10;
			}
		}
		else if (	(AllSelect == 10) 
				||	(AllSelect == 21) 
				)
		{
			SOUND_SELECT;
			Tempcode /= 10;	// Drop off the last digit!
		}	
		GameDataRef->SetRegistrationCode(Tempcode);
	}

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP + 120;
		region.right = 240;
		region.bottom = region.top + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP + 120, StoryScreenBackground, region));

		// Draw registration code
		region.left = 0;
		region.top = KEYBOARD_START_Y - MENU_ITEM_SPACING - 8;
		region.right = 240;
		region.bottom = region.top + MENU_ITEM_SPACING;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, region.top, StoryScreenBackground, region));

		CString TempStr;
		char	TempChars[32];
		sprintf(TempChars, "%05d", GameDataRef->GetRegistrationCode());
		TempStr = TempChars;
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempStr,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										KEYBOARD_START_Y - MENU_ITEM_SPACING,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + 120 + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SOUND_SELECT;
			if (GetRegistrationString(&Temp))
			{
				// Good name, so lets test the name against the key the user entered.
				if (ValidateRegistrationDetails(&Temp, GameDataRef->GetRegistrationCode(), PRODUCT_ACTIVATION_RPN_STRING))
				{
					// Successfully registered
					GameDataRef->GetPreferences()->Registered = true;

					unsigned long codecrypt = EncryptCode(GameDataRef->GetRegistrationCode());
					SaveToRegistry(	HKEY_LOCAL_MACHINE,							// Existing key or default root.
									&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
									&CString(REG_ACTIVATION_CODE),				// Name of the value to store.
									REG_BINARY,									// Specifies the format for storing the data.
									(char*)&codecrypt,							// A pointer to the data to store.
									sizeof(codecrypt));

					SetState(EFES_Register_ThankYou);
				}
				else
				{
					SetState(EFES_Register_Mistake);
				}
			}
			else
			{
				SetState(EFES_Register_BadUserName);
			}
			break;

		case 1:
			SOUND_SELECT;
			SetState(EFES_Main);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	for (i = REGMESSAGELINES; i--;)
	{
		SAFELY_DELETE_C2248(RegMessage[i]);
	}

	return true;
}


// ----
// Registeration result screens
bool	CFrontEnd::RegisterThankYou()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!
	CString	Temp;

	#define	REGTYMESSAGELINES		5

	CString		*RegMessage[REGTYMESSAGELINES];
	RegMessage[0] = new CString("Thank you for purchasing");
	RegMessage[1] = new CString("Speedball2. ");
	RegMessage[2] = new CString(" ");
	RegMessage[3] = new CString("This product is registered to:");
	RegMessage[4] = new CString();
	GetRegistrationString(RegMessage[4]);
	//RegMessage[4] = new CString("www.portable-games.com");

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Okay";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&CString("Thank you"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										10,
										TEXT_CENTRED);

		for (int i = 0; i < REGTYMESSAGELINES; i++)
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											RegMessage[i],
											10, 
											40 + 10 * i,
											TEXT_LEFT);
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP + 120;
		region.right = 240;
		region.bottom = region.top + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP + 120, StoryScreenBackground, region));


		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + 80 + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + 80 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + 80 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SOUND_SELECT;
			SetState(EFES_Main);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	for (i = REGTYMESSAGELINES; i--;)
	{
		SAFELY_DELETE_C2248(RegMessage[i]);
	}

	return true;
}

bool	CFrontEnd::RegisterMistake()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!
	CString	Temp;

	#define	REGMMESSAGELINES		4

	CString		*RegMessage[REGMMESSAGELINES];
	RegMessage[0] = new CString("Sorry, the product key you entered");
	RegMessage[1] = new CString("does not match your username.");
	RegMessage[2] = new CString("Please try again, or contact ");
	RegMessage[3] = new CString("www.portable-games.com for advice.");

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Okay";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&CString("Registration"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										10,
										TEXT_CENTRED);

		for (int i = 0; i < REGMMESSAGELINES; i++)
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											RegMessage[i],
											10, 
											40 + 10 * i,
											TEXT_LEFT);
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP + 120;
		region.right = 240;
		region.bottom = region.top + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, region.top, StoryScreenBackground, region));


		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + 120 + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SOUND_SELECT;
			SetState(EFES_Register);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	for (i = REGMMESSAGELINES; i--;)
	{
		SAFELY_DELETE_C2248(RegMessage[i]);
	}

	return true;
}

bool	CFrontEnd::RegisterBadUserName()
{

	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!
	CString	Temp;

	#define	REGBUNMESSAGELINES		13

	CString		*RegMessage[REGBUNMESSAGELINES];
	RegMessage[0] = new CString("Sorry, you are unable to register");
	RegMessage[1] = new CString("Speedball2 unless you have a ");
	RegMessage[2] = new CString("valid username.");
	RegMessage[3] = new CString("  ");
	RegMessage[4] = new CString("Unfortunately, either you have");
	RegMessage[5] = new CString("not set a username, or the name");
	RegMessage[6] = new CString("you have set does not contain");
	RegMessage[7] = new CString("sufficient plain characters. ");
	RegMessage[8] = new CString("  ");
	RegMessage[9] = new CString("If you wish to set a username now");
	RegMessage[10] = new CString("select the quit option and go to");
	RegMessage[11] = new CString("Start - Settings - Personal - ");
	RegMessage[12] = new CString("Owner Information.");

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Okay";
		MenuTextArray[1]	= "Quit";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&CString("Registration"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										10,
										TEXT_CENTRED);

		for (int i = 0; i < REGBUNMESSAGELINES; i++)
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											RegMessage[i],
											10, 
											40 + 10 * i,
											TEXT_LEFT);
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP + 120;
		region.right = 240;
		region.bottom = region.top + NumberOfOptions * MENU_ITEM_SPACING;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, region.top, StoryScreenBackground, region));


		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + 120 + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + 120 + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		
			SOUND_SELECT;
			SetState(EFES_Main);
			break;

		case 1:		
			SOUND_SELECT;
			SetState(EFES_ConfirmQuit);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	for (i = REGBUNMESSAGELINES; i--;)
	{
		SAFELY_DELETE_C2248(RegMessage[i]);
	}

	return true;
}


// ----
// Cheat menu
bool	CFrontEnd::Cheat()
{
	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}

		MenuTextArray[0]	= "Show cup";
		MenuTextArray[1]	= "Back";

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		if (GraphicsObjectRef)
		{
			// Clear screen
			SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				SAFELY_CALL(GraphicsObjectRef, EndDraw());
				return false;
			}
*/
			RECT all;
			all.left = 0;
			all.top = 0;
			all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

			SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);


	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString("Cheat"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SetState(EFES_Single_KnockOut_Won);
		case 1:
			SetState(EFES_Main);
			break;
		default:
			NODEFAULT;
			break;
		}
	}
	
	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}


bool	CFrontEnd::ComingOutOfGame()
{
	int PrevDiv;

	if (!GameDataRef)
		return false;

	if (!DialogInitialised)
	{
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}
	else
	{
		if (GetTimeDifference(&TimeNow, &DialogStartTime) > 3.0f)
		{
			switch (FrontEndState)
			{
			case EFES_Single_KnockOut_ComingOutOfGame:
				// Score the game.
				GameDataRef->KnockOutScore();

				if ((GameDataRef->GetActiveTeamByIndex(0)->Score) <	
					(GameDataRef->GetActiveTeamByIndex(1)->Score))
				{ 
					// Lost game, so back to main menu
					SetState(EFES_Single_KnockOut_GameOver); 
				}
				else 
				if ((GameDataRef->GetActiveTeamByIndex(0)->Score) >
					(GameDataRef->GetActiveTeamByIndex(1)->Score))
				{
					SetState(EFES_Single_KnockOut_Fixtures);

					if (!GameDataRef->UpdateKnockOut())
						SetState(EFES_Single_KnockOut_Won);
				}
				else
				{
					// Drawn game. Do nothing! (Continue playing against this team until there is a winner / loser!)
					SetState(EFES_Single_KnockOut_Fixtures);
				}
				return true;

			case EFES_Single_League_New_ComingOutOfGame:
				// Determine if this is a playoff
				if (!GameDataRef->IsHumanInLeaguePlayOff())
				{
					// No. Simple go straight tot he results screen.
					SetState(EFES_Single_League_New_Results);
				}
				else
				{
					// Right. What happens now.
					// 1. The teams are drawn		Playoff		-> Manager
					// 2. There is a winner...
					//	a. Human is promoted		Promotion	-> New Season
					//	b. Human is relegated		Relegation	-> New Season
					//	c. No change				New Season.

					// Store human's division number now.
					PrevDiv = GameDataRef->GetTeamByIndex(0)->GetDivision();

					// 1/2. Resolve the score.
					if (GameDataRef->ResolveHumanPlayOff() == false)
					{
						// 1. Another playoff
						SetState(EFES_Single_League_New_PlayOff);
					}
					else
					{
						// 2. There is a winner.
						if (GameDataRef->GetTeamByIndex(0)->GetDivision() < PrevDiv)
						{
							// 2a. Promotion.
							SetState(EFES_Single_League_New_Promotion);
						}
						else if (GameDataRef->GetTeamByIndex(0)->GetDivision() > PrevDiv)
						{
							// 2b. Relegation.
							SetState(EFES_Single_League_New_Relegate);
						}
						else
						{
							// 2c. No change.
							SetState(EFES_Single_League_New_End_Season);
						}
					}
				}
				return true;

			case EFES_Single_League_TeamManager_ComingOutOfGame:
				// Determine if this is a playoff
				if (!GameDataRef->IsHumanInLeaguePlayOff())
				{
					// No. Simple go straight tot he results screen.
					SetState(EFES_Single_League_TeamManager_Results);
				}
				else
				{
					// Right. What happens now.
					// 1. The teams are drawn		Playoff		-> Manager
					// 2. There is a winner...
					//	a. Human is promoted		Promotion	-> New Season
					//	b. Human is relegated		Relegation	-> New Season
					//	c. No change				New Season.

					// Store human's division number now.
					PrevDiv = GameDataRef->GetTeamByIndex(0)->GetDivision();

					// 1/2. Resolve the score.
					if (GameDataRef->ResolveHumanPlayOff() == false)
					{
						// 1. Another playoff
						SetState(EFES_Single_League_TeamManager_PlayOff);
					}
					else
					{
						// 2. There is a winner.
						if (GameDataRef->GetTeamByIndex(0)->GetDivision() < PrevDiv)
						{
							// 2a. Promotion.
							SetState(EFES_Single_League_TeamManager_Promotion);
						}
						else if (GameDataRef->GetTeamByIndex(0)->GetDivision() > PrevDiv)
						{
							// 2b. Relegation.
							SetState(EFES_Single_League_TeamManager_Relegate);
						}
						else
						{
							// 2c. No change.
							SetState(EFES_Single_League_TeamManager_End_Season);
						}
					}
				}
				return true;

			case EFES_Single_Cup_New_ComingOutOfGame:
				SetState(EFES_Single_Cup_New_Results);
				return true;

			default:
				return false;
			}
		}
	}
	return true;
}

// ----
// Render won screen
#define	WON_HANDS_LEFT			0
#define	WON_HANDS_TOP			0
#define	WON_HANDS_WIDTH			160
#define	WON_HANDS_HEIGHT		176

#define	WON_BALL_WIDTH			80
#define	WON_BALL_HEIGHT			112

#define	WON_HAND_TO_BALL_OFFSET	32

#define	WON_TIMEOUT				(10.0f)

bool	CFrontEnd::Won()
{
	if (	(!GameDataRef) 
		||	(!GraphicsObjectRef) 
		||	(!ControllerRef))
		return false;

	RECT	Region;

	GraphicsObjectRef->StartDraw();
	
	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(0, 0, 0));
		
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(CUPS_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
		
		
		Region.top		= WON_HANDS_TOP;
		Region.bottom	= Region.top + WON_HANDS_HEIGHT;
		Region.left		= WON_HANDS_LEFT;
		Region.right	= Region.left + WON_HANDS_WIDTH;
		GraphicsObjectRef->DrawSprite(	(GraphicsObjectRef->GetDisplay()->cxWidth - WON_HANDS_WIDTH) >> 1,
										(GraphicsObjectRef->GetDisplay()->cyHeight - WON_HANDS_HEIGHT) >> 1,
										BackgroundGraphic, 
										Region);

		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_Won:
			Region.left		= Region.right + 1 * WON_BALL_WIDTH;
			break;
		case EFES_Single_League_New_Won:
		case EFES_Single_League_TeamManager_Won:
			Region.left		= Region.right + 0 * WON_BALL_WIDTH;
			break;
		case EFES_Single_Cup_Won:
			Region.left		= Region.right + 2 * WON_BALL_WIDTH;
			break;
		default:
			return false;
		}

		Region.bottom	= Region.top + WON_BALL_HEIGHT;
		Region.right	= Region.left + WON_BALL_WIDTH;
		GraphicsObjectRef->DrawSprite(	((GraphicsObjectRef->GetDisplay()->cxWidth - WON_HANDS_WIDTH) >> 1) + WON_HAND_TO_BALL_OFFSET,
										(GraphicsObjectRef->GetDisplay()->cyHeight - WON_HANDS_HEIGHT) >> 1,
										BackgroundGraphic, 
										Region);
		
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}
	
	if(!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(0, 0, 0));
		
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(CUPS_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
		
		
		Region.top		= WON_HANDS_TOP;
		Region.bottom	= Region.top + WON_HANDS_HEIGHT;
		Region.left		= WON_HANDS_LEFT;
		Region.right	= Region.left + WON_HANDS_WIDTH;
		GraphicsObjectRef->DrawSprite(	(GraphicsObjectRef->GetDisplay()->cxWidth - WON_HANDS_WIDTH) >> 1,
										(GraphicsObjectRef->GetDisplay()->cyHeight - WON_HANDS_HEIGHT) >> 1,
										BackgroundGraphic, 
										Region);

		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_Won:
			Region.left		= Region.right + 1 * WON_BALL_WIDTH;
			break;
		case EFES_Single_League_New_Won:
		case EFES_Single_League_TeamManager_Won:
			Region.left		= Region.right + 0 * WON_BALL_WIDTH;
			break;
		case EFES_Single_Cup_Won:
			Region.left		= Region.right + 2 * WON_BALL_WIDTH;
			break;
		default:
			return false;
		}

		Region.bottom	= Region.top + WON_BALL_HEIGHT;
		Region.right	= Region.left + WON_BALL_WIDTH;
		GraphicsObjectRef->DrawSprite(	((GraphicsObjectRef->GetDisplay()->cxWidth - WON_HANDS_WIDTH) >> 1) + WON_HAND_TO_BALL_OFFSET,
										(GraphicsObjectRef->GetDisplay()->cyHeight - WON_HANDS_HEIGHT) >> 1,
										BackgroundGraphic, 
										Region);
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;


	// Now process the fire buttons
	if (	(ControllerRef->IsButtonPressedDB(Select))
		||	(ControllerRef->IsButtonPressedDB(MouseSelect))
		||	(GetTimeDifference(&TimeNow, &DialogStartTime) > WON_TIMEOUT))
	{
		SOUND_SELECT;
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
		SetState(EFES_Credits);	// Go straight into the credits screen.
	}
	
	GraphicsObjectRef->EndDraw();
	return true;
}


bool	CFrontEnd::Options()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

//	int vol = GameDataRef->GetPreferences()->Volume;

	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 5;	// Remember to update this for each method!!!

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		MenuTextArray[0]	= "Audio";
		MenuTextArray[1]	= "Graphics";
		MenuTextArray[2]	= "Controls";
		MenuTextArray[3]	= "Difficulty";
		MenuTextArray[4]	= "Back";
		//MenuTextArray[5]	= "Cancel";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

	// Update volume control (although it is still one frame lagged).

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, BackgroundGraphic, region));

		// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_OPTIONS),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SOUND_SELECT;
			SetState(EFES_Volume);
			break;

		case 1:
			SOUND_SELECT;
			SetState(EFES_Buffer);
			break;

		case 2:
			SOUND_SELECT;
			SetState(EFES_Stylus);
			break;

		case 3:
			SOUND_SELECT;
			SetState(EFES_Difficulty);
			break;

		case 4:
			SOUND_SELECT;
			GameDataRef->SavePreferences();
			SetState(EFES_Main);
			break;

		/*case 5:
			SOUND_SELECT;
			GameDataRef->LoadPreferences(PlatformRef);
			SoundSystemRef->SetVolume(GameDataRef->GetPreferences()->Volume);
			
			SetState(EFES_Main);
			break;
		*/
		default:
			NODEFAULT;
			break;
		}
	}
	
	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

bool		CFrontEnd::Single_Cup_RoundN()
{
//	char	convert[24];
	
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	if (!DialogInitialised)
	{
		RedrawCountDown = 0;
		// If we have won the cup there is no point 
		// showing this screen - jump to Cup screen.
		if (GameDataRef->GetCupState() == ECS_WonCup)
		{
			SetState(EFES_Single_Cup_Won);
			return true;
		}
		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if(!RedrawCountDown)
	{
		GraphicsObjectRef->StartDraw();
		GraphicsObjectRef->ClearScreen(0, 0, 0);
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			GraphicsObjectRef->EndDraw();
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		GraphicsObjectRef->DrawSprite(0, 0, BackgroundGraphic, all);

		// Screen title
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_NROUND),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);

		
		switch (GameDataRef->GetCupState())
		{
		case ECS_Normal:
			if (GameDataRef->GetCupLeg() == 0)
			{
				TempString = "Round ";
				if ((GameDataRef->GetCupRound() + 1) < MAX_NUM_STRING)
					TempString += NumStringArray[GameDataRef->GetCupRound() + 1];
			}
			else
			{
				TempString = "Leg ";
				if ((GameDataRef->GetCupLeg() + 1) < MAX_NUM_STRING)
					TempString += NumStringArray[GameDataRef->GetCupLeg() + 1];
			}
			break;

		case ECS_Drew:
			// Show replay text
			TempString = "Playoff";
			break;

		case ECS_Lost:
			TempString = "Game over";
			break;

		case ECS_WonCup:
			SetState(EFES_Single_Cup_Won);
			break;

		default:
			NODEFAULT;
			break;
		}

		
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&TempString,
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										(GraphicsObjectRef->GetDisplay()->cyHeight - MainMenuFont->GetLineHeight()) >> 1,
										TEXT_CENTRED);

		GraphicsObjectRef->EndDraw();
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	if (	(ControllerRef->IsButtonPressedDB(Select)) 
		||	(ControllerRef->IsButtonPressedDB(MouseSelect))
		||	(GetTimeDifference(&TimeNow, &DialogStartTime) > 5.0f))
	{
		switch (GameDataRef->GetCupState())
		{
		case ECS_Normal:
		case ECS_Drew:
			// Using this state will ensure that the game goes to the manager screen after the CupFixtures screen
			SetState(EFES_Single_Cup_New_Manager_Fixtures);
			break;

		case ECS_Lost:
			SetState(EFES_Main);
			break;

		case ECS_WonCup:
			SetState(EFES_Single_Cup_Won);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	return true;
}

// ----
// Generic message screen.
bool	CFrontEnd::Message()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	int i;
	RECT Region;

	if (	(!DialogInitialised)
		||	(!RedrawCountDown) )
	{
		GraphicsObjectRef->StartDraw();
		GraphicsObjectRef->ClearScreen(0, 0, 0);

		bool	ShowCaptions = true;
		int		ShowNCaps = 0;
		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_GameOver:
			TempString = "Speedball II";
			TempStringArray[0] = "Game Over";
			ShowNCaps = 1;
			break;

		case EFES_Single_League_New_Promotion:
		case EFES_Single_League_TeamManager_Promotion:
			ShowCaptions = false;
			ShowNCaps = 0;
			
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			
			TempString = PROMOTION_SCREEN_GRAPHIC;
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &TempString, PlatformRef), ==, false)
			{
				GraphicsObjectRef->EndDraw();
				return false;
			}

			Region.left = 0;
			Region.top = 0;
			Region.right = 160;
			Region.bottom = 192;

			GraphicsObjectRef->DrawSprite(	(GraphicsObjectRef->GetDisplay()->cxWidth - Region.right) >> 1, 
											(GraphicsObjectRef->GetDisplay()->cyHeight - Region.bottom) >> 1, 
											BackgroundGraphic,
											Region);
			break;

		case EFES_Single_League_New_Relegate:
		case EFES_Single_League_TeamManager_Relegate:
			TempString = TEXT_MENU_TITLE_LEAGUE;

			TempStringArray[0] = GameDataRef->GetActiveTeamByIndex(0)->GetTeamName();
			TempStringArray[1] = "Are Relegated.";
			ShowNCaps = 2;
			break;

			// Need to play game to decide promotion / relegation.
		case EFES_Single_League_New_PlayOff:
		case EFES_Single_League_TeamManager_PlayOff:
			TempString = TEXT_MENU_TITLE_LEAGUE;

			TempStringArray[0] = "Playoff";
			// Assumes that I've already chosen the teams.
			TempStringArray[1] = GameDataRef->GetActiveTeamByIndex(0)->GetShortTeamName();
			TempStringArray[1] += " Vs ";
			TempStringArray[1] = GameDataRef->GetActiveTeamByIndex(1)->GetShortTeamName();
			ShowNCaps = 2;
			break;

			// This state for all other instances.
		case EFES_Single_League_New_End_Season:
		case EFES_Single_League_TeamManager_End_Season:
			TempString = TEXT_MENU_TITLE_LEAGUE;
			TempStringArray[0] = "New Season";
			ShowNCaps = 1;
			break;

		case EFES_Single_League_New_Save_Failed:
		case EFES_Single_League_TeamManager_Save_Failed:
		case EFES_Single_Cup_New_Save_Failed:
			TempString = TEXT_MENU_TITLE_MAIN;
			TempStringArray[0] = "Save failed";
			ShowNCaps = 1;
			break;

		case EFES_Single_League_New_Save_Successful:
		case EFES_Single_League_TeamManager_Save_Successful:
		case EFES_Single_Cup_New_Save_Successful:
			TempString = TEXT_MENU_TITLE_MAIN;
			TempStringArray[0] = "Save successful";
			ShowNCaps = 1;
			break;

		case EFES_Load_Failed:
			TempString = TEXT_MENU_TITLE_MAIN;
			TempStringArray[0] = "Load failed";
			ShowNCaps = 1;
			break;

		case EFES_Single_League_New_Load_Successful:
		case EFES_Single_League_TeamManager_Load_Successful:
		case EFES_Single_Cup_New_Load_Successful:
			TempString = TEXT_MENU_TITLE_MAIN;
			TempStringArray[0] = "Load successful";
			ShowNCaps = 1;
			break;

		default:
			GraphicsObjectRef->EndDraw();
			return false;
		}

		if (ShowCaptions)
		{
			// Load splash screen.
			SAFELY_DELETE(BackgroundGraphic);
			NEW(BackgroundGraphic, CGraphicsData());
			
			SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
			{
				GraphicsObjectRef->EndDraw();
				return false;
			}

			Region.left = 0;
			Region.top = 0;
			Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
			Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
			GraphicsObjectRef->DrawSprite(0, 0, BackgroundGraphic, Region);

			// Screen title
			GraphicsObjectRef->DrawString(	TopSmallFont, 
											&TempString,
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_CAPTION_POSITION,
											TEXT_CENTRED);


			// Show all the text...
			for (i = ShowNCaps; i--;)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&TempStringArray[i],
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												((GraphicsObjectRef->GetDisplay()->cyHeight - MainMenuFont->GetLineHeight() * ShowNCaps) >> 1) + (i * MainMenuFont->GetLineHeight()),
												TEXT_CENTRED);
			}


			// Screen footer
			GraphicsObjectRef->DrawString(	BottomSmallFont, 
											&CString(TEXT_PRESS_FIRE),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION,
											TEXT_CENTRED);
		}

		GraphicsObjectRef->EndDraw();

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}	

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// Exit the screen.
	if (	(ControllerRef->IsButtonPressedDB(Select)) 
		||	(ControllerRef->IsButtonPressedDB(MouseSelect))
		||	(GetTimeDifference(&TimeNow, &DialogStartTime) > 5.0f))
	{
		
		switch (FrontEndState)
		{
		case EFES_Single_KnockOut_GameOver:
			SetState(EFES_Main);
			break;
		
		// Continue on to the manager screen.
		
		case EFES_Single_League_New_Relegate:
		case EFES_Single_League_New_Promotion:
			SetState(EFES_Single_League_New_End_Season);
			break;

		case EFES_Single_League_New_End_Season:
			GameDataRef->NewLeagueSeason(); // The break is deliberately missing!

		case EFES_Single_League_New_PlayOff:
			SetState(EFES_Single_League_New_Manager);
			break;

		// Continue on to the manager (team manager) screen.
		case EFES_Single_League_TeamManager_Relegate:
		case EFES_Single_League_TeamManager_Promotion:
			SetState(EFES_Single_League_TeamManager_End_Season);
			break;
		
		case EFES_Single_League_TeamManager_End_Season:
			GameDataRef->NewLeagueSeason(); // The break is deliberately missing!

		case EFES_Single_League_TeamManager_PlayOff:
			SetState(EFES_Single_League_TeamManager_Manager);
			break;

		// Team manager save states
		case EFES_Single_League_TeamManager_Save_Successful:
		case EFES_Single_League_TeamManager_Save_Failed:
			SetState(EFES_Single_League_TeamManager_Manager);
			break;

		// League save states
		case EFES_Single_League_New_Save_Successful:
		case EFES_Single_League_New_Save_Failed:
			SetState(EFES_Single_League_New_Manager);
			break;

		// Cup save states
		case EFES_Single_Cup_New_Save_Successful:
		case EFES_Single_Cup_New_Save_Failed:
			SetState(EFES_Single_Cup_New_Manager);
			break;

		case EFES_Load_Failed:
			SetState(EFES_Main);
			break;

		case EFES_Single_League_New_Load_Successful:
			SetState(EFES_Single_League_New_Manager);
			break;

		case EFES_Single_League_TeamManager_Load_Successful:
			SetState(EFES_Single_League_TeamManager_Manager);
			break;

		case EFES_Single_Cup_New_Load_Successful:
			SetState(EFES_Single_Cup_New_Manager);
			break;

		default:
			GraphicsObjectRef->EndDraw();
			return false;
		}
	}

	return true;
}




//*********************************************************************************************************
bool	CFrontEnd::Volume()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		//MenuTextArray[0]	= "Music On";
		//MenuTextArray[1]	= "Music Off";
		MenuTextArray[1]	= "Back";
		
		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 1;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
		SelectedFace = GameDataRef->GetPreferences()->Volume;
	}

	if (RedrawCountDown >= 0)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));

		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&(CString("Audio")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

	}

	//RedrawCountDown--;
	//if (RedrawCountDown < -1)
	//	RedrawCountDown = -1;
	// ----
	// Process
		//to move the cursor
	IfLeftDB					// Macros
	{
		RedrawCountDown = 1;
		SelectedFace --;
		SOUND_SELECT;//SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfRightDB					// Macros
	{
		SelectedFace ++;
		RedrawCountDown = 1;
		SOUND_SELECT;//SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	if (SelectedFace > 10)
	{
		SelectedFace = 10;
	}

	if (SelectedFace < 0)
	{
		SelectedFace = 0;
	}

	if (GameDataRef->GetPreferences()->Volume != SelectedFace)
	{
		GameDataRef->GetPreferences()->Volume = SelectedFace;
		SoundSystemRef->SetVolume(SelectedFace);
	}

	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];
	RECT region;

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		
		region.left = 0;
		region.top = (SCREEN_EXIT - MENU_ITEM_SPACING);
		region.right = 240;
		region.bottom = (SCREEN_EXIT - MENU_ITEM_SPACING) + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, (SCREEN_EXIT - MENU_ITEM_SPACING), BackgroundGraphic, region));

		if (GameDataRef->GetPreferences()->MusicEnabled)
			MenuTextArray[0] = "Music On";
		else
			MenuTextArray[0] = "Music Off";
		
		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											(SCREEN_EXIT - MENU_ITEM_SPACING) + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (i != CurrentMenuPos)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										(SCREEN_EXIT - MENU_ITEM_SPACING) + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										(SCREEN_EXIT - MENU_ITEM_SPACING) + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}

#define BASELINE		175
#define BLOCKHEIGHT		10
#define BLOCKSPACING    1
#define BLOCKWIDTH		18
#define BLOCKSTART		(USABLE_LEFT + 6)

	// Now do the detect bit.
	for(i = 0; i <= 10; i++)
	{
		bool InRect = true;
		POINT*	_Point = GameWindowRef->GetMousePoint();

		//hack to get it to regester the whole column
		region.left		= BLOCKSTART + (BLOCKWIDTH * i);
		region.top		= BASELINE - ((BLOCKHEIGHT * i) + (BLOCKSPACING * (i - 1)));
		region.right	= region.left + BLOCKWIDTH - (BLOCKSPACING);
		region.bottom	= region.top + ((BLOCKHEIGHT + 1) * (i + 1));

		if (	(_Point->x >= 0)			// Is the mouse on the screen?
			&&	(_Point->y >= 0)
			&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
			&&	(_Point->x < region.right) 
			&&	(_Point->y > region.top) 
			&&	(_Point->y < region.bottom) 
			)
			InRect = true;	// Yes.
		else
			InRect = false;	// No, to one of the above.
		
		// Process mouse
		if (SelectedFace != i)
		{
			if (InRect)
			{
				RedrawCountDown = 1;

				int vol;
				vol = i;
				// Zoom to the control.
				SelectedFace = i;
				//MouseMovedSelected = true;
				GameDataRef->GetPreferences()->Volume = vol;
				SoundSystemRef->SetVolume(vol);

				SOUND_SELECT;
			}
		}
	}

	
	// Now draw
	if (RedrawCountDown >= 0)
	{	
		for(i = 0; i <= 10; i++)
		{
			RECT region;

			if(SelectedFace != i)
			{
				if(i == 0)
				{
					region.top		= BASELINE + 1;
					region.bottom	= BASELINE + BLOCKHEIGHT + 1;
					region.left		= BLOCKSTART;
					region.right	= BLOCKSTART + BLOCKWIDTH - (BLOCKSPACING);
					//draw red block underbaseline

					GraphicsObjectRef->DrawRect(&region, 150, 0, 0);
				}
				else
				{
					//draw i blocks above line in dark green
					for(int p = 0; p <= i; p++)
					{
						region.top		= BASELINE - ( (BLOCKHEIGHT * p) + (BLOCKSPACING * (p - 1)));
						region.bottom	= region.top + BLOCKHEIGHT;
						region.left		= BLOCKSTART + ((BLOCKWIDTH * i) );
						region.right	= region.left + BLOCKWIDTH - (BLOCKSPACING);
						//draw red block underbaseline

						GraphicsObjectRef->DrawRect(&region, 0, 150, 0);
					}
				}
			}
			else
			{
				//draw i blocks above baseline in light green
				if(i == 0)
				{
					region.top		= BASELINE + 1;
					region.bottom	= BASELINE + BLOCKHEIGHT + 1;
					region.left		= BLOCKSTART;
					region.right	= BLOCKSTART + BLOCKWIDTH - (BLOCKSPACING);
					//draw red block underbaseline

					GraphicsObjectRef->DrawRect(&region, 255, 0, 0);
				}
				else
				{
					
					//draw i blocks above line in dark green
					for(int p = 0; p <= i; p++)
					{
						region.top		= BASELINE - ((BLOCKHEIGHT * p) + (BLOCKSPACING * (p - 1)));
						region.bottom	= region.top + BLOCKHEIGHT;
						region.left		= BLOCKSTART + (BLOCKWIDTH * i);
						region.right	= region.left + BLOCKWIDTH - (BLOCKSPACING);
						//draw red block underbaseline

						GraphicsObjectRef->DrawRect(&region, 0, 255, 0);
					}
				}
			}
		}

		//diplay the volume in text
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&(CString(("Use left and right or the \nstylus to change volume"))),
										(GraphicsObjectRef->GetDisplay()->cxWidth >> 1) - 85, 
										BASELINE + 20,
										TEXT_LEFT);

	}

	RedrawCountDown--;
	if (RedrawCountDown < 0)
		RedrawCountDown = -1;

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			if (GameDataRef->GetPreferences()->MusicEnabled)
			{
				// Stop mod.
				GameDataRef->GetPreferences()->MusicEnabled = false;
				SoundSystemRef->StopMod();
			}
			else
			{
				// Restart mod.
				GameDataRef->GetPreferences()->MusicEnabled = true;
				SoundSystemRef->PlayMod();
			}
			break;
		/*case 1:
			SOUND_SELECT;
			if(GameDataRef->GetPreferences()->MusicEnabled != false)
			{
				GameDataRef->GetPreferences()->MusicEnabled = false;
				SoundSystemRef->StopMod();
			}
			break;
			*/
		case 1:
			SOUND_SELECT;
			SetState(EFES_Options);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//*********************************************************************************************************
bool	CFrontEnd::Buffer()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[1]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));

		FILE *text;
		TempString = PlatformRef->GetPath(EP_Text);
		// Generate file name here
		TempString += "db.ctd";// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												USABLE_LEFT + USABLE_COMMON_BORDER, 
												USABLE_TOP + USABLE_COMMON_BORDER);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);

		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&(CString("Graphics")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

#define	BUFFER_SCREEN_EXIT			(SCREEN_FOOTER_POSITION - 6 * MENU_ITEM_SPACING)

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = BUFFER_SCREEN_EXIT;
		region.right = 240;
		region.bottom = region.top + 2 * MENU_ITEM_SPACING;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, BUFFER_SCREEN_EXIT, BackgroundGraphic, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			if (i != 0)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												BUFFER_SCREEN_EXIT + 1 * MENU_ITEM_SPACING,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}
			else
			{

				if(GameDataRef->GetPreferences()->DoubleBufferingEnabled)
					TempString = "Enabled";
				else
					TempString = "Disabled";
				
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&TempString,
												(GraphicsObjectRef->GetDisplay()->cxWidth >> 1), 
												BUFFER_SCREEN_EXIT,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);
				
			}	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										BUFFER_SCREEN_EXIT + CurrentMenuPos * MENU_ITEM_SPACING,
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										BUFFER_SCREEN_EXIT + CurrentMenuPos * MENU_ITEM_SPACING,	
										TEXT_RIGHT);
	}
	
	

	
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT
			GameDataRef->GetPreferences()->DoubleBufferingEnabled = !GameDataRef->GetPreferences()->DoubleBufferingEnabled;
			RedrawCountDown = 1;
			break;
		case 1:
			SOUND_SELECT;
			SetState(EFES_Options);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//*********************************************************************************************************
bool	CFrontEnd::Stylus()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 6;	// Remember to update this for each method!!!
	char convert[24];
		

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "No stylus";
		MenuTextArray[1]	= "Screen input";
		MenuTextArray[2]	= "Run to cursor";
		MenuTextArray[3]	= "Right pad";
		MenuTextArray[4]	= "Left pad";
		MenuTextArray[5]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
		CurrentMenuPos = GameDataRef->GetPreferences()->StylusControl;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));

		//****************************************
		// This does the text!!!
		//****************************************

		FILE *text;

		if(CurrentMenuPos > 5)
		{
			CurrentMenuPos = 0;
		}

		TempString = PlatformRef->GetPath(EP_Text);
		// Generate file name here
		sprintf(convert, "stylus%d.ctd", GameDataRef->GetPreferences()->StylusControl);

		TempString += convert;
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												USABLE_LEFT + USABLE_COMMON_BORDER, 
												USABLE_TOP + USABLE_COMMON_BORDER);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);

		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&(CString("Controls")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

#define TEXT_START		(SCREEN_FOOTER_POSITION - 7 * MENU_ITEM_SPACING)
	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = TEXT_START;
		region.right = 240;
		region.bottom = region.top + NumberOfOptions * MENU_ITEM_SPACING;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, TEXT_START, BackgroundGraphic, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			if (i == 5)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												(TEXT_START) + i * MENU_ITEM_SPACING,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}
			else
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												TEXT_START + i * MENU_ITEM_SPACING,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);

				if (GameDataRef->GetPreferences()->StylusControl == i)
				{
					//top line
					RECT box;
					box.left	=	28;
					box.right	=	212;
					box.top		=	(TEXT_START + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	box.top + 1;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//bottom line
					box.left	=	28;
					box.right	=	212;
					box.top		=	(TEXT_START + i * MENU_ITEM_SPACING) + 16;
					box.bottom	=	box.top + 1;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//left line
					box.left	=	27;
					box.right	=	28;
					box.top		=	(TEXT_START + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	(TEXT_START + i * MENU_ITEM_SPACING) + 16;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//right line
					box.left	=	211;
					box.right	=	212;
					box.top		=	(TEXT_START + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	(TEXT_START + i * MENU_ITEM_SPACING) + 16;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 
				}
			}

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					RedrawCountDown = 1;
					//DialogInitialised = false;
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										(TEXT_START) + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);
			
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										(TEXT_START) + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
			SOUND_SELECT;
			GameDataRef->GetPreferences()->StylusControl = CurrentMenuPos;
			DialogInitialised = false;
			break;
		case 5:
			SOUND_SELECT;
			SetState(EFES_Options);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//****************************************************************************************************
bool	CFrontEnd::Difficulty()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 6;	// Remember to update this for each method!!!

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Very Easy";
		MenuTextArray[1]	= "Easy";
		MenuTextArray[2]	= "Normal";
		MenuTextArray[3]	= "Hard";
		MenuTextArray[4]	= "Very Hard";
		MenuTextArray[5]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = MENU_TOP - 15;
		region.right = 240;
		region.bottom = SCREEN_FOOTER_POSITION + MENU_ITEM_SPACING;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP - 15, BackgroundGraphic, region));

			// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString("Difficulty"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												MENU_TOP + i * (MENU_ITEM_SPACING),
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);

				if( (GameDataRef->GetDefaultGameDifficulty() + 2) == i)
				{
					//top line
					RECT box;
					box.left	=	50;
					box.right	=	190;
					box.top		=	(MENU_TOP + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	box.top + 1;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//bottom line
					box.left	=	50;
					box.right	=	190;
					box.top		=	(MENU_TOP + i * MENU_ITEM_SPACING) + 16;
					box.bottom	=	box.top + 1;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//left line
					box.left	=	50;
					box.right	=	51;
					box.top		=	(MENU_TOP + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	(MENU_TOP + i * MENU_ITEM_SPACING) + 16;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 

					//right line
					box.left	=	189;
					box.right	=	190;
					box.top		=	(MENU_TOP + i * MENU_ITEM_SPACING) - 2;
					box.bottom	=	(MENU_TOP + i * MENU_ITEM_SPACING) + 16;
					GraphicsObjectRef->DrawRect( &box, 100, 160, 160); 
				}

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:	
		case 1:
		case 2:
		case 3:
		case 4:
			SOUND_SELECT;
			GameDataRef->SetDefaultGameDifficulty(CurrentMenuPos - 2);
			break;
		case 5:
			SOUND_SELECT;
			SetState(EFES_Options);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
/**************************************EOF*****************************************************/