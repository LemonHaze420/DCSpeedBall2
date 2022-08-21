#include	"Match.h"
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"

#include	"graphicsclasses.h"
#include	"GameDefines.h"
//for the random number generator
#include	<stdlib.h>
#include	<windows.h>
#include	"Platform.h"

//**********************************

#define PITCH_IMAGE				"pitch"
#define	BALL_IMAGE				"ball"
#define	BLUE_PLAYER_IMAGE		"blue"
#define	RED_PLAYER_IMAGE		"red"
#define	PICKUP_IMAGE			"pickup"
#define	MEDIC_IMAGE				"medic"
#define	LAUNCHER_IMAGE			"launcher"
#define	ADDONS_IMAGE			"addons"
#define	CHUTE_IMAGE				"chute"
#define	INFOBAR_IMAGE			"infobar"
#define MARKER_IMAGE			"markers"

#define	INFOBAR_FONT			"infobarfont"
//#define	POPUP_FONT				"popupfont"
#define	MAIN_MENU_FONT			"mainmenufont"

#define	ARGGH_SAMPLE			"arggh.uwd"
#define	AIRHORN_SAMPLE			"airhorn.uwd"
#define	BELL_SAMPLE				"bell.uwd"
#define	BOUNCE_SAMPLE			"bounce.uwd"
#define	CROWD_SAMPLE			"crowd.uwd"
#define	CROWD2_SAMPLE			"crowd2.uwd"
#define	ELECTRO_SAMPLE			"electro.uwd"
#define	GET_SAMPLE				"get.uwd"
#define	OOF_SAMPLE				"oof.uwd"
#define	PEOW_SAMPLE				"peow.uwd"
#define	PUNCH_SAMPLE			"punch.uwd"
#define	READY_SAMPLE			"ready.uwd"
#define	REPLAY_SAMPLE			"replay.uwd"
#define	TELEPORT_SAMPLE			"teleport.uwd"
#define	THROW_SAMPLE			"throw.uwd"
#define	THUNDER_SAMPLE			"thunder.uwd"
#define	TUMBLE_SAMPLE			"tumble.uwd"
#define	WALL_SAMPLE				"wall.uwd"
#define	STAR_SAMPLE				"star.uwd"
#define	POWERUP_SAMPLE			"collectpowerup.uwd"

#define	PITCH_MAP_DATA			"map.dat"

// ----
CMatch::CMatch()
{
	// Imported object refs.
	PlatformRef				= NULL;
	GameDataRef				= NULL;
	GameWindowRef			= NULL;
	GraphicsObjectRef		= NULL;
	ControllerRef			= NULL;
	SoundSystemRef			= NULL;

	// Internal data objects
	TheBall					= NULL;

	// Graphic objects
	ThePitchGraphic			= NULL;
	TheBallGraphic			= NULL;
	TheBluePlayerGraphic	= NULL;
	TheRedPlayerGraphic		= NULL;
	ThePickupGraphic		= NULL;
	TheMedicGraphic			= NULL;
	TheLauncherGraphic		= NULL;
	TheAddOnsGraphic		= NULL;
	TheChuteGraphic			= NULL;
	ThePopUpGraphic			= NULL;
	TheInfoBarGraphic		= NULL;
	TheDPadGraphic			= NULL;
	TheMarkerGraphic		= NULL;

	TheInfoBarFont			= NULL;
	ThePopUpFont			= NULL;
	TheMainMenuFont			= NULL;

	// Samples
	TheArgghSample			= NULL;
	TheAirHornSample		= NULL;
	TheBellSample			= NULL;
	TheBounceSample			= NULL;
	TheCrowdSample			= NULL;
	TheCrowd2Sample			= NULL;
	TheElectroSample		= NULL;
	TheGetSample			= NULL;
	TheIcecreamSample		= NULL;
	TheOofSample			= NULL;
	ThePeowSample			= NULL;
	ThePunchSample			= NULL;
	TheReadySample			= NULL;
	TheReplaySample			= NULL;
	TheTeleportSample		= NULL;
	TheThrowSample			= NULL;
	TheThunderSample		= NULL;
	TheTumbleSample			= NULL;
	TheWallSample			= NULL;
	TheStarSample			= NULL;
	TheAllStarSample		= NULL;
	TheSirenSample			= NULL;
	ThePowerupSample		= NULL;
	TheBallLaunchSample		= NULL;

	// State info
	MatchState		= StartingMatch;

	LeftDome		= NULL;
	RightDome		= NULL;

	TopShroom		= NULL;
	BottomShroom	= NULL;

	LeftChute		= NULL;
	RightChute		= NULL;

	LeftBank		= NULL;
	RightBank		= NULL;

	ThingA			= NULL;
	ThingB			= NULL;

	for(int i = 0; i < MAX_TOKENS; i++)
	{
		TokenArray[i] = NULL;
	}

	PreviousMatchState = PlayingGame;
	OptionSelected = 0;

	IsAirhornPlaying = false;
}

