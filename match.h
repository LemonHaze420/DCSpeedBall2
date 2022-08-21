#ifndef _MATCH_H_
#define _MATCH_H_

#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"SoundSystem.h"
#include	"Platform.h"

#include	"Actor_Ball.cpp"
#include	"Actor_Chute.cpp"
#include	"Actor_Dome.cpp"
#include	"Actor_Extra.cpp"
#include	"Actor_shroom.cpp"
#include	"Actor_StarBank.cpp"
#include	"Actor_Token.cpp"


// Macros to ease the playing of sounds
#define	PLAY_ARGGH			SAFELY_CALL(SoundSystemRef, PlaySample(TheArgghSample));
#define	PLAY_AIRHORN		SAFELY_CALL(SoundSystemRef, PlaySample(TheAirHornSample));
#define	PLAY_AIRHORN_HALF	SAFELY_CALL(SoundSystemRef, PlaySample(TheAirHornSample, 2, 0.0f, true));
#define	PLAY_AIRHORN_END	SAFELY_CALL(SoundSystemRef, PlaySample(TheAirHornSample, 3, 0.0f, true));
#define	PLAY_BELL			{SAFELY_CALL(SoundSystemRef, PlaySample(TheBellSample));}
#define	PLAY_BOUNCE			SAFELY_CALL(SoundSystemRef, PlaySample(TheBounceSample));
#define	PLAY_CROWD			SAFELY_CALL(SoundSystemRef, PlaySample(TheCrowdSample));
#define PLAY_CROWD_LOOP		SAFELY_CALL(SoundSystemRef, PlaySample(TheCrowdSample, -1, 0.0f));
#define	PLAY_CROWD2			SAFELY_CALL(SoundSystemRef, PlaySample(TheCrowd2Sample));
#define	PLAY_ELECTRO		SAFELY_CALL(SoundSystemRef, PlaySample(TheElectroSample));
#define	PLAY_GET			SAFELY_CALL(SoundSystemRef, PlaySample(TheGetSample));
#define	PLAY_OOF			SAFELY_CALL(SoundSystemRef, PlaySample(TheOofSample));
#define	PLAY_PEOW			SAFELY_CALL(SoundSystemRef, PlaySample(ThePeowSample));
#define	PLAY_PUNCH			SAFELY_CALL(SoundSystemRef, PlaySample(ThePunchSample));
#define	PLAY_READY			SAFELY_CALL(SoundSystemRef, PlaySample(TheReadySample));
#define	PLAY_REPLAY			SAFELY_CALL(SoundSystemRef, PlaySample(TheReplaySample));
#define	PLAY_TELEPORT		SAFELY_CALL(SoundSystemRef, PlaySample(TheTeleportSample));
#define	PLAY_THROW			SAFELY_CALL(SoundSystemRef, PlaySample(TheThrowSample));
#define	PLAY_THUNDER		SAFELY_CALL(SoundSystemRef, PlaySample(TheThunderSample));
#define	PLAY_TUMBLE			SAFELY_CALL(SoundSystemRef, PlaySample(TheTumbleSample));
#define	PLAY_WALL			SAFELY_CALL(SoundSystemRef, PlaySample(TheWallSample));
#define	PLAY_STAR			SAFELY_CALL(SoundSystemRef, PlaySample(TheStarSample));
#define	PLAY_POWERUP		SAFELY_CALL(SoundSystemRef, PlaySample(ThePowerupSample));

#define	ICECREAM_SAMPLE			"icecream.uwd"
#define	SIREN_SAMPLE			"siren.uwd"
#define	LAUNCH_SAMPLE			"launchball.uwd"
#define	ALLSTAR_SAMPLE			"allstars.uwd"

