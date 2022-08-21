#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

#include	"Object.h"
#include	"GameDefines.h"
#include	"MyString.h"
#include	"Team.cpp"
#include	"BasicTypes.h"
#include	"Platform.h"
#include	"Actor_TeamMember.cpp"

// Preferences
struct _Prefs
{
	int				Volume;					// Master volume. 0 -> mute. 10 -> 100%. 
	bool			DoubleBufferingEnabled;	// Turn on and off double buffering. This might be ignored in favour of a registry value.
	int				StylusControl;			// Value indicates how the stylus is interpreted during game. 0 - ignored. 1 - indicates direction of play relative to centre of screen. 2 - indicates the user is to use a small D-pad graphic on the display.
//	int				AutoSleepTimeOut;		// The length of time the application will run without input. After which time the game will stop processing the game and go into a 'hibernate' mode - (polls for input a few times a second, but doesn't update display). 0 indicates the feature is disabled. Negative values are reserved and will currently be ignored (i.e. interpreted as 0).
	bool			MusicEnabled;			// Play music?
	bool			SlowVRAMWorkAround;		// Quicker to check against a shadow of the display?
	bool			Registered;				// Whether the product is registered
};

#define	SCORE_ENTRIES				128
#define	NUM_OF_STAR_PLAYERS			18
#define	MIN_INITIAL_SALE_PAUSE		0
#define	ALREADY_PURCHASED			10
#define	PLAYER_SALE_TIMEOUT			(-4)			// A player will never be available for more than 4 matches
#define	IS_FOR_SALE(_Value)			((_Value) < 0)

// States for the cup game.
enum	ECupState
{
	ECS_Normal				= 0,	// when things should just continue
	ECS_Drew,						// need to play decider
	ECS_Lost,						// you are the weakest link - goodbye.

	ECS_WonCup,						// show cup.

	ECS_Error				= -1	// when there is an error
};

struct	SaveInfoStruct
{
	CString				FileName;			// File name
	CString				FriendlyName;		// Friendly name for save. (file - ext).
	int					SaveVersion;		// Save version.
	int					GameType;			// Game type
	int					Round;				// League / cup round.
	int					Cash;				// How much cash the player has.
	long				Date;				// Date the game was played.
};

struct	SaveInfoListStruct
{
	SaveInfoStruct		Data;				// Actual save info.

	SaveInfoListStruct	*NextSave;			// Pointer to next save.
	SaveInfoListStruct	*NextCupSave;		// Pointer to next cup save.
	SaveInfoListStruct	*NextLeagueSave;	// Pointer to next league save.
};

#define	FILE_SAVE_VERSION			102

struct SaveTeamStruct
{
	//CNotSoRandomStuff* random;
	int				Cash;

	//Game Stats
	int				NumGamesPlayed;
	int				NumGamesWon;
	int				NumGamesDrawn;
	int				NumGamesLost;
	
	//League Specific Stuff
	int				LeagueTable;	//division 1 or 2
	int				LeaguePlacing;
	int				LeagueMovement; //ie. going up down or steady.
	int				PointsFor;
	int				PointsAgainst;
	int				Points;

	int				Score;
	int				CupScore;		// Stores the combined score for the two cup games.

	//LastMatchStats
	int				ShotsAtGoal;
	int				GoalsScored;
	int				GoalsSaved;
	int				TimeInPossesion;
	int				TimeInOppenentsHalf;
	int				SuccesfulTackles;
	int				Substitutions;
	int				BonusPointsScored;
};

// The following attributes need to be stored for each player (in teams[0] and for all star players).
struct SavePlayerStruct
{
	// Active - if 0 then the players info is to be ignored (this is used to flag a NULL star player).
	int		Active;

	// From CActor...				(cut out in game stuff).
	int		type;				// Look in GameDefines.h for type definitions.

	// From CActor_TeamMember...	(cut out in game stuff).
	TCHAR	Name[16];
	int		Position;
	int		Worth;
				
	int		Agr;
	int		Att;
	int		Def;
	int		Spd;
	int		Thr;
	int		Pow;
	int		Sta;
	int		Int;

	int		PictureNumber;
};

struct SaveStruct
{
	// These five items MUST NOT change position or size!!!
	int					SaveVersion;			// Save file version number
	int					GameType;				// Game type
	int					Round;					// League / cup round.
	int					Cash;					// How much cash the player has.
	long				Date;					// Date the games was played
												
	int					GameLength;				// Length of a game (per side). Default is 90.
	int					GameDifficulty;			// A scale indicating game difficulty. 0 indicates a regular game, negative values easier games and positive values indicate increasingly difficult games. Values must be in the bounds -2 -> 2, inclusive.
						
	// Need entire record of Teams[0]... (other teams can be recreated).
	SaveTeamStruct		Teams[16];			// Team attributes.
	SavePlayerStruct	HumanPlayerArray[12];	// Human player