// ---- **********
CMatch::~CMatch()
{
	SoundSystemRef->StopAllSamples();
	// We don't delete imported data...

	// Data objects
	SAFELY_DELETE(TheBall);
	SAFELY_DELETE(LeftDome);
	SAFELY_DELETE(RightDome);
	SAFELY_DELETE(TopShroom);
	SAFELY_DELETE(BottomShroom);
	SAFELY_DELETE(LeftChute);
	SAFELY_DELETE(RightChute);
	SAFELY_DELETE(LeftBank);
	SAFELY_DELETE(RightBank);

	// Graphic objects
	SAFELY_DELETE(ThePitchGraphic);
	SAFELY_DELETE(TheBallGraphic);
	SAFELY_DELETE(TheBluePlayerGraphic);
	SAFELY_DELETE(TheRedPlayerGraphic);
	SAFELY_DELETE(ThePickupGraphic);
	SAFELY_DELETE(TheMedicGraphic);
	SAFELY_DELETE(TheLauncherGraphic);
	SAFELY_DELETE(TheAddOnsGraphic);
	SAFELY_DELETE(TheChuteGraphic);
	SAFELY_DELETE(ThePopUpGraphic);
	SAFELY_DELETE(TheInfoBarGraphic);
	SAFELY_DELETE(TheDPadGraphic);
	SAFELY_DELETE(TheMarkerGraphic);

	SAFELY_DELETE(TheInfoBarFont);
	SAFELY_DELETE(ThePopUpFont);
	SAFELY_DELETE(TheMainMenuFont);

	// Samples
	SAFELY_DELETE(TheArgghSample);
	SAFELY_DELETE(TheAirHornSample);
	SAFELY_DELETE(TheBellSample);
	SAFELY_DELETE(TheBounceSample);
	SAFELY_DELETE(TheCrowdSample);
	SAFELY_DELETE(TheCrowd2Sample);
	SAFELY_DELETE(TheElectroSample);
	SAFELY_DELETE(TheGetSample);
	SAFELY_DELETE(TheIcecreamSample);
	SAFELY_DELETE(TheOofSample);
	SAFELY_DELETE(ThePeowSample);
	SAFELY_DELETE(ThePunchSample);
	SAFELY_DELETE(TheReadySample);
	SAFELY_DELETE(TheReplaySample);
	SAFELY_DELETE(TheTeleportSample);
	SAFELY_DELETE(TheThrowSample);
	SAFELY_DELETE(TheThunderSample);
	SAFELY_DELETE(TheTumbleSample);
	SAFELY_DELETE(TheWallSample);
	SAFELY_DELETE(TheStarSample);
	SAFELY_DELETE(TheAllStarSample);
	SAFELY_DELETE(TheSirenSample);
	SAFELY_DELETE(ThePowerupSample);
	SAFELY_DELETE(TheBallLaunchSample);

	// Things (medics).
	SAFELY_DELETE(ThingA);
	SAFELY_DELETE(ThingB);
	
	for(int i = 0; i < MAX_TOKENS; i++)
	{
		SAFELY_DELETE(TokenArray[i]);
		TokenArray[i] = NULL;
	}
}