// These samples are free'd dynamically to keep memory overheads down.
#define	PLAY_SIREN															\
{																			\
	if (!TheSirenSample)													\
	{																		\
		NEW(TheSirenSample, CSample());										\
		TheSirenSample->Init(PlatformRef, &CString(SIREN_SAMPLE));			\
	}																		\
																			\
	if (SoundSystemRef && TheSirenSample)									\
		SoundSystemRef->PlaySample(TheSirenSample, 1, 0.0f, true);			\
}			

#define	PLAY_ICECREAM														\
{																			\
	if (!TheIcecreamSample)													\
	{																		\
		NEW(TheIcecreamSample, CSample());									\
		TheIcecreamSample->Init(PlatformRef, &CString(ICECREAM_SAMPLE));	\
	}																		\
																			\
	if (SoundSystemRef && TheIcecreamSample)								\
		SoundSystemRef->PlaySample(TheIcecreamSample, 2, 0.4f);				\
}

#define	PLAY_BALL_LAUNCH													\
{																			\
	if (!TheBallLaunchSample)												\
	{																		\
		NEW(TheBallLaunchSample, CSample());								\
		TheBallLaunchSample->Init(PlatformRef, &CString(LAUNCH_SAMPLE));	\
	}																		\
																			\
	if (SoundSystemRef && TheBallLaunchSample)								\
		SoundSystemRef->PlaySample(TheBallLaunchSample,1,0.0f, true);					\
}

#define	PLAY_ALLSTARS														\
{																			\
	if (!TheAllStarSample)													\
	{																		\
		NEW(TheAllStarSample, CSample());									\
		TheAllStarSample->Init(PlatformRef, &CString(ALLSTAR_SAMPLE));		\
	}																		\
																			\
	if (SoundSystemRef && TheAllStarSample)									\
		SoundSystemRef->PlaySample(TheAllStarSample, 1, 0.0f, true);		\
}

//	SAFELY_DELETE(TheAllStarSample);
//	NEW(TheAllStarSample, CSample());
//	SAFELY_CONTROL(if, TheAllStarSample, Init(PlatformRef, &CString(ALLSTAR_SAMPLE)), ==, false)
//		return false;


#define	POPUP_IMAGE			"popups"
#define	DPAD_IMAGE			"dpad"

// DPAD defines, used in both MatchRender and MatchConst...
#define	DPAD_LEFT			0
#define	DPAD_TOP			0
#define	DPAD_WIDTH			48
#define	DPAD_HEIGHT			48

#define	DPAD_POS_X_R		(GraphicsObjectRef->GetDisplay()->cxWidth - DPAD_WIDTH - 16)
#define	DPAD_POS_X_L		16
#define	DPAD_POS_Y			(GraphicsObjectRef->GetDisplay()->cyHeight - DPAD_HEIGHT - 42)

// Stores the x, y, and frame information for a rendered item
struct _ReplayXYF
{
	int	X;
	int	Y;
	int Frame;
};

struct _ReplayXYFS
{
	int		X;
	int		Y;
	int		Frame;
	bool	Side;
};

// Stores info needed to replay starbank
struct _ReplayStarBank
{
	bool	Owner;
	char	BitPattern;
};

// Info needed per frame
struct _ReplayFrame
{
	int				CameraX;
	int				CameraY;
	_ReplayXYF		Ball;				// Ball render pos and frame.
	_ReplayXYFS		Players[18];		// A record of the sorted player list!!!
	//_ReplayXYFS		PlayerCaptions[18];	// Letters / symbols above peoples' heads.
	_ReplayXYF		Tokens[MAX_TOKENS];	// Token record
	_ReplayStarBank	StarBank[2];
	int				PitchItemsBitPattern;
};

#define	TOP_SHROOM_BIT_MASK			0x000001	// 000000000001
#define	BOTTOM_SHROOM_BIT_MASK		0x000002	// 000000000010
#define	LEFT_DOME_BIT_MASK			0x000004	// 000000000100
#define	RIGHT_DOME_BIT_MASK			0x000008	// 000000001000
#define	CHUTE_BIT_MASK				0x000070	// 000001110000