	// Division - need to store an array of integers representing the team values.
	int					Division[NUM_OF_DIVISIONS][NUM_OF_TEAMS_PER_DIVISION];
	int					SortedDivision[NUM_OF_DIVISIONS][NUM_OF_TEAMS_PER_DIVISION];
						
	int					PlayerOne;				// Integer index of the main team
	int					PlayerTwo;				// Opponent index.
	int					LastTeamPlayed;			// Last team index.
					
	// Knockout
	int					KnockOutOpponent;		// Counter as to how many opponents you have played in knock out mode.
	int					LeagueRound;			// Number of rounds played in a League style game.
	bool				LeagueHumanPlayOff;		// Is the game in league playoff mode.
	
	// Fixtures info.
	IntCouple			LeagueFixtures[NUM_OF_TEAMS_PER_DIVISION - 1][NUM_OF_TEAMS_PER_DIVISION >> 1];
	int					LeagueFixturesOrder[(NUM_OF_TEAMS_PER_DIVISION - 1) << 1];

	// Store cup meetings.
	IntCouple			CupFixtures[NUM_OF_CUP_ROUNDS][MAX_CUP_FIXTURES];
	int					CupRound;
	int					CupLeg;
	int					CupState;

	// -1 player already owned
	// 0 player for sale
	// >0 sale time out. Reduces at the rate of 1 per meeting.
	int					StarPlayersState[NUM_OF_STAR_PLAYERS];
	SavePlayerStruct	StarPlayersArray[NUM_OF_STAR_PLAYERS];
};

// ----
// Stores game data. 
class CGameData : public CObject
{
public:
	CGameData();
	~CGameData();

	// ----
	// GENERAL METHODS

	// ----
	// Init the game, create all the teams.
	bool		Init(int _GameType, CPlatform*	_PlatformRef);

	// ----
	// Returns false if one of the games required files is missing.
	bool		ValidateFiles();

	// ----
	// Per game instances
	int			GetGameDuration()	{return GameLength;}
	int			GetGameDifficulty()	{return GameDifficulty;}

	// ----
	// Overall game settings (use these in FrontEnd).
	int			GetDefaultGameDuration()			{return DefaultLength;}
	int			GetDefaultGameDifficulty()			{return DefaultDifficulty;}
	void		SetDefaultGameDuration(int _a);
	void		SetDefaultGameDifficulty(int _a);

	// ----
	// Return a pointer to the save name string - you can 
	// set the string using the return value. 
	// eg. *(GameDataRef->GetSaveNamePointer()) = "Bob";
	CString*	GetSaveNamePointer()	{return &SaveName;}

	// ----
	// Returns a pointer to a structure holding the player's preferences.
	_Prefs*		GetPreferences();

	// ----
	// Return registration code
	unsigned long	GetRegistrationCode();
	void			SetRegistrationCode(unsigned long code);

	// ----
	// Returns true to indicate that saves were found. (I'll use this Anthony).
	bool			CatSaveFiles();
protected:
	void			CSFSlave(WIN32_FIND_DATA	*_FoundFile);
public:

	// ----
	// Frees data created by 'CatSaveFiles'
	void			FreeSaveCache();

	// ----
	// Returns the number of files of a particular type.
	unsigned int	GetNumberOfSaves();
	unsigned int	GetNumOfCupSaves();
	unsigned int	GetNumOfLeagueSaves();