// ----
bool	CMatch::Init(	CPlatform*		_PlatformRef,
						CGameData*		_GameDataRef,
						CGameWindow*	_GameWindowRef,
						CGraphics*		_GraphicsObjectRef,
						CControls*		_ControllerRef,
						CSoundSystem*	_SoundSystemRef)
{
	FILE*	FH;

	// ----
	// Store imported
	PlatformRef			= _PlatformRef;
	GameDataRef			= _GameDataRef;
	GameWindowRef		= _GameWindowRef;
	GraphicsObjectRef	= _GraphicsObjectRef;
	ControllerRef		= _ControllerRef;
	SoundSystemRef		= _SoundSystemRef;



	// ----
	// Internal data objects
	SAFELY_DELETE(TheBall);
	NEW(TheBall, CBall());

	//init the zap domes
	SAFELY_DELETE(LeftDome);
	NEW(LeftDome, CDome(-320, -368));

	SAFELY_DELETE(RightDome);
	NEW(RightDome, CDome(320, 368));

	SAFELY_DELETE(TopShroom);
	NEW(TopShroom, CShroom(SHROOM_X, TSHROOM_Y));

	SAFELY_DELETE(BottomShroom);
	NEW(BottomShroom, CShroom(SHROOM_X, BSHROOM_Y));

	SAFELY_DELETE(LeftChute);
	NEW(LeftChute, CChute(	-310, 
							-32, 
							GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber(),
							GameDataRef->GetActiveTeamByIndex(1)->GetTeamNumber()
							));

	SAFELY_DELETE(RightChute);
	NEW(RightChute, CChute(	310, 
							32,
							GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber(),
							GameDataRef->GetActiveTeamByIndex(1)->GetTeamNumber()
							));

	SAFELY_DELETE(LeftBank);
	NEW(LeftBank, CStarBank(true, GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber()));

	SAFELY_DELETE(RightBank);
	NEW(RightBank, CStarBank(false, GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber()));

	SAFELY_DELETE(ThingA);
	NEW(ThingA, CExtra());
	
	SAFELY_DELETE(ThingB);
	NEW(ThingB, CExtra());

	// ----
	// Graphics.

	// Pitch
	SAFELY_DELETE(ThePitchGraphic);
	NEW(ThePitchGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ThePitchGraphic, Init(GraphicsObjectRef, &CString(PITCH_IMAGE), PlatformRef), ==, false)
		return false;

	// Ball
	SAFELY_DELETE(TheBallGraphic);
	NEW(TheBallGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheBallGraphic, Init(GraphicsObjectRef, &CString(BALL_IMAGE), PlatformRef), ==, false)
		return false;

	// Blue player
	SAFELY_DELETE(TheBluePlayerGraphic);
	NEW(TheBluePlayerGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheBluePlayerGraphic, Init(GraphicsObjectRef, &CString(BLUE_PLAYER_IMAGE), PlatformRef), ==, false)
		return false;

	// Red player
	SAFELY_DELETE(TheRedPlayerGraphic);
	NEW(TheRedPlayerGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheRedPlayerGraphic, Init(GraphicsObjectRef, &CString(RED_PLAYER_IMAGE), PlatformRef), ==, false)
		return false;

	// Pickups???
	SAFELY_DELETE(ThePickupGraphic);
	NEW(ThePickupGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ThePickupGraphic, Init(GraphicsObjectRef, &CString(PICKUP_IMAGE), PlatformRef), ==, false)
		return false;

	// Medic
	SAFELY_DELETE(TheMedicGraphic);
	NEW(TheMedicGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheMedicGraphic, Init(GraphicsObjectRef, &CString(MEDIC_IMAGE), PlatformRef), ==, false)
		return false;

	// Launcher
	SAFELY_DELETE(TheLauncherGraphic);
	NEW(TheLauncherGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheLauncherGraphic, Init(GraphicsObjectRef, &CString(LAUNCHER_IMAGE), PlatformRef), ==, false)
		return false;

	// AddOns
	SAFELY_DELETE(TheAddOnsGraphic);
	NEW(TheAddOnsGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheAddOnsGraphic, Init(GraphicsObjectRef, &CString(ADDONS_IMAGE), PlatformRef), ==, false)
		return false;

	// Chute add ons
	SAFELY_DELETE(TheChuteGraphic);
	NEW(TheChuteGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheChuteGraphic, Init(GraphicsObjectRef, &CString(CHUTE_IMAGE), PlatformRef), ==, false)
		return false;

	// PopUps
	SAFELY_DELETE(ThePopUpGraphic);
	NEW(ThePopUpGraphic, CGraphicsData());
	SAFELY_CONTROL(if, ThePopUpGraphic, Init(GraphicsObjectRef, &CString(POPUP_IMAGE), PlatformRef), ==, false)
		return false;

	// HUD - just to please Anthony.
	SAFELY_DELETE(TheInfoBarGraphic);
	NEW(TheInfoBarGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheInfoBarGraphic, Init(GraphicsObjectRef, &CString(INFOBAR_IMAGE), PlatformRef), ==, false)
		return false;

#ifndef DEMO_BUILD
//	// DPad
//	SAFELY_DELETE(TheDPadGraphic);
//	NEW(TheDPadGraphic, CGraphicsData());
//	SAFELY_CONTROL(if, TheDPadGraphic, Init(GraphicsObjectRef, &CString(DPAD_IMAGE), PlatformRef), ==, false)
//		return false;
#endif

	SAFELY_DELETE(TheMarkerGraphic);
	NEW(TheMarkerGraphic, CGraphicsData());
	SAFELY_CONTROL(if, TheMarkerGraphic, Init(GraphicsObjectRef, &CString(MARKER_IMAGE), PlatformRef), ==, false)
		return false;


	// Score / time font
	SAFELY_DELETE(TheInfoBarFont);
	NEW(TheInfoBarFont, CFont());
	SAFELY_CONTROL(if, TheInfoBarFont, Init(PlatformRef, GraphicsObjectRef, &CString(INFOBAR_FONT)), ==, false)
		return false;


	SAFELY_DELETE(ThePopUpFont);
	NEW(ThePopUpFont, CFont());
	SAFELY_CONTROL(if, ThePopUpFont, Init(PlatformRef, GraphicsObjectRef, &CString(POPUP_FONT)), ==, false)
		return false;

	SAFELY_DELETE(TheMainMenuFont);
	NEW(TheMainMenuFont, CFont());
	SAFELY_CONTROL(if, TheMainMenuFont, Init(PlatformRef, GraphicsObjectRef, &CString(MAIN_MENU_FONT)), ==, false)
		return false;


	// ---- 
	// Samples
	SAFELY_DELETE(TheArgghSample);
	NEW(TheArgghSample, CSample());
	SAFELY_CONTROL(if, TheArgghSample, Init(PlatformRef, &CString(ARGGH_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheAirHornSample);
	NEW(TheAirHornSample, CSample());
	SAFELY_CONTROL(if, TheAirHornSample, Init(PlatformRef, &CString(AIRHORN_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheBellSample);
	NEW(TheBellSample, CSample());
	SAFELY_CONTROL(if, TheBellSample, Init(PlatformRef, &CString(BELL_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheBounceSample);
	NEW(TheBounceSample, CSample());
	SAFELY_CONTROL(if, TheBounceSample, Init(PlatformRef, &CString(BOUNCE_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheCrowdSample);
	NEW(TheCrowdSample, CSample());
	SAFELY_CONTROL(if, TheCrowdSample, Init(PlatformRef, &CString(CROWD_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheCrowd2Sample);
	NEW(TheCrowd2Sample, CSample());
	SAFELY_CONTROL(if, TheCrowd2Sample, Init(PlatformRef, &CString(CROWD2_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheElectroSample);
	NEW(TheElectroSample, CSample());
	SAFELY_CONTROL(if, TheElectroSample, Init(PlatformRef, &CString(ELECTRO_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheGetSample);
	NEW(TheGetSample, CSample());
	SAFELY_CONTROL(if, TheGetSample, Init(PlatformRef, &CString(GET_SAMPLE)), ==, false)
		return false;

//	SAFELY_DELETE(TheIcecreamSample);
//	NEW(TheIcecreamSample, CSample());
//	SAFELY_CONTROL(if, TheIcecreamSample, Init(PlatformRef, &CString(ICECREAM_SAMPLE)), ==, false)
//		return false;

	SAFELY_DELETE(TheOofSample);
	NEW(TheOofSample, CSample());
	SAFELY_CONTROL(if, TheOofSample, Init(PlatformRef, &CString(OOF_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(ThePeowSample);
	NEW(ThePeowSample, CSample());
	SAFELY_CONTROL(if, ThePeowSample, Init(PlatformRef, &CString(PEOW_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(ThePunchSample);
	NEW(ThePunchSample, CSample());
	SAFELY_CONTROL(if, ThePunchSample, Init(PlatformRef, &CString(PUNCH_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheReadySample);
	NEW(TheReadySample, CSample());
	SAFELY_CONTROL(if, TheReadySample, Init(PlatformRef, &CString(READY_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheReplaySample);
	NEW(TheReplaySample, CSample());
	SAFELY_CONTROL(if, TheReplaySample, Init(PlatformRef, &CString(REPLAY_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheTeleportSample);
	NEW(TheTeleportSample, CSample());
	SAFELY_CONTROL(if, TheTeleportSample, Init(PlatformRef, &CString(TELEPORT_SAMPLE)), ==, false)
		return false;
		
	SAFELY_DELETE(TheThrowSample);
	NEW(TheThrowSample, CSample());
	SAFELY_CONTROL(if, TheThrowSample, Init(PlatformRef, &CString(THROW_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheThunderSample);
	NEW(TheThunderSample, CSample());
	SAFELY_CONTROL(if, TheThunderSample, Init(PlatformRef, &CString(THUNDER_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheTumbleSample);
	NEW(TheTumbleSample, CSample());
	SAFELY_CONTROL(if, TheTumbleSample, Init(PlatformRef, &CString(TUMBLE_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheWallSample);
	NEW(TheWallSample, CSample());
	SAFELY_CONTROL(if, TheWallSample, Init(PlatformRef, &CString(WALL_SAMPLE)), ==, false)
		return false;

	SAFELY_DELETE(TheStarSample);
	NEW(TheStarSample, CSample());
	SAFELY_CONTROL(if, TheStarSample, Init(PlatformRef, &CString(STAR_SAMPLE)), ==, false)
		return false;

//	SAFELY_DELETE(TheAllStarSample);
//	NEW(TheAllStarSample, CSample());
//	SAFELY_CONTROL(if, TheAllStarSample, Init(PlatformRef, &CString(ALLSTAR_SAMPLE)), ==, false)
//		return false;

//	SAFELY_DELETE(TheSirenSample);
//	NEW(TheSirenSample, CSample());
//	SAFELY_CONTROL(if, TheSirenSample, Init(PlatformRef, &CString(SIREN_SAMPLE)), ==, false)
//		return false;

	SAFELY_DELETE(ThePowerupSample);
	NEW(ThePowerupSample, CSample());
	SAFELY_CONTROL(if, ThePowerupSample, Init(PlatformRef, &CString(POWERUP_SAMPLE)), ==, false)
		return false;

//	SAFELY_DELETE(TheBallLaunchSample);
//	NEW(TheBallLaunchSample, CSample());
//	SAFELY_CONTROL(if, TheBallLaunchSample, Init(PlatformRef, &CString(LAUNCH_SAMPLE)), ==, false)
//		return false;

	//Load data

	CString FilePath = PlatformRef->GetPath(EP_Data);
	FilePath += PITCH_MAP_DATA;
#ifdef _UNICODE
	FH = _wfopen(FilePath.GetString(), TEXT("rb"));
#else
	FH = fopen(FilePath.GetString(), TEXT("rb"));
#endif
	if (FH)
	{
		if (fread((void*)PitchMap, 2 * PITCH_TILE_WIDTH * PITCH_TILE_HEIGHT, 1, FH) != 1)
		{
			fclose(FH);
			return false;
		}
		fclose(FH);
	}
	else
		return false;
	
	// ----
	// Replay buffer
	ReplayBufferIndex			= 0;
	ReplayBufferPlaybackIndex	= 0;
	ReplayTime					= REPLAY_BUFFER_LENGTH * 2;

	// ----
	// State
	CameraX = 0;
	CameraY = 0;
	CameraX = CMatch::PitchXToScreenX(0) - (GraphicsObjectRef->GetDisplay()->cxWidth >> 1);
	CameraY = CMatch::PitchYToScreenY(0) - (GraphicsObjectRef->GetDisplay()->cyHeight >> 1);

	MatchState	= StartingMatch;

	//set all the players on each team to playing the right way up or down the pitch
	CTeamMember* temp;

	GameHalf = FIRST_HALF;

	if(GameDataRef->GetGameType() == CUP)
	{
		if(GameDataRef->IsHomeGame())
		{
			TeamOneHalf = 0;
			for(int i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				temp->DirectionOfPlay = UP_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}

			for(i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				temp->DirectionOfPlay = DOWN_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}
		}
		else
		{
			GameHalf = SECOND_HALF;
			TeamOneHalf = 1;
			for(int i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				temp->DirectionOfPlay = DOWN_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}

			for(i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				temp->DirectionOfPlay = UP_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}
		}
	}
	else
	{
		if(GameDataRef->GetGameType() == LEAGUE)
		{
			if(GameDataRef->IsHomeGame())
			{
				TeamOneHalf = 0;
				for(int i = 0; i < 12; i++)
				{
					temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
					temp->DirectionOfPlay = UP_MAP;
					temp->HasPossesion = false;
					temp->setCurrentStamina(temp->getSta());
				}

				for(i = 0; i < 12; i++)
				{
					temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
					temp->DirectionOfPlay = DOWN_MAP;
					temp->HasPossesion = false;
					temp->setCurrentStamina(temp->getSta());
				}
			}
			else
			{
				GameHalf = SECOND_HALF;
				TeamOneHalf = 1;
				for(int i = 0; i < 12; i++)
				{
					temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
					temp->DirectionOfPlay = DOWN_MAP;
					temp->HasPossesion = false;
					temp->setCurrentStamina(temp->getSta());
				}

				for(i = 0; i < 12; i++)
				{
					temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
					temp->DirectionOfPlay = UP_MAP;
					temp->HasPossesion = false;
					temp->setCurrentStamina(temp->getSta());
				}
			}
		}
		else
		{
			TeamOneHalf = 0;
			for(int i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				temp->DirectionOfPlay = UP_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}

			for(i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				temp->DirectionOfPlay = DOWN_MAP;
				temp->HasPossesion = false;
				temp->setCurrentStamina(temp->getSta());
			}
		}
	}


	if(GameDataRef->GetGameType() != PRACTICE)
	{
		ActualHalf = 1;
		HalvesToPlay = 2;
	}
	else
	{
		if(GameDataRef->GetGameType() == PRACTICE)
		{
			ActualHalf = 1;
			HalvesToPlay = 200000;
		}
	}

	GameDataRef->GetActiveTeamByIndex(0)->ResetLastMatchStats();
	GameDataRef->GetActiveTeamByIndex(1)->ResetLastMatchStats();
	
	TeamAMulti = 1;
	TeamBMulti = 1;

	TimeCount = 0;
	PlayCrowd = false;
	HasSetActions = false;
	GoalCount = 0;

	FirstAICount = 0;
	SecondAICount = 0;

	ProcessWho = true;
	PlayReplay = true;
	PlayIcecream = true;

	InjuryCount = 0;

	IsAirhornPlaying = false;

	// ----
	// Other bits.
	//seed the random number generator with the current time
	SYSTEMTIME Time;

	GetSystemTime(&Time);

	int TimeInSecs = 0;
	//TimeInSecs += ((Time.wDay			* 60 * 60 * 24));
	//TimeInSecs += ((Time.wHour			* 60 * 60));
	//TimeInSecs += ((Time.wMinute			* 60));
	//TimeInSecs += ((Time.wSecond));
	TimeInSecs = ((Time.wSecond << 7) ^ ((Time.wMinute << 2) & 0xFF00)) + Time.wHour;
	srand(TimeInSecs);

	for (int i = 0; i < 300; i++)
	{
		RandomArray[i] = (rand() >> 7); // Values in the range 0->255. //thank you are ........
	}
	RandomArrayLoc = 0;

	// ----
	// Time been on pitch.
	TimePlayed = 0;
	PrevClock = 0;	// The match code will initialise the timer when the game starts.

	return true;
}


// ****************************************************************************
// ****************************************************************************
// This method is only here cos Anthony is working with Match.cpp
// ****************************************************************************
// ****************************************************************************
#define	STYLUS_CONDUIT	40
void	CMatch::ConvertMouseCoOrdsToCursorMovement()
{
	POINT*			MousePointer;
	POINT			Relative;
	CTeamMember*	TMRef			= NULL;
	int i,j;

	if (	(ControllerRef)
		&&	(GameWindowRef) 
		&&	(GameDataRef))
	{
		MousePointer = GameWindowRef->GetMousePoint();
		
		// Check that the mouse is actually on the screen.
		if (	(MousePointer->x == -1)
			||	(MousePointer->y == -1))
			return;

		switch (GameDataRef->GetPreferences()->StylusControl)
		{
		case STYLUS_CONTROL_NONE:
			break;	// No stylus control.

		case STYLUS_CONTROL_SCREEN:
			// In this mode the mouse co ords are relative to the centre of the screen.

			TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetControlledTeamMember();
			if (TMRef)
			{
				Relative.x = GraphicsObjectRef->GetDisplay()->cxWidth >> 1;//CMatch::PitchXToScreenX(TMRef->getXPos());
				Relative.y = GraphicsObjectRef->GetDisplay()->cyHeight >> 1;//CMatch::PitchYToScreenY(TMRef->getYPos());

				CMCOTCMSlave(&Relative, MousePointer, STYLUS_CONDUIT);
			}
			break;

		case STYLUS_CONTROL_PLAYER:
			// Get the human teams controlled player.
			TMRef = GameDataRef->GetActiveTeamByIndex(0)->GetControlledTeamMember();
			if (TMRef)
			{
				Relative.x = CMatch::PitchXToScreenX(TMRef->getXPos());
				Relative.y = CMatch::PitchYToScreenY(TMRef->getYPos());

				CMCOTCMSlave(&Relative, MousePointer, STYLUS_CONDUIT >> 1);
			}
			break;

		case STYLUS_CONTROL_DPAD_R:
		case STYLUS_CONTROL_DPAD_L:
			// These values are defined in MatchRender!!!

			if (GameDataRef->GetPreferences()->StylusControl == STYLUS_CONTROL_DPAD_R)
			{
				Relative.x = DPAD_POS_X_R + (DPAD_WIDTH >> 1);
			}
			else
			{
				Relative.x = DPAD_POS_X_L + (DPAD_WIDTH >> 1);
			}
			Relative.y = DPAD_POS_Y + (DPAD_HEIGHT >> 1);

			i = Relative.x - MousePointer->x;
			i = i * i;
			
			j = Relative.y - MousePointer->y;
			j = j * j;

			if ((i + j) < ((DPAD_HEIGHT >> 1) * (DPAD_HEIGHT >> 1)))
			{
				CMCOTCMSlave(&Relative, MousePointer, 6);
			}
			break;

		default:
			break;
		}
	}
}

void	CMatch::CMCOTCMSlave(POINT* _Start, POINT* _End, int _Conduit)
{
	if (_End->y < (_Start->y - _Conduit))
	{
		// Up
		if (_End->x < (_Start->x - _Conduit))
		{
			// Up-left
			ControllerRef->SetButton(Up);
			ControllerRef->SetButton(Left);
		}
		else
		if (_End->x > (_Start->x + _Conduit))
		{
			// Up-right
			ControllerRef->SetButton(Up);
			ControllerRef->SetButton(Right);
		}
		else
		{
			// Up.
			ControllerRef->SetButton(Up);
		}
	}
	else
	if (_End->y > (_Start->y + _Conduit))
	{
		// Down
		if (_End->x < (_Start->x - _Conduit))
		{
			// Down-left
			ControllerRef->SetButton(Down);
			ControllerRef->SetButton(Left);
		}
		else
		if (_End->x > (_Start->x + _Conduit))
		{
			// Down-right
			ControllerRef->SetButton(Down);
			ControllerRef->SetButton(Right);
		}
		else
		{
			// Down
			ControllerRef->SetButton(Down);
		}
	}
	else
	{
		// No vertical
		if (_End->x < (_Start->x - _Conduit))
		{
			// Left
			ControllerRef->SetButton(Left);
		}
		else
		if (_End->x > (_Start->x + _Conduit))
		{
			// Right
			ControllerRef->SetButton(Right);
		}
	}
}