enum EMatchState
{
	StartingMatch	= 0,
	LaunchingBall,
	PlayingGame,
	DoingGoalSideA,
	DoingGoalSideB,
	DoingHomeGoalSideA,
	DoingHomeGoalSideB,
	DoingInjury,
	DoingWinA,
	DoingWinB,
	DoingDraw,
	Dancing,
	Paused,
	// More later...

	MatchAbandoned,
	MatchOver,

	MatchError		= -1
};


// For use by render functions.
#define	PITCH_TILE_WIDTH			40
#define PITCH_TILE_HEIGHT			72
									
#define TILE_WIDTH					16
#define TILE_HEIGHT					16
#define	TILE_WIDTH_SHIFT			4
#define	TILE_HEIGHT_SHIFT			4
#define Y_TILE_PITCH				20

class CMatch : public CObject
{
public:
	CMatch();
	~CMatch();

	// Init the object
	bool	Init(	CPlatform*		_PlatformRef,
					CGameData*		_GameDataRef,
					CGameWindow*	_GameWindowRef,
					CGraphics*		_GraphicsObjectRef,
					CControls*		_ControllerRef,
					CSoundSystem*	_SoundSystemRef);

	// ----
	// Reduce the memory burden.
	void	ConserveMemory(bool _Force = false);

	// ----
	// Process the match state.
	bool	Process();

	// ----
	// Returns true if the game is over.
	bool	IsGameOver();
	bool	IsGameAbandoned();

	// ----
	void	ConvertMouseCoOrdsToCursorMovement();
	void	CMCOTCMSlave(POINT* _Start, POINT* _End, int _Conduit = 40);

	// ----
	int		PitchXToScreenX(int _X) 
	{
		return ((TILE_WIDTH * PITCH_TILE_WIDTH) >> 1) + _X - CameraX;
	}

	// ----
	int		PitchYToScreenY(int _Y) 
	{
		return ((TILE_HEIGHT * PITCH_TILE_HEIGHT) >> 1) - _Y - CameraY;
	}

	//*************************************
	//variables
	//*************************************
#if defined(NOT_FINAL_BUILD) || defined(SHOW_FPS)
public:
	float	FrameTime;
#endif
protected:
	
	// ----
	CPlatform		*PlatformRef;
	CGameData		*GameDataRef;
	CGameWindow		*GameWindowRef;
	CGraphics		*GraphicsObjectRef;
	CControls		*ControllerRef;
	CSoundSystem	*SoundSystemRef;
	
	// Graphics data.
	CGraphicsData	*ThePitchGraphic;
	CGraphicsData	*TheBallGraphic;
	CGraphicsData	*TheBluePlayerGraphic;
	CGraphicsData	*TheRedPlayerGraphic;
	CGraphicsData	*ThePickupGraphic;
	CGraphicsData	*TheMedicGraphic;
	CGraphicsData	*TheLauncherGraphic;
	CGraphicsData	*TheAddOnsGraphic;
	CGraphicsData	*TheChuteGraphic;
	CGraphicsData	*ThePopUpGraphic;
	CGraphicsData	*TheInfoBarGraphic;
	CGraphicsData	*TheDPadGraphic;
	CGraphicsData	*TheMarkerGraphic;
	
	// Fonts
	CFont			*ThePopUpFont;
	CFont			*TheInfoBarFont;
	CFont			*TheMainMenuFont;

	// Samples
	CSample			*TheArgghSample;
	CSample			*TheAirHornSample;
	CSample			*TheBellSample;
	CSample			*TheBounceSample;
	CSample			*TheCrowdSample;
	CSample			*TheCrowd2Sample;
	CSample			*TheElectroSample;
	CSample			*TheGetSample;
	CSample			*TheIcecreamSample;
	CSample			*TheOofSample;
	CSample			*ThePeowSample;
	CSample			*ThePunchSample;
	CSample			*TheReadySample;
	CSample			*TheReplaySample;
	CSample			*TheTeleportSample;
	CSample			*TheThrowSample;
	CSample			*TheThunderSample;
	CSample			*TheTumbleSample;
	CSample			*TheWallSample;
	CSample			*TheStarSample;
	CSample			*TheAllStarSample;
	CSample			*TheSirenSample;
	CSample			*ThePowerupSample;
	CSample			*TheBallLaunchSample;