	// ----
	// Returns true if the _Index is in bounds 
	// ((_Index >= 0) && (_Index < GetNumberOfSaves())) and
	// fills in the provided structure. A return of false 
	// will indicate that the _Index is an illegal value 
	// and that struct will not contain valid info.
	bool			GetSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo);
	
	// ----
	bool			GetSaveLCInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo);

	// ----
	// As above, but filter games according to type.
	bool			GetCupSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo);
	bool			GetLeagueSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo);


	// ----
	// Loads a game
	// True means that the game loaded successfully.
	bool			LoadGame(unsigned int _Index);
	bool			LoadCupGame(unsigned int _Index);
	bool			LoadLeagueGame(unsigned int _Index);


	// ----
	// Checks to see if a file with the given name exists. 
	// '_SaveName' should not contain a file extension or 
	// path information. 
	bool			DoesSaveExist(CString*	_SaveName);

	// ----
	// Saves the current data to the given file.
	// '_SaveName' should not contain a file extension or 
	// path information. 
	// 
	// WARNING:	Overwrites files without warning!!!
	bool			SaveGame(CString*	_SaveName);

	// ----
	// Returns a handle to the indexed star player.
	CTeamMember*	GetStarPlayer(int _Index);

	// ----
	// Determines which players the team can purchase.
	// Call once straight after playing a match.
	void		ProcessStarPlayers();

	// ----
	// Purchase a selected star player
	// SelectedPlayer is the star player index.
	// SelectedPosition is the person they replace.
	bool		Purchase(int _SelectedPlayer, int _SelectedPosition);

	// ----
	// Returns the index of the next available star player
	// Returns -1 if no players available.
	int			GetNextStarPlayerIndex(int _Index = -1);
	
	// ----
	// Returns the index of the previous available star player
	// Returns -1 if no players available.
	int			GetPreviousStarPlayerIndex(int _Index = -1);

	// ----
	// Loads the preference data.
	bool		LoadPreferences(CPlatform* _PlatformRef);

	// ----
	// Saves preference data.
	bool		SavePreferences();

	// ----
	// Currently _PlayerIndex is ignored, and must be set to 0.
	//CString*	GetPlayerName(int _PlayerIndex = 0);		

	// ----
	// Returns an integer that one should compare with given values in "GameDefines.h".
	int			GetGameType() {return GameType;}


	// ----
	// Ensure the order of meeting is random.
	void		RandomiseLeagueFixturesOrder();

	// ----
	// Returns whether then game is a home or away game
	bool		IsHomeGame();
	bool		IsAwayGame() {return (!IsHomeGame());}

	// ----
	// Basically, returns the teams that will be playing in the match.
	// The return value can be NULL.
	// 0 for human's team
	// 1 for cpu
	CTeam*		GetActiveTeamByIndex(int _Index = 0);

	// ----
	// Returns a team according to position in the underlying array structure.
	// You should not really need this method too much.
	CTeam*		GetTeamByIndex(int _Index);

	// ----
	//bool		LoadGame(CString*	_FileName);


	CTeam*		GetLastTeamPlayed() {return LastTeamPlayed;}
	// ----
	// KNOCKOUT
	// Progress to next team, etc.
	bool		UpdateKnockOut();	
	bool		KnockOutScore();


	// ----
	// LEAGUE
	// League related methods.
	bool		SortLeagues();								// Sorts leagues into decending order, according to points. 
	bool		UpdateLeagueState();						// Add scores, etc.
	bool		IsLeagueOver();								// True if there are no more games to play.
	bool		DidHumanWinLeague();						// True if the above is true and the human team has the most points in the top division.
	bool		DidHumanGetPromotion();						// True if the human is automatically promoted (don't use this in playoff).
	bool		DidHumanGetRelegated();						// True if the human is automatically relegated (don't use this in playoff).

	// ----
	// Promotion / league playoff.
	void		PrepareForLeaguePlayOff();					// Call this when the league finishes. (Only the once).
	bool		IsHumanInLeaguePlayOff();					// Call after calling the above.
	void		AssignPlayOffTeams();						// Call if 'IsHumanInLeaguePlayOff' returned true.
protected:
	void		PromoteRelegateNonplayOffTeams();			// Called internally to swap top and bottom teams.
	void		SwapPlayOffTeams();							// Called internally to swap the teams that had to playoff.
public:
	void		ResolveCPUOnlyPlayOff();					// Call if 'IsHumanInLeaguePlayOff' returned false.
	bool		ResolveHumanPlayOff();						// Call on all subsequent loops. Returns true if it is time to continue.

	void		NewLeagueSeason();							// Reset all the required data.

	// Tells me if the game is in LeagueHumanPlayOff mode.
	bool		IsThisALeagueHumanPlayOff()	{return LeagueHumanPlayOff;}
	void		SetLeagueHumanPlayOff(bool _a) {LeagueHumanPlayOff = _a;}

	bool		LeagueGenerateScores();						// Generate scores for cpu v cpu games.

	// ----
	// Get a team from a division (the array (will be) 
	// sorted by points).
	// 
	// NOTE:	By default the human player is returned. 
	//			However, if the user specifies a value for
	//			_Index then the particular team FROM THE 
	//			HUMAN PLAYER's division will be returned. 
	//			Specifying a _Division along with the 
	//			_Index allows access to all the teams.
	// NOTE:	Special values for _Index.
	//			-1	will always be the human player's team.
	//			-2	will always return the handle to the 
	//				current opponents
	//			-3	will return the 'LastTeamPlayed', 
	//				if there was one.
	// NOTE:	Special values for _Division
	//			-1	will select a team from the human 
	//				player's division.
	//			
	// WARNING:	All other negative values are reserved and
	//			will result in a NULL return value, as will
	//			any other out of bound value.
	#define	GET_HUMAN_TEAM						(-1)
	#define GET_CURRENT_OPPONENT				(-2)
	#define	GET_LAST_OPPONENTS					(-3)
	#define	SELECT_FROM_HUMAN_PLAYERS_DIVISION	(-1)
	CTeam*		GetTeamFromDivision(int _Index		= GET_HUMAN_TEAM, 
									int _Division	= SELECT_FROM_HUMAN_PLAYERS_DIVISION);

	// ----
	// Returns teams in the order they appear if the league
	// is sorted - call SortLeague before using this method
	// to ensure that the league is actually sorted.
	// 
	// NOTE:	This method does not support any of the 
	//			special values for _Index. It does, 
	//			however, support the 
	//			SELECT_FROM_HUMAN_PLAYERS_DIVISION value 
	//			for _Division.
	CTeam*		GetTeamFromSortedDivision(	int _Index, 
											int _Division	= SELECT_FROM_HUMAN_PLAYERS_DIVISION);

	// ----
	// Fills out the _OpposingTeams with the number of the teams playing one another.
	void		GetFixture(IntCouple* _OpposingTeams, int _MatchIndex);//, int _Division = SELECT_FROM_HUMAN_PLAYERS_DIVISION);


	// ----
	// CUP
	// Cup relayed methods.
	bool		CupGenerateScores();
	void		UpdateCupState();	

	int			GetCupLeg()								{return CupLeg;}
	int			GetCupRound()							{return CupRound;}
	ECupState	GetCupState()							{return CupState;}
	IntCouple*	GetCupDraw(int _Round, int _Meeting)	{return &CupFixtures[_Round][_Meeting];}

	// ----
	int			GetDemoTimeOut()		{return ShowDemoTimeOut;}
	void		SetDemoTimeOut(int a);

//protected:
public:
	// 
	CPlatform			*PlatformRef;

	// General application preferences
	_Prefs				Preferences;

	// Save info.
	unsigned int		NumberOfSaves;
	unsigned int		NumberOfCupSaves;
	unsigned int		NumberOfLeagueSaves;

	SaveInfoListStruct	*FirstSave;
	SaveInfoListStruct	*FirstCupSave;
	SaveInfoListStruct	*FirstLeagueSave;


	// Game specific preferences.
	CString				SaveName;
public:	// For the sake of a hack.
	int					GameType;				// Type of game. See values defined in "GameDefines.h"
protected:
	int					GameLength;				// Length of a game (per side). Default is 90.
	int					GameDifficulty;			// A scale indicating game difficulty. 0 indicates a regular game, negative values easier games and positive values indicate increasingly difficult games. Values must be in the bounds -2 -> 2, inclusive.

	// Registration code
	unsigned long		RegistrationCode;

	// FrontEnd specific.
	int					ShowDemoTimeOut;

	// These are the values set by the user in the frontend
	// - they are store in the registry and copied into a 
	// game when it is init'ed.
	int					DefaultDifficulty;		
	int					DefaultLength;			


	CTeam*				Teams[NUM_OF_DIVISIONS * NUM_OF_TEAMS_PER_DIVISION];			// Contains all the teams.
	CTeam*				Division[NUM_OF_DIVISIONS][NUM_OF_TEAMS_PER_DIVISION];			// References the teams in leagues.
	CTeam*				SortedDivision[NUM_OF_DIVISIONS][NUM_OF_TEAMS_PER_DIVISION];	// References the teams in leagues (in a sorted form).
						
	CTeam*				PlayerOne;				// Pointer to one of the Teams elements. Generally references the human team (but not always - demo).
	CTeam*				PlayerTwo;				// Similar to PlayerOne, however, it generally references the opponent.
	CTeam*				LastTeamPlayed;			// The last team played by PlayerOne. Only set in certain game modes, where it is important to remember who you just played.
						
	int					KnockOutOpponent;		// Counter as to how many opponents you have played in knock out mode.
	int					LeagueRound;			// Number of rounds played in a League style game.
	
	// Scores
	unsigned char		ScoreData[NUM_OF_TEAMS_PER_DIVISION * NUM_OF_TEAMS_PER_DIVISION * SCORE_ENTRIES];

	// Fixtures info.
	// Assumes that NUM_OF_TEAMS_PER_DIVISION is even!!!
	IntCouple			LeagueFixtures[NUM_OF_TEAMS_PER_DIVISION - 1][NUM_OF_TEAMS_PER_DIVISION >> 1];
	int					LeagueFixturesOrder[(NUM_OF_TEAMS_PER_DIVISION - 1) << 1];
	bool				LeagueHumanPlayOff;

	// Store cup meetings.
	IntCouple			CupFixtures[NUM_OF_CUP_ROUNDS][MAX_CUP_FIXTURES];
	int					CupRound;
	int					CupLeg;
	ECupState			CupState;

	// -1 player already owned
	// 0 player for sale
	// >0 sale time out. Reduces at the rate of 1 per meeting.
	int					StarPlayersState[NUM_OF_STAR_PLAYERS];
	CTeamMember*		StarPlayersArray[NUM_OF_STAR_PLAYERS];	// Pointers to star player objects. 
};

#endif