	WORD			PitchMap[PITCH_TILE_HEIGHT][PITCH_TILE_WIDTH];
	int				CameraX, CameraY;

	// State info
	EMatchState		MatchState;
	
	// Anthony's bits...
	bool			CallMedics; //I can't remember for the life of me why this was put here!
	CBall			*TheBall; //pointer to the ball
	int				StartPauseCount; //the pause before you can move at the start
	int				RandomArray[300]; //an array of random numbers
	CToken			*TokenArray[MAX_TOKENS]; //an array of random tokens
	int				RandomArrayLoc; //a pointer to the posiiton we are in in the random number array
	int				TeamOneHalf; //i.e. Which Half team One's goal is . 0 = bottom half, 1 = top half;
	CTeamMember		*ClosestPlayer; //ummm... welll..... duurrrrre..(dribble)
	EMatchState		PreviousMatchState; //For when you pause the game it knows where to go back to
	int				OptionSelected;

	//Map Objects. To be inited in the init during init
	CDome			*LeftDome;
	CDome			*RightDome;

	CShroom			*TopShroom;
	CShroom			*BottomShroom;

	CChute			*LeftChute;
	CChute			*RightChute;

	CStarBank		*LeftBank;
	CStarBank		*RightBank;

	CExtra			*ThingA;
	CExtra			*ThingB;


	//multiplyer holders
	float			TeamAMulti;
	float			TeamBMulti;

	int				GameHalf;
	int				ActualHalf;
	int				HalvesToPlay;
	float			TimePlayed;
	DWORD			PrevClock;

	int				TimeCount;
	bool			PlayCrowd;

	bool			HasSetActions;
	int				GoalCount;

	// Replay buffer.
	_ReplayFrame	ReplayBuffer[REPLAY_BUFFER_LENGTH];
	int				ReplayBufferIndex;
	int				ReplayBufferPlaybackIndex;
	int				ReplayTime;

	int				FirstAICount, SecondAICount;

	bool			ProcessWho;
	bool			PlayReplay;
	bool			PlayIcecream;

	int				InjuryCount;

	bool			IsAirhornPlaying;
//******************************************
//	Protected Methods
//******************************************

	// ----
	// Draw everything...
	bool	Draw();

	// ----
	bool	DrawPitch();
	bool	DrawTokens();
	bool	DrawDPad();
	bool	DrawPlayers();
	bool	DrawBall();
	bool	DrawExtras();
	bool	DrawPopUp();
	bool	DrawInfoBar();

	//Draws the confirm/quit dialog over the pause screen
	bool	DrawPauseScreen();

	// Draws all of the above in replay mode.
	bool	DrawReplay();

	// ---- 
	// Returns true if the Actor is within '_Dimension' pixels of the rendered screen area.
	bool	IsActorOnScreen(CActor* _ActorRef, int _Dimension = 48);
	bool	IsActorOnScreen(int x, int y, int _Dimension = 48);

	bool	CheckForGoalDoor(int Goal);

	//Process all players movements
	bool			ProcessMoves();
	//find if the player has collided with anything
	bool			GetCollision(CActor* thing);
	//process ai for when you have the ball
	bool			ProcessTeam(CTeam* theTeam, CTeam* oppTeam);
	//process all the animation
	bool			ProcessAnimation();
	//update ball postion and stuff
	bool			ProcessBall();
	//check to see if a goal has been scored or anything
	bool			CheckFlags();
	//Do stuff with the map tokens.
	bool			ProcessTokens();
	//process all the starts and ramps and things
	bool			ProcessMapObjects();
	//put everyone into the kick off position
	bool			GetEveryoneIntoStartingPositions();
	//get the medics outn coz someones been hurt
	bool			CallTheMedics();
	//find the closest player to the ball
	CTeamMember*	FindClosestPlayer(CTeam* TheTeam);
	//Porcess the ball launch start
	bool			LaunchBall();
	//do all the goal scored stuff
	bool			ProcessGoal(CTeam* TheTeam, bool HomeGoal = false,  CTeam* oppTeam = NULL);
	//process end of match things
	bool			ProcessMatchOver();
	//process all the controls for plyer one
	bool			ProcessControls(CTeamMember* person);
	//work out what the primary computer player is doing
	bool			ProcessMainComputerPlayer(CTeamMember* person);
	//find the closest person on either tem... hah.. has been changed
	CTeamMember*	FindClosestOpponent(CTeamMember* person, CTeam* theTeam);

	//finds the closest token
	CToken*		FindClosestToken(CTeamMember* thePlayer);

	//the next function plot the end of two diagonal lines at 90 degrees from each other
	//coming from the front of the player.

	int				GetLeftDiagX(CTeamMember* person);
	int				GetLeftDiagY(CTeamMember* person);
	int				GetRightDiagX(CTeamMember* person);
	int				GetRightDiagY(CTeamMember* person);

	bool	PredictCatch(CBall* theBall, CTeamMember* thePerson);

	int		GetRandomNumber();

	bool	GoToPosition(CTeamMember* thePlayer, CTeamMember* ControlledPlayer, CTeam* theTeam, CTeam* oppTeam);

	//Computer controlled character methods
	void	ProcessComputer(CTeamMember* thePlayer, CTeam* theTeam, CTeam* oppTeam);

	//works out the distance between any two actor objects
	int	WorkOutDist(CActor* ObjectOne, CActor* ObjectTwo, int endX = -300, int endY = PITCH_BLY);
	
	//finds and returns a vaguely sensible person to pass to
	bool DoPass(CTeamMember* thePlayer, int LineToPassTo = 50);
	//tries to make a shot
	bool DoShot(CTeamMember* thePlayer, CTeam* theTeam, CTeam* oppTeam);
	//runs somewhere else
	bool DoRun(CTeamMember* thePlayer);
	//tries to go for one of the bonus target
	bool DoTarget(CTeamMember* thePlayer);

	//checks to see if there is anyone in thier line of sight
	bool	CheckLOS(int startX, int startY, int xDest, int yDest, CTeamMember* thePlayer = NULL, int distance = 300, CTeam* oppTeam = NULL);

	//checks to see which direction somwthing should travel. either a pass or a run
	int		WorkOutDirection(int startX, int startY, int xDest, int yDest, int Conduit = 15);

	//Fiinds the closest map target
	CActor*	FindClosestTarget(CTeamMember* thePlayer);

	//checks to see where the end of a continuos line will be
	int		GetEndXOfLine(int startX, int startY, int direction);
	int		GetEndYOfLine(int startX, int startY, int direction);
	
	//methods for collisions
	int     BallCollision(CBall* theBall);
	bool	CheckCollision(int startX1, int startY1,int endX1,int endY1,int startX2, int startY2,int endX2,int endY2);
	bool	CheckCollision(CActor* ObjectOne, CActor* ObjectTwo, int widthOne, int widthTwo);

	//computes collision effects between players
	int	PlayerCollision(CTeamMember* thePlayer,CTeam* theTeam,  CTeam* oppTeam);

	bool IsPlayerInZone(CTeamMember* thePlayer);
	int	 WhichZoneIsPlayerIn(CTeamMember* thePlayer);
	void GoToDefaultPosition(CTeamMember* thePlayer);
	void RunToStartingPosition(CTeamMember* temp);

	void ResetRandomNumbers();

	bool GamePause();
};

#endif