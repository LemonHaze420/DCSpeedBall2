#include	"SafeProgramming.h"
#include	"GameData.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"MyString.h"
#include	"RegistryFunctions.h"
#include	"Platform.h"
#include	"Misc.h"

#define	SCORE_DATA				"scores.dat"

// ----
CGameData::CGameData()
{
	int i;

	GameType			= PRACTICE;
	//PlayerName[0]		= DEFAULT_PLAYER_NAME;
	
	for (i = (NUM_OF_DIVISIONS * NUM_OF_TEAMS_PER_DIVISION); i--;)
	{
		Teams[i]		= NULL;
	}

	for (i = NUM_OF_STAR_PLAYERS; i--;)
	{
		StarPlayersState[i] = MIN_INITIAL_SALE_PAUSE;
		StarPlayersArray[i] = NULL;
	}

	PlayerOne			= NULL;
	PlayerTwo			= NULL;
	LastTeamPlayed		= NULL;

	// Save info
	NumberOfSaves		= 0;
	NumberOfCupSaves	= 0;
	NumberOfLeagueSaves	= 0;

	FirstSave			= NULL;
	FirstCupSave		= NULL;
	FirstLeagueSave		= NULL;
}


// ----
CGameData::~CGameData()
{
	int i;
	for (i = (NUM_OF_DIVISIONS * NUM_OF_TEAMS_PER_DIVISION); i--;)
	{
		SAFELY_DELETE(Teams[i]);
	}

	for (i = NUM_OF_STAR_PLAYERS; i--;)
	{
		SAFELY_DELETE(StarPlayersArray[i]);
	}

	// Free cached save data
	FreeSaveCache();

	PlayerOne			= NULL;
	PlayerTwo			= NULL;
	LastTeamPlayed		= NULL;
}


// ----
// Returns false if one of the games required files is missing.
bool		CGameData::ValidateFiles()
{
	// Must have Platform object
	if (!PlatformRef)
		return false;

	bool	ret = true;
	CString	TempString;

	// The content of "Data"
	TempString = PlatformRef->GetPath(EP_Data); TempString += "map.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Data); TempString += "scores.dat";
	ret &= TestReadFile(&TempString);

	// Audio
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "airhorn.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "allstars.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "arggh.uwd";
	ret &= TestReadFile(&TempString);
	//TempString = PlatformRef->GetPath(EP_AudioData); TempString += "backup.uwd";
	//ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "bell.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "bounce.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "collectpowerup.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "crowd.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "crowd2.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "get.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "icecream.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "launchball.uwd";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "menumovement.uwd";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "oof.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "peow.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "punch.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "ready.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "replay.uwd";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "select.uwd";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "siren.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "star.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "teleport.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "throw.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "thunder.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "tumble.uwd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "wall.uwd";
	ret &= TestReadFile(&TempString);

	// Music file
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_AudioData); TempString += "music.umd";
	ret &= TestReadFile(&TempString);
#endif
	// Fonts
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "bottomsmallfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "bottomsmallfont.dat";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "infobarfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "infobarfont.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "mainmenufont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "mainmenufont.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "popupfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "popupfont.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "regfont.dat";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "smalldarkfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "smalldarkfont.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "smalllightfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "smalllightfont.dat";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "topsmallfont.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Fonts); TempString += "topsmallfont.dat";
	ret &= TestReadFile(&TempString);
#endif

	// Images
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "addons.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "armour.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "ball.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "bblogo.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "blue.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "bonushelp.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
#if 1
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "bslogo.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "chute.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "cups.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "dpad.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "faces.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "gym.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "helppickup.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "infobar.cbd.gz";
	ret &= TestReadFile(&TempString);
#if 1
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "ivlogo.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "keyboard.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "keys.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "launcher.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "loadoverlay.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "lose.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "manager.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "markers.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "medic.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "menubg.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "pickup.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "pitch.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "pitchhelp.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "popups.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "promotion.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "red.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "shield.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "shieldletters.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "splash.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "stadpic.cbd.gz";
	ret &= TestReadFile(&TempString);
#ifndef DEMO_BUILD
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "win.cbd.gz";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_ImageData); TempString += "won.cbd.gz";
	ret &= TestReadFile(&TempString);
#endif
#ifndef DEMO_BUILD
	// Text
	TempString = PlatformRef->GetPath(EP_Text); TempString += "bonus.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "controls.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "db.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "modes.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "modes1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "modes2.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "modes3.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen1-1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen1-2.ctd";
	ret &= TestReadFile(&TempString);
	//TempString = PlatformRef->GetPath(EP_Text); TempString += "screen1.ctd";
	//ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen2-1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen2-2.ctd";
	ret &= TestReadFile(&TempString);
	//TempString = PlatformRef->GetPath(EP_Text); TempString += "screen2.ctd";
	//ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen3-1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen3-2.ctd";
	ret &= TestReadFile(&TempString);
	//TempString = PlatformRef->GetPath(EP_Text); TempString += "screen3.ctd";
	//ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen4-1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "screen4-2.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "story.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "stylus0.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "stylus1.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "stylus2.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "stylus3.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "stylus4.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "intro.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token0.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token10.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token11.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token12.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token13.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token14.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token15.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token16.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token17.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token18.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token19.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token2.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token20.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token3.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token4.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token44.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token5.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token6.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token7.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token8.ctd";
	ret &= TestReadFile(&TempString);
	TempString = PlatformRef->GetPath(EP_Text); TempString += "token9.ctd";
	ret &= TestReadFile(&TempString);
#endif
	return (ret);
}

// ----
bool	CGameData::Init(int _GameType, CPlatform*	_PlatformRef)
{
	// Check that the game type is valid. 
	if (	(_GameType != PRACTICE) 
		&&	(_GameType != LEAGUE) 
		&&	(_GameType != CUP) 
		&&	(_GameType != KNOCKOUT)
		&&	(_GameType != DEMO)
		&&	(_GameType != LEAGUE_TEAM_MANAGER))
		return false;	// Failed. 

	// are: Right! Always create all 16 teams in one array, 
	//		then arrange data in a variety of ways. 

	// Seed the random number system.
	SYSTEMTIME	Time;
	int			i, j;
	int			RandTeamA;
	int			RandTeamB;
	int			TimeInSecs = 0;

	PlatformRef = _PlatformRef;

	// Clear save name
	SaveName.Clear();

	// Build a list of save files.
	//SAFELY_CALL(SoundSystemRef, Process());
	CatSaveFiles();

	// Seed random.
	GetSystemTime(&Time);	
	//TimeInSecs = ((Time.wDay			* 60 * 60 * 24));
	//TimeInSecs += ((Time.wHour			* 60 * 60));
	//TimeInSecs += ((Time.wMinute		* 60));
	TimeInSecs = ((Time.wSecond << 7) ^ ((Time.wMinute << 2) & 0xFF00)) + Time.wHour;
	srand(TimeInSecs);

	// Assign difficulty
	GameLength			= DefaultLength;
	GameDifficulty		= DefaultDifficulty;

	CString FilePath = _PlatformRef->GetPath(EP_Data);
	FilePath += SCORE_DATA;
#ifdef _UNICODE
	FILE* FH = _wfopen(FilePath.GetString(), TEXT("rb"));
#else
	FILE* FH = fopen(FilePath.GetString(), TEXT("rb"));
#endif
	if (FH)
	{
		if (fread((void*)ScoreData, NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS * SCORE_ENTRIES, 1, FH) != 1)
		{
			fclose(FH);
			return false;
		}
		fclose(FH);
	}
	else
		return false;


	/*
	1,2,3,4,5,6,7,8
	1,4,2,3,5,8,6,7
	1,8,2,7,3,6,4,5
	1,3,2,4,5,7,6,8
	1,5,2,6,3,7,4,8
	1,7,2,8,3,5,4,6
	1,6,2,5,3,8,4,7
	*/

	// The "LeagueFixtures" array determines which teams 
	// are to face one another in a "meeting". By "meeting"
	// I mean all the games are played at the same time.
	// (These teams will play on wednesday, etc).
	// 
	//					Meeting
	//				  /		Game
	//				 /	 /	Side
	//				/	/ /		 
	LeagueFixtures[0][0].A	= 0;
	LeagueFixtures[0][0].B	= 1;
	LeagueFixtures[0][1].A	= 2;
	LeagueFixtures[0][1].B	= 3;
	LeagueFixtures[0][2].A	= 4;
	LeagueFixtures[0][2].B	= 5;
	LeagueFixtures[0][3].A	= 6;
	LeagueFixtures[0][3].B	= 7;

	LeagueFixtures[1][0].A	= 0;
	LeagueFixtures[1][0].B	= 3;
	LeagueFixtures[1][1].A	= 1;
	LeagueFixtures[1][1].B	= 2;
	LeagueFixtures[1][2].A	= 4;
	LeagueFixtures[1][2].B	= 7;
	LeagueFixtures[1][3].A	= 5;
	LeagueFixtures[1][3].B	= 6;

	LeagueFixtures[2][0].A	= 0;
	LeagueFixtures[2][0].B	= 7;
	LeagueFixtures[2][1].A	= 1;
	LeagueFixtures[2][1].B	= 6;
	LeagueFixtures[2][2].A	= 2;
	LeagueFixtures[2][2].B	= 5;
	LeagueFixtures[2][3].A	= 3;
	LeagueFixtures[2][3].B	= 4;

	LeagueFixtures[3][0].A	= 0;
	LeagueFixtures[3][0].B	= 2;
	LeagueFixtures[3][1].A	= 1;
	LeagueFixtures[3][1].B	= 3;
	LeagueFixtures[3][2].A	= 4;
	LeagueFixtures[3][2].B	= 6;
	LeagueFixtures[3][3].A	= 5;
	LeagueFixtures[3][3].B	= 7;

	LeagueFixtures[4][0].A	= 0;
	LeagueFixtures[4][0].B	= 4;
	LeagueFixtures[4][1].A	= 1;
	LeagueFixtures[4][1].B	= 5;
	LeagueFixtures[4][2].A	= 2;
	LeagueFixtures[4][2].B	= 6;
	LeagueFixtures[4][3].A	= 3;
	LeagueFixtures[4][3].B	= 7;

	LeagueFixtures[5][0].A	= 0;
	LeagueFixtures[5][0].B	= 6;
	LeagueFixtures[5][1].A	= 1;
	LeagueFixtures[5][1].B	= 7;
	LeagueFixtures[5][2].A	= 2;
	LeagueFixtures[5][2].B	= 4;
	LeagueFixtures[5][3].A	= 3;
	LeagueFixtures[5][3].B	= 5;	// Data 'corrupted' from here on down.

	LeagueFixtures[6][0].A	= 0;
	LeagueFixtures[6][0].B	= 5;
	LeagueFixtures[6][1].A	= 1;
	LeagueFixtures[6][1].B	= 4;
	LeagueFixtures[6][2].A	= 2;
	LeagueFixtures[6][2].B	= 7;
	LeagueFixtures[6][3].A	= 3;
	LeagueFixtures[6][3].B	= 6;

	RandomiseLeagueFixturesOrder();	// Randomise fixture order.
	LeagueRound			= 0;

	// Not playing a playoff featuring a human team.
	LeagueHumanPlayOff	= false;
	
	// Store game type. 
	GameType			= _GameType;
	//PlayerName[0]		= DEFAULT_PLAYER_NAME;

	// Free all current data (if any) and then create new teams. 
	for (i = (NUM_OF_DIVISIONS * NUM_OF_TEAMS_PER_DIVISION); i--;)
	{
		SAFELY_DELETE(Teams[i]);
		if (i) 
		{
			NEW(Teams[i], CTeam(_GameType, i, true, GameDifficulty));
		}
		else
		{
			NEW(Teams[i], CTeam(_GameType));
			if (	(i == 0) 
				&&	(_GameType == LEAGUE_TEAM_MANAGER))
			{
				Teams[i]->ControlledBy = false;	// Tells the computer to process the team.
			}
		}
	}
	PlayerOne			= NULL;
	PlayerTwo			= NULL;
	LastTeamPlayed		= NULL;
	KnockOutOpponent	= -1;

	// Clear "Division" info. 
	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
		{
			Division[i][j]			= NULL;
			SortedDivision[i][j]	= NULL;
		}
	}

	// Set all star players as not for sale.
	for (i = NUM_OF_STAR_PLAYERS; i--;)
	{
		StarPlayersState[i] = max(MIN_INITIAL_SALE_PAUSE, 1 + (rand() >> 12));
		SAFELY_DELETE(StarPlayersArray[i]);
	}

	// Create Star players				
	//									 Team Pos   Agg  Att  Def  Spd  Thr  Pwr  Sta  Int
	NEW(StarPlayersArray[0], CTeamMember(0,	  CFWD, 180, 230, 250, 250, 230, 230, 250, 250));
	SAFELY_CALL(StarPlayersArray[0], setName(&CString("Jams")));

	NEW(StarPlayersArray[1], CTeamMember(0,	  CFWD, 180, 220, 240, 240, 220, 220, 240, 230));
	SAFELY_CALL(StarPlayersArray[1], setName(&CString("Norman")));

	NEW(StarPlayersArray[2], CTeamMember(0,	  CFWD, 160, 200, 220, 220, 190, 190, 220, 200));
	SAFELY_CALL(StarPlayersArray[2], setName(&CString("Caza")));

	NEW(StarPlayersArray[3], CTeamMember(0,	  CFWD, 170, 200, 200, 210, 210, 200, 180, 190));
	SAFELY_CALL(StarPlayersArray[3], setName(&CString("Weiss")));

	NEW(StarPlayersArray[4], CTeamMember(0,	  CFWD, 200, 240, 220, 220, 240, 240, 220, 230));
	SAFELY_CALL(StarPlayersArray[4], setName(&CString("Garrik")));

	NEW(StarPlayersArray[5], CTeamMember(0,	  CFWD, 170, 210, 210, 220, 220, 210, 190, 200));
	SAFELY_CALL(StarPlayersArray[5], setName(&CString("Roscopp")));

	NEW(StarPlayersArray[6], CTeamMember(0,	  CFWD, 180, 220, 230, 230, 220, 220, 230, 220));
	SAFELY_CALL(StarPlayersArray[6], setName(&CString("Montez")));

	NEW(StarPlayersArray[7], CTeamMember(0,	  CFWD, 180, 200, 180, 180, 200, 200, 180, 180));
	SAFELY_CALL(StarPlayersArray[7], setName(&CString("Shorn")));

	NEW(StarPlayersArray[8],  CTeamMember(0,  CFWD,  160, 180, 200, 200, 180, 180, 200, 180));
	SAFELY_CALL(StarPlayersArray[8], setName(&CString("Quiss")));

	NEW(StarPlayersArray[9],  CTeamMember(0,  CFWD,  190, 220, 220, 230, 230, 220, 220, 220));
	SAFELY_CALL(StarPlayersArray[9], setName(&CString("Quaid")));

	NEW(StarPlayersArray[10],  CTeamMember(0, CFWD,  170, 190, 190, 200, 200, 190, 180, 180));
	SAFELY_CALL(StarPlayersArray[10], setName(&CString("Rocco")));

	NEW(StarPlayersArray[11],  CTeamMember(0, CFWD,  180, 210, 180, 180, 210, 210, 180, 190));
	SAFELY_CALL(StarPlayersArray[11], setName(&CString("Luthor")));

	NEW(StarPlayersArray[12],  CTeamMember(0, CFWD,  200, 250, 230, 230, 250, 250, 230, 250));
	SAFELY_CALL(StarPlayersArray[12], setName(&CString("Jenson")));

	NEW(StarPlayersArray[13],   CTeamMember(0,CFWD,  200, 230, 220, 220, 230, 230, 220, 220));
	SAFELY_CALL(StarPlayersArray[13], setName(&CString("Cooper")));

	NEW(StarPlayersArray[14],   CTeamMember(0,CFWD,  190, 230, 230, 240, 240, 220, 220, 230));
	SAFELY_CALL(StarPlayersArray[14], setName(&CString("Stavia")));

	NEW(StarPlayersArray[15],   CTeamMember(0,CFWD,  160, 190, 210, 210, 180, 180, 210, 190));
	SAFELY_CALL(StarPlayersArray[15], setName(&CString("Midia")));

	NEW(StarPlayersArray[16],   CTeamMember(0,CFWD,  190, 240, 240, 250, 250, 230, 230, 250));
	SAFELY_CALL(StarPlayersArray[16], setName(&CString("Seline")));

	NEW(StarPlayersArray[17],   CTeamMember(0,CFWD,  180, 220, 190, 190, 220, 220, 190, 200));
	SAFELY_CALL(StarPlayersArray[17], setName(&CString("Bodini")));

	// Assign correct faces and calculate worth of star players
	for (i = NUM_OF_STAR_PLAYERS; i--;)
	{
		StarPlayersArray[i]->setPicture(12 + i);
		StarPlayersArray[i]->CalcValue();
	}


	// Create teams in different was depending on the game type.
	switch (GameType)
	{
	case PRACTICE:
		PlayerOne		= Teams[0];	// Team zero is always Brutal. 
		PlayerTwo		= Teams[1];	// Hack!!!
		*(Teams[1]->GetTeamName()) = " ";
		*(Teams[1]->GetShortTeamName()) = " ";
		return true;

	case DEMO:
		// Generate two unique team numbers.
#ifndef DEMO_BUILD
		RandTeamA = 1 + ((rand() * 14) / RAND_MAX);
		RandTeamB = RandTeamA;
		while (RandTeamB == RandTeamA)
			RandTeamB = 1 + ((rand() * 14) / RAND_MAX);
#else
		RandTeamA = 11;
		RandTeamB = 13;
#endif

		// Assign teams into game. 
		PlayerOne		= Teams[RandTeamA];
		PlayerTwo		= Teams[RandTeamB];
		return true;

	case KNOCKOUT:
		// Easy, as all matches are sequential. 
		KnockOutOpponent= 1;
		PlayerOne		= Teams[0];
		PlayerTwo		= Teams[KnockOutOpponent];
		return true;

	case LEAGUE:
	case LEAGUE_TEAM_MANAGER:
		// Create the divisions. 
		for (i = NUM_OF_DIVISIONS; i--;)
		{
			for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
			{
				Division[i][j]			= Teams[(NUM_OF_TEAMS_PER_DIVISION * (NUM_OF_DIVISIONS - i - 1)) + j];
				SortedDivision[i][j]	= Division[i][j];
			}
		}
		
		// Sort tables
		SortLeagues();

		// Assign first game
		PlayerOne		= Teams[0];	// Human team. 
		PlayerTwo		= Teams[LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B + ((1 - PlayerOne->GetDivision()) * NUM_OF_TEAMS_PER_DIVISION)];
		return true;

	case CUP:
		// Generate cup data.
		CupRound	= -1;
		CupLeg		= 0;
		UpdateCupState();	// CupRound MUST be negative!

		Teams[0]->SetBalance(CUP_STARTING_CASH);

		// Simply assign teams. 
	//	PlayerOne	=	Teams[CupFixtures[CupRound][0].A];
	//	PlayerTwo	=	Teams[CupFixtures[CupRound][0].B];

		return true;
	}

	// Hmmm, failure
	return false;
}

// ----
void	CGameData::RandomiseLeagueFixturesOrder()
{
	int i, k, j;

	// 'Home' matches
	for (i = 0; i < (NUM_OF_TEAMS_PER_DIVISION - 1); i++)
	{
		k = NUM_OF_TEAMS_PER_DIVISION;
		while (k >= (NUM_OF_TEAMS_PER_DIVISION - 1))
			k = (rand() >> 12); // I am relient on 'k' ending up in the range 0-6 inclusive. (Better hope MAX_RAND doesn't change).

		j =0;
		while (j < i)
		{
			if (LeagueFixturesOrder[j] == k)
			{
				// New number, and start testing again.
				k = NUM_OF_TEAMS_PER_DIVISION;
				while (k >= (NUM_OF_TEAMS_PER_DIVISION - 1))
					k = (rand() >> 12);
				j = 0;
			}
			else
				j++;
		}

		// Assign value.
		LeagueFixturesOrder[i] = k;
	}

	// 'Away' matches.
	for (i = 0; i < (NUM_OF_TEAMS_PER_DIVISION - 1); i++)
	{
		k = NUM_OF_TEAMS_PER_DIVISION;
		while (k >= (NUM_OF_TEAMS_PER_DIVISION - 1))
			k = (rand() >> 12); // I am relient on 'k' ending up in the range 0-6 inclusive. (Better hope MAX_RAND doesn't change).

		j =0;
		while (j < i)
		{
			if (LeagueFixturesOrder[(NUM_OF_TEAMS_PER_DIVISION - 1) + j] == k)
			{
				// New number, and start testing again.
				k = NUM_OF_TEAMS_PER_DIVISION;
				while (k >= (NUM_OF_TEAMS_PER_DIVISION - 1))
					k = (rand() >> 12);
				j = 0;
			}
			else
				j++;
		}

		// Assign value.
		LeagueFixturesOrder[(NUM_OF_TEAMS_PER_DIVISION - 1) + i] = k;
	}

	// Output for checking.
	for (i = 0; i < ((NUM_OF_TEAMS_PER_DIVISION - 1) << 1); i++)
	{
		TCHAR convert[24];
		wsprintf(convert, TEXT("Fixture %d is %d\n"), i, LeagueFixturesOrder[i]);
		OutputDebugString(convert);
	}
}


// ----
void		CGameData::SetDefaultGameDuration(int _a)	
{
	DefaultLength = _a;
}

// ----
void		CGameData::SetDefaultGameDifficulty(int _a)	
{
	DefaultDifficulty = _a;
}

// ----
// Returns a pointer to a structure holding the player's preferences.
_Prefs*		CGameData::GetPreferences()
{
	return &Preferences;
}

// ----
// Return registration code
unsigned long		CGameData::GetRegistrationCode()
{
	return RegistrationCode;
}

// ----
void		CGameData::SetRegistrationCode(unsigned long code)
{
	RegistrationCode = code;
}

// ----
// Returns true to indicate that saves were found. (I'll use this Anthony).
bool			CGameData::CatSaveFiles()
{
	TCHAR			SearchPath[128];
	WIN32_FIND_DATA	FoundFile;
	HANDLE			FindHandle;

	// Free existing save data.
	FreeSaveCache();

	// Get base path.
	wcscpy(SearchPath, PlatformRef->GetPath(EP_Saves)->GetString());
	wcscat(SearchPath, TEXT("*.sbs"));

	/*
	DWORD dwFileAttributes; 
	FILETIME ftCreationTime; 
	FILETIME ftLastAccessTime; 
	FILETIME ftLastWriteTime; 
	DWORD nFileSizeHigh; 
	DWORD nFileSizeLow; 
	DWORD dwOID; 
	TCHAR cFileName[MAX_PATH]; 
	WIN32_FIND_DATA; 
	*/

	// Go through all the files found.
	// First file,
	OutputDebugString(PlatformRef->GetPath(EP_Saves)->GetString());
	OutputDebugString(SearchPath);
	FindHandle = FindFirstFile(SearchPath, &FoundFile);
	if (FindHandle != INVALID_HANDLE_VALUE)
	{
		CSFSlave(&FoundFile);
	
		// Subsequent files.
		while (FindNextFile(FindHandle, &FoundFile))
		{
			CSFSlave(&FoundFile);
		}
		FindClose(FindHandle);
	}
	
	// ----
	// ALL
	// Sort all saves alpabetically.
	if (NumberOfSaves > 0)
	{
		SaveInfoListStruct* SortItem;	// Current item
		SaveInfoListStruct* Sorted;		// End up here
		SaveInfoListStruct*	Current;
		SaveInfoListStruct*	Previous;

		Sorted = FirstSave;
		FirstSave = FirstSave->NextSave;
		Sorted->NextSave = NULL;

		// For all items
		for (int count = NumberOfSaves - 1; count--;)
		{
			// Get next 
			SortItem	= FirstSave;
			FirstSave	= FirstSave->NextSave;
			SortItem->NextSave = NULL;

			// Hunt position
			Current = Sorted;	
			Previous = NULL;
			while (Current)//->NextSave)	
			{
				if (_wcsicmp(Current->Data.FriendlyName.GetString(), SortItem->Data.FriendlyName.GetString()) > 0)
					break;
				Previous = Current;
				Current = Current->NextSave;
			}

			// Insert
			SortItem->NextSave = Current;
			if (Previous)
				Previous->NextSave = SortItem;
			else
				Sorted = SortItem;
		}

		// Assign finished list.
		FirstSave = Sorted;
	}


	// ---- 
	// CUP
	// Sort cup saves alpabetically.
	if (NumberOfCupSaves > 0)
	{
		SaveInfoListStruct* SortItem;	// Current item
		SaveInfoListStruct* Sorted;		// End up here
		SaveInfoListStruct*	Current;
		SaveInfoListStruct*	Previous;

		Sorted				= FirstCupSave;
		FirstCupSave		= FirstCupSave->NextCupSave;
		Sorted->NextCupSave	= NULL;

		// For all items
		for (int count = NumberOfCupSaves - 1; count--;)
		{
			// Get next 
			SortItem		= FirstCupSave;
			FirstCupSave	= FirstCupSave->NextCupSave;
			SortItem->NextCupSave = NULL;

			// Hunt position
			Current = Sorted;	
			Previous = NULL;
			while (Current)//->NextCupSave)	
			{
				if (_wcsicmp(Current->Data.FriendlyName.GetString(), SortItem->Data.FriendlyName.GetString()) > 0)
					break;
				Previous = Current;
				Current = Current->NextCupSave;
			}

			// Insert
			SortItem->NextCupSave = Current;
			if (Previous)
				Previous->NextCupSave = SortItem;
			else
				Sorted = SortItem;
		}

		// Assign finished list.
		FirstCupSave = Sorted;
	}


	// ----
	// LEAGUE
	// Sort all saves alpabetically.
	if (NumberOfLeagueSaves > 0)
	{
		SaveInfoListStruct* SortItem;	// Current item
		SaveInfoListStruct* Sorted;		// End up here
		SaveInfoListStruct*	Current;
		SaveInfoListStruct*	Previous;

		Sorted = FirstLeagueSave;
		FirstLeagueSave = FirstLeagueSave->NextLeagueSave;
		Sorted->NextLeagueSave = NULL;

		// For all items
		for (int count = NumberOfLeagueSaves - 1; count--;)
		{
			// Get next 
			SortItem					= FirstLeagueSave;
			FirstLeagueSave				= FirstLeagueSave->NextLeagueSave;
			SortItem->NextLeagueSave	= NULL;

			// Hunt position
			Current = Sorted;	
			Previous = NULL;
			while (Current)//->NextLeagueSave)	
			{
				if (_wcsicmp(Current->Data.FriendlyName.GetString(), SortItem->Data.FriendlyName.GetString()) > 0)
					break;
				Previous = Current;
				Current = Current->NextLeagueSave;
			}

			// Insert
			SortItem->NextLeagueSave = Current;			// Insert before the current item.
			if (Previous)								// Was there a previous item
				Previous->NextLeagueSave = SortItem;	// Yes, insert after.
			else
				Sorted = SortItem;						// No, make front of list.
		}

		// Assign finished list.
		FirstLeagueSave = Sorted;
	}
		
	// Did we find any files?
	return (NumberOfSaves > 0);
}

// ----
#define	SAVE_FILE_HEADER_SIZE	(sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + sizeof(long))
// Does the nasty work of validating a save file and adding it into the lists.
void			CGameData::CSFSlave(WIN32_FIND_DATA	*_FoundFile)
{
	FILE				*FH;
	TCHAR				InstancePath[128];
	CString				TempString;
	SaveInfoListStruct	*tempSIS;
	TCHAR				*tempChar;
	char				ReadBuffer[SAVE_FILE_HEADER_SIZE + 1];
	char				*Index;

	// Open file.
	// Read header info.
	// Add to list.
	wcscpy(InstancePath, PlatformRef->GetPath(EP_Saves)->GetString());
	wcscat(InstancePath, _FoundFile->cFileName);

#ifdef _UNICODE
	FH = _wfopen(InstancePath, TEXT("rb"));
#else
	FH = fopen(InstancePath, TEXT("rb"));
#endif
	if (FH)
	{
		// Okay, file exists for sure.
		tempSIS = new SaveInfoListStruct;
		tempSIS->NextSave = NULL;
		tempSIS->NextCupSave = NULL;
		tempSIS->NextLeagueSave = NULL;

		// Stored file name
		tempSIS->Data.FileName		= _FoundFile->cFileName;

		tempChar = wcsrchr(_FoundFile->cFileName, '.');
		if (tempChar)
			*tempChar = 0;
		tempSIS->Data.FriendlyName	= _FoundFile->cFileName;

		// Check that the file name is not too long. (if it is then simple discard the file).
		if (tempSIS->Data.FriendlyName.GetStringLength() > MAX_SAVE_NAME_LENGTH)
		{
			delete tempSIS;
			tempSIS = NULL;
			fclose(FH);
			return;
		}

		// Read save header	(validating read length on the way).
		if (fread(ReadBuffer, SAVE_FILE_HEADER_SIZE, 1, FH) != 1)
		{
			delete tempSIS;
			tempSIS = NULL;
			fclose(FH);
			return;
		}
		fclose(FH);

		// Process read data.
		Index = ReadBuffer;
		tempSIS->Data.SaveVersion	= *(int*)Index; Index += sizeof(int);
		tempSIS->Data.GameType		= *(int*)Index; Index += sizeof(int);
		tempSIS->Data.Round			= *(int*)Index; Index += sizeof(int);
		tempSIS->Data.Cash			= *(int*)Index; Index += sizeof(int);
		tempSIS->Data.Date			= *(long*)Index; Index = NULL;

		/// Check to discard unsupported SaveVersions.
		if (tempSIS->Data.SaveVersion != FILE_SAVE_VERSION)
		{
			delete tempSIS;
			tempSIS = NULL;
			return;
		}

		// Now add data into chain.
		tempSIS->NextSave = FirstSave;
		FirstSave = tempSIS;

		// Inc counters
		NumberOfSaves++;
		switch (tempSIS->Data.GameType)
		{
		case LEAGUE:
		case LEAGUE_TEAM_MANAGER:
			NumberOfLeagueSaves++;
			tempSIS->NextLeagueSave = FirstLeagueSave;
			FirstLeagueSave = tempSIS;
			break;
		case CUP:
			NumberOfCupSaves++;
			tempSIS->NextCupSave = FirstCupSave;
			FirstCupSave = tempSIS;
		default:
			break;
		}
	}
}

// ----
// Frees data created by 'CatSaveFiles'
void			CGameData::FreeSaveCache()
{
	SaveInfoListStruct*	tempSIS;

	tempSIS = FirstSave;
	while (tempSIS)
	{
		FirstSave = tempSIS->NextSave;
		delete tempSIS;
		tempSIS = FirstSave;
	}

	// Reset counts
	NumberOfSaves		= 0;
	NumberOfCupSaves	= 0;
	NumberOfLeagueSaves	= 0;
}

// ----
unsigned int	CGameData::GetNumberOfSaves()
{
	return NumberOfSaves;
}

// ----
unsigned int	CGameData::GetNumOfCupSaves()
{
	return NumberOfCupSaves;
}

// ----
unsigned int	CGameData::GetNumOfLeagueSaves()
{
	return NumberOfLeagueSaves;
}


// ----
// Returns true if the _Index is in bounds 
// ((_Index >= 0) && (_Index < GetNumberOfSaves())) and
// fills in the provided structure. A return of false 
// will indicate that the _Index is an illegal value 
// and that struct will not contain valid info.
bool			CGameData::GetSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo)
{
	int					CountDown	= _Index;
	SaveInfoListStruct	*tempSIS	= FirstSave;

	// Too big?
	if (_Index >= NumberOfSaves)
		return false;

	// Find
	while ((CountDown) && (tempSIS))
	{
		CountDown--;
		tempSIS = tempSIS->NextSave;
	}

	// Validate
	if (tempSIS)
	{
		// Assign pointers
		*_SaveInfo = &(tempSIS->Data);
		return true;
	}

	return false;
}

// ----
bool			CGameData::GetSaveLCInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo)
{
	int					CountDown	= _Index;
	SaveInfoListStruct	*tempSIS	= NULL;

	// Too big?
	if (_Index >= NumberOfSaves)
		return false;

	if (_Index < NumberOfLeagueSaves)
	{
		// League save
		tempSIS		= FirstLeagueSave;

		// Find
		while ((CountDown) && (tempSIS))
		{
			CountDown--;
			tempSIS = tempSIS->NextLeagueSave;
		}
	}
	else
	{
		// Must be a cup save then...
		CountDown	-= NumberOfLeagueSaves;
		tempSIS		= FirstCupSave;

		// Find
		while ((CountDown) && (tempSIS))
		{
			CountDown--;
			tempSIS = tempSIS->NextCupSave;
		}
	}

	// Validate
	if (tempSIS)
	{
		// Assign pointers
		*_SaveInfo = &(tempSIS->Data);
		return true;
	}

	return false;
}

// ----
// As above, but filter games according to type.
bool			CGameData::GetCupSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo)
{
	int					CountDown	= _Index;
	SaveInfoListStruct	*tempSIS	= FirstCupSave;

	// Too big?
	if (_Index >= NumberOfCupSaves)
		return false;

	// Find
	while ((CountDown) && (tempSIS))
	{
		CountDown--;
		tempSIS = tempSIS->NextCupSave;
	}

	// Validate
	if (tempSIS)
	{
		// Assign pointers
		*_SaveInfo = &(tempSIS->Data);
		return true;
	}

	return false;
}

bool			CGameData::GetLeagueSaveInfo(unsigned int _Index, SaveInfoStruct** _SaveInfo)
{
	int					CountDown	= _Index;
	SaveInfoListStruct	*tempSIS	= FirstLeagueSave;

	// Too big?
	if (_Index >= NumberOfLeagueSaves)
		return false;

	// Find
	while ((CountDown) && (tempSIS))
	{
		CountDown--;
		tempSIS = tempSIS->NextLeagueSave;
	}

	// Validate
	if (tempSIS)
	{
		// Assign pointers
		*_SaveInfo = &(tempSIS->Data);
		return true;
	}

	return false;
}



// ----
// Loads a game
// True means that the game loaded successfully.
bool			CGameData::LoadGame(unsigned int _Index)
{
	SaveInfoListStruct	*tempSIS;
	TCHAR				InstancePath[128];
	SaveStruct			*LoadData;
	int					Count, i, j;
	FILE*				FH;

	if (_Index >= NumberOfSaves)
		return false;

	LoadData = NULL;
	LoadData = (SaveStruct*) malloc(sizeof(SaveStruct));
	
	// Count to the appropriate cup game
	Count	= _Index;
	tempSIS	= FirstSave;
	while ((Count) && (tempSIS))
	{
		tempSIS = tempSIS->NextSave;
		Count--;
	}

	// tempSIS now points to correct save record.
	// Time to go get the data... (joy),

	wcscpy(InstancePath, PlatformRef->GetPath(EP_Saves)->GetString());
	wcscat(InstancePath, tempSIS->Data.FileName.GetString());

#ifdef _UNICODE
	FH = _wfopen(InstancePath, TEXT("rb"));
#else
	FH = fopen(InstancePath, TEXT("rb"));
#endif
	if (FH)
	{
		// Read the data in then...
		if (fread((void*)LoadData, sizeof(SaveStruct), 1, FH) != 1)
		{
			fclose(FH);
			free(LoadData);
			return false;
		}
		fclose(FH);

		if (LoadData->SaveVersion != FILE_SAVE_VERSION)
		{
			free(LoadData);
			return false;
		}


		// Recreate all the teams by Init'ing the game as the one loaded.
		GameType	= LoadData->GameType;
		switch (GameType)
		{
		case LEAGUE:
		case LEAGUE_TEAM_MANAGER:
			Init(GameType, PlatformRef);
			LeagueRound	= LoadData->Round;
			break;							
		case CUP:							
			Init(GameType, PlatformRef);
			CupRound	= LoadData->Round;
			break;							
		default:							
			free(LoadData);
			return false;	// Unknown game type.
			break;
		}								
		GameType	= LoadData->GameType; // Should be right
		Teams[0]->SetBalance(LoadData->Cash);
											
		// Other info.						
		GameLength							= LoadData->GameLength;
		GameDifficulty						= LoadData->GameDifficulty;
			
		// Loop through restoring team states
		for (i = 0; i < 16; i++)
		{
			Teams[i]->SetBalance(LoadData->Teams[i].Cash);
			Teams[i]->SetPlayed(LoadData->Teams[i].NumGamesPlayed);
			Teams[i]->SetWon(LoadData->Teams[i].NumGamesWon);
			Teams[i]->SetDrawn(LoadData->Teams[i].NumGamesDrawn);
			Teams[i]->SetLost(LoadData->Teams[i].NumGamesLost);
		
			Teams[i]->SetDivision(LoadData->Teams[i].LeagueTable);
			Teams[i]->SetPlacing(LoadData->Teams[i].LeaguePlacing);
			Teams[i]->SetMovement(LoadData->Teams[i].LeagueMovement); 
			Teams[i]->SetFor(LoadData->Teams[i].PointsFor); 
			Teams[i]->SetAgainst(LoadData->Teams[i].PointsAgainst);
			Teams[i]->SetPoints(LoadData->Teams[i].Points); 

			Teams[i]->Score							= LoadData->Teams[i].Score;
			Teams[i]->CupScore						= LoadData->Teams[i].CupScore;

			Teams[i]->ShotsAtGoal					= LoadData->Teams[i].ShotsAtGoal;
			Teams[i]->GoalsScored					= LoadData->Teams[i].GoalsScored;
			Teams[i]->GoalsSaved					= LoadData->Teams[i].GoalsSaved;
			Teams[i]->TimeInPossesion				= LoadData->Teams[i].TimeInPossesion;
			Teams[i]->TimeInOppenentsHalf			= LoadData->Teams[i].TimeInOppenentsHalf;
			Teams[i]->SuccesfulTackles				= LoadData->Teams[i].SuccesfulTackles;
			Teams[i]->Substitutions					= LoadData->Teams[i].Substitutions;
			Teams[i]->BonusPointsScored				= LoadData->Teams[i].BonusPointsScored;
		}

		// Save off all the required player info.
		for (i = 0; i < 12; i++)
		{
			if (LoadData->HumanPlayerArray[i].Active != 1)
			{
				free(LoadData);
				return false;
			}

			Teams[0]->GetTeamMember(i)->setType(LoadData->HumanPlayerArray[i].type);
			CString TempString;
			TempString = LoadData->HumanPlayerArray[i].Name;
			Teams[0]->GetTeamMember(i)->setName(&TempString);

			Teams[0]->GetTeamMember(i)->setPosition(LoadData->HumanPlayerArray[i].Position);
			Teams[0]->GetTeamMember(i)->setWorth(LoadData->HumanPlayerArray[i].Worth);
							
			Teams[0]->GetTeamMember(i)->setAgr(LoadData->HumanPlayerArray[i].Agr);
			Teams[0]->GetTeamMember(i)->setAtt(LoadData->HumanPlayerArray[i].Att);
			Teams[0]->GetTeamMember(i)->setDef(LoadData->HumanPlayerArray[i].Def);
			Teams[0]->GetTeamMember(i)->setSpd(LoadData->HumanPlayerArray[i].Spd);
			Teams[0]->GetTeamMember(i)->setThr(LoadData->HumanPlayerArray[i].Thr);
			Teams[0]->GetTeamMember(i)->setPow(LoadData->HumanPlayerArray[i].Pow);
			Teams[0]->GetTeamMember(i)->setSta(LoadData->HumanPlayerArray[i].Sta);
			Teams[0]->GetTeamMember(i)->setInt(LoadData->HumanPlayerArray[i].Int);

			Teams[0]->GetTeamMember(i)->SetPictureNumber(LoadData->HumanPlayerArray[i].PictureNumber);
		}

		// Convert pointers back to integer team index.
		for (i = NUM_OF_DIVISIONS; i--;)
		{
			for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
			{
				if (LoadData->Division[i][j] >= 0)
				{
					if (LoadData->Division[i][j] < 16)
						Division[i][j]			= Teams[LoadData->Division[i][j]];
					else
					{
						free(LoadData);
						return false;
					}
				}

				if (LoadData->SortedDivision[i][j] >= 0)
				{
					if (LoadData->SortedDivision[i][j] < 16)
						SortedDivision[i][j]	= Teams[LoadData->SortedDivision[i][j]];
					else
					{
						free(LoadData);
						return false;
					}
				}
			}
		}

		if (LoadData->PlayerOne >= 0)
		{ 
			if (LoadData->PlayerOne < 16)
				PlayerOne		= Teams[LoadData->PlayerOne];
			else
			{
				free(LoadData);
				return false;
			}
		}
		else
		{
			free(LoadData);
			return false;
		}

		PlayerTwo = NULL;
		if (LoadData->PlayerTwo >= 0)
		{ 
			if (LoadData->PlayerTwo < 16)
				PlayerTwo		= Teams[LoadData->PlayerTwo];
			else
			{
				free(LoadData);
				return false;
			}
		}

		LastTeamPlayed = NULL;
		if (LoadData->LastTeamPlayed >= 0)
		{ 
			if (LoadData->LastTeamPlayed < 16)
				LastTeamPlayed		= Teams[LoadData->LastTeamPlayed];
			else
			{
				free(LoadData);
				return false;
			}
		}
	
		KnockOutOpponent	= LoadData->KnockOutOpponent;
		LeagueRound			= LoadData->LeagueRound;
		LeagueHumanPlayOff	= LoadData->LeagueHumanPlayOff;

	
		// League fixtures
		for (i = (NUM_OF_TEAMS_PER_DIVISION - 1); i--;)
		{
			for (j = (NUM_OF_TEAMS_PER_DIVISION >> 1); j--;)
			{
				LeagueFixtures[i][j].A	= LoadData->LeagueFixtures[i][j].A;
				LeagueFixtures[i][j].B	= LoadData->LeagueFixtures[i][j].B;
			}
		}

		// LeagueFixturesOrder
		for (i = ((NUM_OF_TEAMS_PER_DIVISION - 1) << 1); i--;)
		{
			LeagueFixturesOrder[i] = LoadData->LeagueFixturesOrder[i];
		}

		// Cup fixtures
		for (i = NUM_OF_CUP_ROUNDS; i--;)
		{
			for (j = MAX_CUP_FIXTURES; j--;)
			{
				CupFixtures[i][j].A	= LoadData->CupFixtures[i][j].A;
				CupFixtures[i][j].B	= LoadData->CupFixtures[i][j].B;
			}
		}

		CupRound	= LoadData->CupRound;
		CupLeg		= LoadData->CupLeg;
		CupState	= (ECupState)LoadData->CupState;

		for (i = NUM_OF_STAR_PLAYERS; i--;)
		{
			// Free the existing players.
			SAFELY_DELETE(StarPlayersArray[i]);

			// Restore market info.
			StarPlayersState[i] = LoadData->StarPlayersState[i];

			if (LoadData->StarPlayersArray[i].Active != 1)
				continue;

			// Recreate player if needed.
			NEW(StarPlayersArray[i], CTeamMember(	0,		// ATeam		// Not updated
													0,		// APosition	// Updated
													100,	// AAgr 		// Updated
													100,	// AAtt 		// Updated
													100,	// ADef 		// Updated
													100,	// ASpd 		// Updated
													100,	// AThr 		// Updated
													100,	// APow 		// Updated
													100,	// ASta			// Updated
													100));	// AInt			// Updated

			// Re-assign values.
			StarPlayersArray[i]->setType(LoadData->StarPlayersArray[i].type);
			CString TempString;
			TempString = LoadData->StarPlayersArray[i].Name;
			StarPlayersArray[i]->setName(&TempString);

			StarPlayersArray[i]->setPosition(LoadData->StarPlayersArray[i].Position);
			StarPlayersArray[i]->setWorth(LoadData->StarPlayersArray[i].Worth);
							
			StarPlayersArray[i]->setAgr(LoadData->StarPlayersArray[i].Agr);
			StarPlayersArray[i]->setAtt(LoadData->StarPlayersArray[i].Att);
			StarPlayersArray[i]->setDef(LoadData->StarPlayersArray[i].Def);
			StarPlayersArray[i]->setSpd(LoadData->StarPlayersArray[i].Spd);
			StarPlayersArray[i]->setThr(LoadData->StarPlayersArray[i].Thr);
			StarPlayersArray[i]->setPow(LoadData->StarPlayersArray[i].Pow);
			StarPlayersArray[i]->setSta(LoadData->StarPlayersArray[i].Sta);
			StarPlayersArray[i]->setInt(LoadData->StarPlayersArray[i].Int);

			StarPlayersArray[i]->SetPictureNumber(LoadData->StarPlayersArray[i].PictureNumber);
		}

		// Right, everything seems okay, thus store the 
		// load name as the save name - thus they will 
		// effectively be prompted to save over the game
		// they load, if they go to save again that is.
		//SaveName = TempNameStore;	

		free(LoadData);
		return true;
	}

	free(LoadData);
	return false;
}

bool			CGameData::LoadCupGame(unsigned int _Index)
{
	// Simplest to look up save and call LoadGame.
	SaveInfoListStruct	*tempSIS;
	SaveInfoListStruct	*lookingFor;
	int Count;

	if (_Index >= NumberOfCupSaves)
		return false;	// Reject - out of bounds

	// Count to the appropriate cup game
	Count		= _Index;
	lookingFor	= FirstCupSave;
	while ((Count) && (lookingFor))
	{
		lookingFor = lookingFor->NextCupSave;
		Count--;
	}

	Count = 0;
	tempSIS = FirstSave;
	while (tempSIS)
	{
		if (tempSIS == lookingFor)
		{
			return LoadGame(Count);
		}

		Count++;
		tempSIS = tempSIS->NextSave;
	}
	return false;
}

bool			CGameData::LoadLeagueGame(unsigned int _Index)
{
	// Simplest to look up save and call LoadGame.
	SaveInfoListStruct	*tempSIS;
	SaveInfoListStruct	*lookingFor;
	int Count;

	if (_Index >= NumberOfLeagueSaves)
		return false;	// Reject - out of bounds

	// Count to the appropriate league game
	Count		= _Index;
	lookingFor	= FirstLeagueSave;
	while ((Count) && (lookingFor))
	{
		lookingFor = lookingFor->NextLeagueSave;
		Count--;
	}

	Count = 0;
	tempSIS = FirstSave;
	while (tempSIS)
	{
		if (tempSIS == lookingFor)
		{
			return LoadGame(Count);
		}

		Count++;
		tempSIS = tempSIS->NextSave;
	}
	return false;
}



// ----
// Checks to see if a file with the given name exists. 
// '_SaveName' should not contain a file extension or 
// path information. 
bool			CGameData::DoesSaveExist(CString*	_SaveName)
{
	TCHAR		InstancePath[128];
	FILE		*FH;
	
	// Generate file path.
	wcscpy(InstancePath, PlatformRef->GetPath(EP_Saves)->GetString());
	wcscat(InstancePath, _SaveName->GetString());
	wcscat(InstancePath, TEXT(".sbs"));

#ifdef _UNICODE
	FH = _wfopen(InstancePath, TEXT("rb"));
#else
	FH = fopen(InstancePath, TEXT("rb"));
#endif
	if (FH)
	{
		fclose(FH);
		return true;
	}

	return false;
}


// ----
// Saves the current data to the given file.
// '_SaveName' should not contain a file extension or 
// path information. 
// 
// WARNING:	Overwrites files without warning!!!
bool			CGameData::SaveGame(CString*	_SaveName)
{
	TCHAR		InstancePath[128];
	FILE		*FH;
	SaveStruct	SaveData;
	int			i, j;

	// Generate file path.
	wcscpy(InstancePath, PlatformRef->GetPath(EP_Saves)->GetString());
	wcscat(InstancePath, _SaveName->GetString());
	wcscat(InstancePath, TEXT(".sbs"));


#ifdef _UNICODE
	FH = _wfopen(InstancePath, TEXT("wb"));
#else
	FH = fopen(InstancePath, TEXT("wb"));
#endif
	if (FH)
	{
		SYSTEMTIME	Time;
		GetSystemTime(&Time);	

		// These five properties constitute the file header.
		SaveData.SaveVersion					= FILE_SAVE_VERSION;
		SaveData.GameType						= GameType;	
		switch (GameType)
		{
		case LEAGUE:
		case LEAGUE_TEAM_MANAGER:
			SaveData.Round						= LeagueRound;
			break;								
		case CUP:								
			SaveData.Round						= CupRound;
			break;							
		default:							
			SaveData.Round						= 0;
			break;							
		}									
		SaveData.Cash							= Teams[0]->GetBalance();
		SaveData.Date							= (Time.wYear << 11) + (Time.wMonth << 6) + (Time.wDay);
												
		// Other info.							
		SaveData.GameLength						= GameLength;
		SaveData.GameDifficulty					= GameDifficulty;
			
		for (i = 0; i < 16; i++)
		{
			SaveData.Teams[i].Cash					= Teams[i]->GetBalance();
			SaveData.Teams[i].NumGamesPlayed		= Teams[i]->GetPlayed();
			SaveData.Teams[i].NumGamesWon			= Teams[i]->GetWon();
			SaveData.Teams[i].NumGamesDrawn			= Teams[i]->GetDrawn();
			SaveData.Teams[i].NumGamesLost			= Teams[i]->GetLost();
														
			SaveData.Teams[i].LeagueTable			= Teams[i]->GetDivision();
			SaveData.Teams[i].LeaguePlacing			= Teams[i]->GetPlacing();
			SaveData.Teams[i].LeagueMovement		= Teams[i]->GetMovement();
			SaveData.Teams[i].PointsFor				= Teams[i]->GetFor();
			SaveData.Teams[i].PointsAgainst			= Teams[i]->GetAgainst();
			SaveData.Teams[i].Points				= Teams[i]->GetPoints();
					
			SaveData.Teams[i].Score					= Teams[i]->Score;
			SaveData.Teams[i].CupScore				= Teams[i]->CupScore;

			SaveData.Teams[i].ShotsAtGoal			= Teams[i]->ShotsAtGoal;
			SaveData.Teams[i].GoalsScored			= Teams[i]->GoalsScored;
			SaveData.Teams[i].GoalsSaved			= Teams[i]->GoalsSaved;
			SaveData.Teams[i].TimeInPossesion		= Teams[i]->TimeInPossesion;
			SaveData.Teams[i].TimeInOppenentsHalf	= Teams[i]->TimeInOppenentsHalf;
			SaveData.Teams[i].SuccesfulTackles		= Teams[i]->SuccesfulTackles;
			SaveData.Teams[i].Substitutions			= Teams[i]->Substitutions;
			SaveData.Teams[i].BonusPointsScored		= Teams[i]->BonusPointsScored;
		}

		// Save off all the required player info.
		for (i = 0; i < 12; i++)
		{
			SaveData.HumanPlayerArray[i].Active			= 1;
			SaveData.HumanPlayerArray[i].type			= Teams[0]->GetTeamMember(i)->getType();
			wcsncpy(SaveData.HumanPlayerArray[i].Name,	  Teams[0]->GetTeamMember(i)->getName()->GetString(), 15);
			SaveData.HumanPlayerArray[i].Position		= Teams[0]->GetTeamMember(i)->getPosition();
			SaveData.HumanPlayerArray[i].Worth			= Teams[0]->GetTeamMember(i)->getWorth();
							
			SaveData.HumanPlayerArray[i].Agr			= Teams[0]->GetTeamMember(i)->getAgr();
			SaveData.HumanPlayerArray[i].Att			= Teams[0]->GetTeamMember(i)->getAtt();
			SaveData.HumanPlayerArray[i].Def			= Teams[0]->GetTeamMember(i)->getDef();
			SaveData.HumanPlayerArray[i].Spd			= Teams[0]->GetTeamMember(i)->getSpd();
			SaveData.HumanPlayerArray[i].Thr			= Teams[0]->GetTeamMember(i)->getThr();
			SaveData.HumanPlayerArray[i].Pow			= Teams[0]->GetTeamMember(i)->getPow();
			SaveData.HumanPlayerArray[i].Sta			= Teams[0]->GetTeamMember(i)->getSta();
			SaveData.HumanPlayerArray[i].Int			= Teams[0]->GetTeamMember(i)->getInt();

			SaveData.HumanPlayerArray[i].PictureNumber	= Teams[0]->GetTeamMember(i)->GetPictureNumber();
		}

		// Convert pointers back to integer team index.
		for (i = NUM_OF_DIVISIONS; i--;)
		{
			for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
			{
				if (Division[i][j] != NULL)
					SaveData.Division[i][j]			= Division[i][j]->GetTeamNumber();
				else
					 SaveData.Division[i][j]		= -1;

				if (SortedDivision[i][j] != NULL)
					SaveData.SortedDivision[i][j]	= SortedDivision[i][j]->GetTeamNumber();
				else
					SaveData.SortedDivision[i][j]	= -1;
			}
		}

		if (PlayerOne)
			SaveData.PlayerOne		= PlayerOne->GetTeamNumber();
		else
			SaveData.PlayerOne		= -1;

		if (PlayerTwo)
			SaveData.PlayerTwo		= PlayerTwo->GetTeamNumber();
		else
			SaveData.PlayerTwo		= -1;

		if (LastTeamPlayed)
			SaveData.LastTeamPlayed	= LastTeamPlayed->GetTeamNumber();
		else
			SaveData.LastTeamPlayed		= -1;				
	
		SaveData.KnockOutOpponent	= KnockOutOpponent;
		SaveData.LeagueRound		= LeagueRound;
		SaveData.LeagueHumanPlayOff	= LeagueHumanPlayOff;

		// League fixtures
		for (i = (NUM_OF_TEAMS_PER_DIVISION - 1); i--;)
		{
			for (j = (NUM_OF_TEAMS_PER_DIVISION >> 1); j--;)
			{
				SaveData.LeagueFixtures[i][j].A	= LeagueFixtures[i][j].A;
				SaveData.LeagueFixtures[i][j].B	= LeagueFixtures[i][j].B;
			}
		}

		// LeagueFixturesOrder
		for (i = ((NUM_OF_TEAMS_PER_DIVISION - 1) << 1); i--;)
		{
			SaveData.LeagueFixturesOrder[i]	= LeagueFixturesOrder[i];
		}

		// Cup fixtures
		for (i = NUM_OF_CUP_ROUNDS; i--;)
		{
			for (j = MAX_CUP_FIXTURES; j--;)
			{
				SaveData.CupFixtures[i][j].A	= CupFixtures[i][j].A;
				SaveData.CupFixtures[i][j].B	= CupFixtures[i][j].B;
			}
		}

		SaveData.CupRound	= CupRound;
		SaveData.CupLeg		= CupLeg;
		SaveData.CupState	= (int)CupState;

		for (i = NUM_OF_STAR_PLAYERS; i--;)
		{
			SaveData.StarPlayersState[i] = StarPlayersState[i];
			if (StarPlayersArray[i])
			{
				SaveData.StarPlayersArray[i].Active			= 1;
				SaveData.StarPlayersArray[i].type			= StarPlayersArray[i]->getType();
				wcsncpy(SaveData.StarPlayersArray[i].Name, StarPlayersArray[i]->getName()->GetString(), 15);
				SaveData.StarPlayersArray[i].Position		= StarPlayersArray[i]->getPosition();
				SaveData.StarPlayersArray[i].Worth			= StarPlayersArray[i]->getWorth();
										
				SaveData.StarPlayersArray[i].Agr			= StarPlayersArray[i]->getAgr();
				SaveData.StarPlayersArray[i].Att			= StarPlayersArray[i]->getAtt();
				SaveData.StarPlayersArray[i].Def			= StarPlayersArray[i]->getDef();
				SaveData.StarPlayersArray[i].Spd			= StarPlayersArray[i]->getSpd();
				SaveData.StarPlayersArray[i].Thr			= StarPlayersArray[i]->getThr();
				SaveData.StarPlayersArray[i].Pow			= StarPlayersArray[i]->getPow();
				SaveData.StarPlayersArray[i].Sta			= StarPlayersArray[i]->getSta();
				SaveData.StarPlayersArray[i].Int			= StarPlayersArray[i]->getInt();

				SaveData.StarPlayersArray[i].PictureNumber	= StarPlayersArray[i]->GetPictureNumber();
			}
			else
			{
				SaveData.StarPlayersArray[i].Active = 0;
			}
		}

		// Write data.
		if (fwrite((void*)&SaveData, sizeof(SaveStruct), 1, FH) != 1)
		{
			fclose(FH);
			return false;
		}

		fclose(FH);
	//	CatSaveFiles();	// Re-generate save file list
		return true;
	}

	return false;
}


// ----
// Returns a handle to the indexed star player.
CTeamMember*	CGameData::GetStarPlayer(int _Index)
{
	if (	(_Index >= 0)
		&&	(_Index < NUM_OF_STAR_PLAYERS))
	{
		return StarPlayersArray[_Index];
	}

	return NULL;
}

// ----
// Determines which players the team can purchase.
// Call once straight after playing a match.
void			CGameData::ProcessStarPlayers()
{
	int i;

	for (i = NUM_OF_STAR_PLAYERS; i--;)
	{
		// Already purchased value acts as a latch to hold state.
		if (StarPlayersState[i] < ALREADY_PURCHASED)
		{
			/* This looks to be wrong from playing the PC version. 
			if (GameType == CUP)
			{
				// Star players are not restricted in Cup.
				StarPlayersState[i] = -1;
			}
			else
			*/
			{
				StarPlayersState[i]--;
				if (StarPlayersState[i] == 0)
				{
					// Start by taking them off the market.
					StarPlayersState[i] = 1 + (rand() >> 12);	// 1 + (0 -> 7) ... assuming MAX_RAND == 2^15 - 1

					// If cup game then put them on the market.
					if (GameType == CUP)
					{
						StarPlayersState[i] = -1 - (rand() >> 13);	// 0 -> 3 ... assuming MAX_RAND == 2^15 - 1
					}

					// If a league game
					if (	(GameType == LEAGUE_TEAM_MANAGER)
						||	(GameType == LEAGUE))
					{
						// Division 1 - players are unlimited
						if (	(Teams[0]->GetDivision() == 0)			// Is division 1
							//&&	(StarPlayersArray[i]->getWorth() >=	((GameType == LEAGUE)?DIVISION_1_PRICE_THRESHOLD_NEW:DIVISION_1_PRICE_THRESHOLD_TM))
							)
						{
							StarPlayersState[i] = -1 - (rand() >> 13);	// 0 -> 3 ... assuming MAX_RAND == 2^15 - 1
						}

						// If in second division limit the players by cost.
						if (	(Teams[0]->GetDivision() == 1)			// Is division 2
							&&	(StarPlayersArray[i]->getWorth() <= ((GameType == LEAGUE)?DIVISION_1_PRICE_THRESHOLD_NEW:DIVISION_1_PRICE_THRESHOLD_TM)))
						{
							StarPlayersState[i] = -1 - (rand() >> 13);	// 0 -> 3 ... assuming MAX_RAND == 2^15 - 1
						}
					}
				}
				else
				{
					// See if the players are ready to come off the market.
					if (StarPlayersState[i] < PLAYER_SALE_TIMEOUT)
					{
						// Players have come off the market.
						// Now reseed the time until they come back onto the market.
						// 1-8 games.
						StarPlayersState[i] = 1 + (rand() >> 12);	// 1 + (0 -> 7) ... assuming MAX_RAND == 2^15 - 1
					}
				}
			}
		}
	}
}

// ----
// Purchase a selected star player
bool		CGameData::Purchase(int _SelectedPlayer, int _SelectedPosition)
{
	// Check that the star player and replaced player are in range
	if (	(_SelectedPlayer < 0) 
		||	(_SelectedPlayer >= NUM_OF_STAR_PLAYERS)
		||	(_SelectedPosition < 0) 
		||	(_SelectedPosition > 11))
	return false;

	CTeamMember*	TMRef = Teams[0]->GetTeamMember(_SelectedPosition);

	// Check balance and player availability
	if (	(StarPlayersState[_SelectedPlayer] < 0)			// Up for sale
		&&	(StarPlayersArray[_SelectedPlayer] != NULL)		// Just a parnoid check to make sure the player exist
		&&	(StarPlayersArray[_SelectedPlayer]->getWorth() <= 
				(Teams[0]->GetBalance() + TMRef->getWorth())
			)
		)
	{
		// Modify balance.
		Teams[0]->DebitAccount(StarPlayersArray[_SelectedPlayer]->getWorth() - TMRef->getWorth());

		// Pull player position info across.
		StarPlayersArray[_SelectedPlayer]->setPosition(TMRef->getPosition());

		// If they are a star player, suppose we should put them back on the market.
		if (TMRef->GetPictureNumber() > 11)	// Star player have a face index > 11 (what a great test!!!).
		{
			// Swap star players then.

			// Select star player into team
			Teams[0]->SetTeamMember(_SelectedPosition, StarPlayersArray[_SelectedPlayer]);
			
			// Place old star back on the market.
			StarPlayersState[_SelectedPlayer] = PLAYER_SALE_TIMEOUT; // Can be re-purchased straight away (just incase the wrong player was sold - and prevents the case of having no players for sale whilst showing the Transfer screen :o)
			StarPlayersArray[_SelectedPlayer] = TMRef;
		}
		else
		{
			// Delete old player
			SAFELY_DELETE(TMRef);

			// Select star player into team
			Teams[0]->SetTeamMember(_SelectedPosition, StarPlayersArray[_SelectedPlayer]);
			
			// Take the star off sale
			StarPlayersState[_SelectedPlayer] = ALREADY_PURCHASED;	// Can't re-purchase
			StarPlayersArray[_SelectedPlayer] = NULL;				// Lose reference to star player
		}
		return true;
	}

	return false;
}


// ----
// Returns the index of the next available star player
// Returns -1 if no players available.
int			CGameData::GetNextStarPlayerIndex(int _Index)
{
	int i;

	// Check above current pos.
	for (i = (_Index + 1); i < NUM_OF_STAR_PLAYERS; i++)
	{
		if (StarPlayersState[i] < 0)
		{
			return i;
		}
	}

	// Check up to (and including) current pos
	for (i = 0; i <= _Index; i++)
	{
		if (StarPlayersState[i] < 0)
		{
			return i;
		}
	}
	return -1;
}

// ----
// Returns the index of the previous available star player
// Returns -1 if no players available.
int				CGameData::GetPreviousStarPlayerIndex(int _Index)
{
	int i;

	// Check above current pos.
	for (i = (_Index - 1); i >= 0; i--)
	{
		if (StarPlayersState[i] < 0)
		{
			return i;
		}
	}

	// Check up to (and including) current pos
	for (i = (NUM_OF_STAR_PLAYERS - 1); i >= _Index; i--)
	{
		if (StarPlayersState[i] < 0)
		{
			return i;
		}
	}
	return -1;
}

// ----
// Loads the preference data.
bool		CGameData::LoadPreferences(CPlatform* _PlatformRef)
{
	DWORD	DataType	= 0;
	char*	Value		= NULL;
	DWORD	DataLength	= 0;
	LONG	ret;
	CString Temp;

	PlatformRef = _PlatformRef;

	// Assume some sensible defaults
	Preferences.Volume					= DEFAULT_VOLUME_LEVEL;
	Preferences.MusicEnabled			= DEFAULT_MUSIC_STATE;
	if (_PlatformRef)
	{
		switch (_PlatformRef->GetMachineFamily())
		{
		case EM_Casio_Cassiopeia_Generic:
			Preferences.DoubleBufferingEnabled	= false;
			break;

		case EM_Compaq_IPaq_Generic:
			Preferences.DoubleBufferingEnabled	= true;
			Preferences.SlowVRAMWorkAround		= false;
			break;

		default:
			Preferences.DoubleBufferingEnabled	= true;
			Preferences.SlowVRAMWorkAround		= true;
			break;
		}
	}
	else
		Preferences.DoubleBufferingEnabled		= false;

	Preferences.StylusControl			= DEFAULT_STYLUS_CONTROL;
//	Preferences.AutoSleepTimeOut		= DEFAULT_AUTO_SLEEP;

#if BOXEDSALE
	Preferences.Registered = true;		//  Always registered - put this here just incase something else in this method fails
#else
	Preferences.Registered = false;		// Will determine if we are registered in just a bit!
#endif


	DefaultLength		= DEFAULT_SIDE_DURATION;
	DefaultDifficulty	= DEFAULT_DIFFICULTY;

	ShowDemoTimeOut		= DEFAULT_DEMO_TIME_OUT;

	// ----
	// Load Volume from registry if we can.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_VOLUME_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Preferences.Volume = (int)*Value; 
		if (	(Preferences.Volume < 0)
			||	(Preferences.Volume > 10))
			Preferences.Volume	= DEFAULT_VOLUME_LEVEL;
	}
	SAFELY_DELETE_ARRAY(Value);


	// ----
	// Load Volume from registry if we can.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_MUSIC_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Preferences.MusicEnabled = (bool)*Value;
	}
	SAFELY_DELETE_ARRAY(Value);

	// Load double buffering settings.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_ENABLE_DB_SETTING),// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Preferences.DoubleBufferingEnabled = (bool)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);


	// Load double buffering settings.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_SLOW_VRAM_SETTING),// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Preferences.SlowVRAMWorkAround = (bool)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);


	// Load and validate stylus control setting
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_STYLUS_SETTING),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		Preferences.StylusControl = (int)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);

	// Load and validate power saving setting
//	ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
//							&CString(REGISTRY_PATH),			// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
//							&CString(REG_POWER_SAVE_SETTING),	// Name of value to retrieve.
//							&DataType,							// A pointer to a DWORD filled-in by the function.
//							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
//							&DataLength);
//	if (	(SUCCEEDED(ret)) 
//		&&	(DataType == REG_BINARY)
//		&&	(DataLength)
//		&&	(Value))
//	{
//		Preferences.AutoSleepTimeOut = (int)*Value; 
//		if (Preferences.AutoSleepTimeOut < 0)
//			Preferences.AutoSleepTimeOut = 0;	
//		// No upper bound.
//	}
//	SAFELY_DELETE_ARRAY(Value);

//	// Sleep after 15 seconds.
//	Preferences.AutoSleepTimeOut	= 15;

	// ----
	// Load game duration. 
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString(REGISTRY_PATH),			// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_DURATION_SETTING),		// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		DefaultLength = (int)*Value; 
		if (DefaultLength < LOWEST_GAME_DURATION)
			DefaultLength = LOWEST_GAME_DURATION;
		if (DefaultLength > UPPER_GAME_DURATION)
			DefaultLength = UPPER_GAME_DURATION;
	}
	SAFELY_DELETE_ARRAY(Value);

	// Load game difficulty. 
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString(REGISTRY_PATH),			// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_DIFFICULTY_SETTING),	// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		DefaultDifficulty = (int)*Value; 
		if (DefaultDifficulty < LOWEST_GAME_DIFFICULTY)
			DefaultDifficulty = LOWEST_GAME_DIFFICULTY;
		if (DefaultDifficulty > UPPER_GAME_DIFFICULTY)
			DefaultDifficulty = UPPER_GAME_DIFFICULTY;
	}
	SAFELY_DELETE_ARRAY(Value);

	// Load the time out until the demo is shown.
	ret = LoadFromRegistry(	HKEY_CURRENT_USER,					// Existing key or default root.
							&CString(REGISTRY_PATH),			// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_DEMO_TIMEOUT_SETTING),	// Name of value to retrieve.
							&DataType,							// A pointer to a DWORD filled-in by the function.
							&Value,								// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		ShowDemoTimeOut = (int)*Value; 
	}
	SAFELY_DELETE_ARRAY(Value);

#if BOXEDSALE
	Preferences.Registered = true;	//  Always registered
#else
	// Determine if we are registered
	CString TempName;

		
	// Load double buffering settings.
	ret = LoadFromRegistry(	HKEY_LOCAL_MACHINE,				// Existing key or default root.
							&CString(REGISTRY_PATH),		// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
							&CString(REG_ACTIVATION_CODE),	// Name of value to retrieve.
							&DataType,						// A pointer to a DWORD filled-in by the function.
							&Value,							// A pointer to buffer created within the function - you are responsible for the freeing of this resource.
							&DataLength);

	if (	(SUCCEEDED(ret)) 
		&&	(DataType == REG_BINARY)
		&&	(DataLength)
		&&	(Value))
	{
		RegistrationCode = DecryptCode(*(unsigned long*)Value); 
	}
	else
	{
		RegistrationCode = 0;
	}
	SAFELY_DELETE_ARRAY(Value);

	// See if the activation code is valid
	if (GetRegistrationString(&TempName))
	{
		Preferences.Registered = ValidateRegistrationDetails(&TempName, RegistrationCode, PRODUCT_ACTIVATION_RPN_STRING); 
	}
	else
	{
		Preferences.Registered = false;
	}
#endif

	return true;
}

// ----
// Saves preference data.
bool		CGameData::SavePreferences()
{
	// Save out volume setting
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_VOLUME_SETTING),				// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&Preferences.Volume,					// A pointer to the data to store.
					sizeof(Preferences.Volume));

	// Save out music setting
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_MUSIC_SETTING),				// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&Preferences.MusicEnabled,			// A pointer to the data to store.
					sizeof(Preferences.MusicEnabled));

	// Save double buffer setting
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_ENABLE_DB_SETTING),			// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&Preferences.DoubleBufferingEnabled,	// A pointer to the data to store.
					sizeof(Preferences.DoubleBufferingEnabled));

	// Save slow vram setting - only used if double buffering is enabled.
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_SLOW_VRAM_SETTING),			// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&Preferences.SlowVRAMWorkAround,		// A pointer to the data to store.
					sizeof(Preferences.SlowVRAMWorkAround));

	// Save stylus settings.
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_STYLUS_SETTING),				// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&Preferences.StylusControl,			// A pointer to the data to store.
					sizeof(Preferences.StylusControl));

	// Save auto sleep setting
//	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
//					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
//					&CString(REG_POWER_SAVE_SETTING),			// Name of the value to store.
//					REG_BINARY,									// Specifies the format for storing the data.
//					(char*)&Preferences.AutoSleepTimeOut,		// A pointer to the data to store.
//					sizeof(Preferences.AutoSleepTimeOut));
//
	// Save out default difficulty
//	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
//					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
//					&CString(REG_DIFFICULTY_SETTING),			// Name of the value to store.
//					REG_BINARY,									// Specifies the format for storing the data.
//					(char*)&DefaultDifficulty,					// A pointer to the data to store.
//					sizeof(DefaultDifficulty));

		
	// Save out default difficulty
	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_DURATION_SETTING),				// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&DefaultLength,						// A pointer to the data to store.
					sizeof(DefaultLength));

	return true;
}

void		CGameData::SetDemoTimeOut(int a)	
{
	ShowDemoTimeOut = a;

	SaveToRegistry(	HKEY_CURRENT_USER,							// Existing key or default root.
					&CString(REGISTRY_PATH),					// Sub key, relative to _ExistingKey. (Must not start with a '/' char!)
					&CString(REG_DEMO_TIMEOUT_SETTING),			// Name of the value to store.
					REG_BINARY,									// Specifies the format for storing the data.
					(char*)&ShowDemoTimeOut,					// A pointer to the data to store.
					sizeof(ShowDemoTimeOut));
}


// ----
// Returns whether then game is a home or away game
bool		CGameData::IsHomeGame()
{
	switch (GameType)
	{
	case LEAGUE:
	case LEAGUE_TEAM_MANAGER:
		if (LeagueRound < (NUM_OF_TEAMS_PER_DIVISION - 1))
			return (LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B % 2);		// 'Home'
		else
			return (!(LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B % 2));

	case CUP:
		return (!(CupLeg % 2));

	case KNOCKOUT:
	case PRACTICE:
	case DEMO:
	default:	
		return true;
	}
}

// ----
bool	CGameData::UpdateLeagueState()
{
	int i, j;

	// Update game states - num games playes/won/lost/drawn.
	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (j = (NUM_OF_TEAMS_PER_DIVISION >> 1); j--;)
		{
			CTeam	*TeamA	= Teams[LeagueFixtures[LeagueFixturesOrder[LeagueRound]][j].A + i * NUM_OF_TEAMS_PER_DIVISION];
			CTeam	*TeamB	= Teams[LeagueFixtures[LeagueFixturesOrder[LeagueRound]][j].B + i * NUM_OF_TEAMS_PER_DIVISION];

			if (TeamA->Score > TeamB->Score)
			{
				TeamA->IncWin(TeamB->Score);
				TeamB->IncLost(TeamA->Score);
			}
			else if (TeamA->Score < TeamB->Score)
			{
				TeamA->IncLost(TeamB->Score);
				TeamB->IncWin(TeamA->Score);
			}
			else
			{
				TeamA->IncDrawn(TeamB->Score);
				TeamB->IncDrawn(TeamA->Score);
			}
		}
	}

	// Store last team played
	LastTeamPlayed	= PlayerTwo;

	// Move to next game in league, and test to see if league over
	LeagueRound++;
	if (IsLeagueOver())
	{
		return false;	// Means the league is over.
	}
	else
	{
		// Determine next opponent
		PlayerOne		= Teams[0];	// Human team. 
		//PlayerTwo		= Teams[LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B + ((1 - PlayerOne->GetDivision()) * NUM_OF_TEAMS_PER_DIVISION)];

		// Do you want to know how much I hate arrays now?
		PlayerTwo		= Division[Teams[0]->GetDivision()][LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B];
		return true;
	}
}

// ----
// True if there are no more games to play.
bool		CGameData::IsLeagueOver()
{
	return (LeagueRound >= (2 * (NUM_OF_TEAMS_PER_DIVISION - 1)));
}

// ----
// True if the above is true and the human team has the 
// most points in the top division.
bool		CGameData::DidHumanWinLeague()
{
	int		highestScoringTeamIndex	= -1;
	int		highestScore			= -1;
	CTeam	*TRef;

	if (Teams[0]->GetDivision() != 0)
		return false;

	for (int i = NUM_OF_TEAMS_PER_DIVISION; i--;)
	{
		TRef = GetTeamFromDivision(i, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
		if (TRef->GetPoints() > highestScore)
		{
			highestScore			= TRef->GetPoints();
			highestScoringTeamIndex	= i;
		}
	}

	TRef = GetTeamFromDivision(highestScoringTeamIndex, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
	return (	(TRef)
			&&	(TRef->GetTeamNumber() == BRUTAL_DELUXE) 
			&&	(IsLeagueOver())
			);
}

// ----
bool		CGameData::DidHumanGetPromotion()
{
	int		highestScoringTeamIndex	= -1;
	int		highestScore			= -1;
	CTeam	*TRef;

	for (int i = NUM_OF_TEAMS_PER_DIVISION; i--;)
	{
		TRef = GetTeamFromDivision(i, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
		if (TRef->GetPoints() > highestScore)
		{
			highestScore			= TRef->GetPoints();
			highestScoringTeamIndex	= i;
		}
	}

	TRef = GetTeamFromDivision(highestScoringTeamIndex, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
	return (	(TRef)
			&&	(TRef->GetTeamNumber() == BRUTAL_DELUXE) 
			&&	(IsLeagueOver())
			);
}

// ----
bool		CGameData::DidHumanGetRelegated()
{
	int		lowestScoringTeamIndex	= -1;
	int		lowestScore				= -1;
	CTeam	*TRef;

	for (int i = NUM_OF_TEAMS_PER_DIVISION; i--;)
	{
		TRef = GetTeamFromDivision(i, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
		if (	(lowestScore < 0)
			||	(TRef->GetPoints() < lowestScore))
		{
			lowestScore			= TRef->GetPoints();
			lowestScoringTeamIndex	= i;
		}
	}

	TRef = GetTeamFromDivision(lowestScoringTeamIndex, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
	return (	(TRef)
			&&	(TRef->GetTeamNumber() == BRUTAL_DELUXE) 
			&&	(IsLeagueOver())
			);
}


// ----
bool	CGameData::LeagueGenerateScores()
{
	int i, j;
	
	int	Column = rand() % SCORE_ENTRIES;

	// Determine other teams scores
	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (j = (NUM_OF_TEAMS_PER_DIVISION >> 1); j--;)
		{
			// Generate scores for the two teams passed in.
			int TeamIntA = LeagueFixtures[LeagueFixturesOrder[LeagueRound]][j].A + (NUM_OF_TEAMS_PER_DIVISION * i);
			int TeamIntB = LeagueFixtures[LeagueFixturesOrder[LeagueRound]][j].B + (NUM_OF_TEAMS_PER_DIVISION * i);
			if (	(TeamIntA < 0) 
				||	(TeamIntA >= (NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS))
				||	(TeamIntB < 0) 
				||	(TeamIntB >= (NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS)))
				return false;

			Teams[TeamIntA]->Score = *(byte*)&ScoreData[TeamIntA * SCORE_ENTRIES + Column];//ScoresArray[TeamIntA][Column];
			Teams[TeamIntB]->Score = *(byte*)&ScoreData[TeamIntB * SCORE_ENTRIES + Column];//ScoresArray[TeamIntB][Column];
		}
	}
	
	// Set human team, and their opponents score to zero.
	PlayerOne->Score	= 0;
	PlayerTwo->Score	= 0;

	return true;
}

bool		CGameData::KnockOutScore()
{
	if (KnockOutOpponent >= (NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS))
	{
		return false;
	}
			
	CTeam	*TeamA	= Teams[0];
	CTeam	*TeamB	= Teams[KnockOutOpponent];

	if (TeamA->Score > TeamB->Score)
	{
		TeamA->IncWin(TeamB->Score);
		TeamB->IncLost(TeamA->Score);
	}
	else if (TeamA->Score < TeamB->Score)
	{
		TeamA->IncLost(TeamB->Score);
		TeamB->IncWin(TeamA->Score);
	}
	else
	{
		TeamA->IncDrawn(TeamB->Score);
		TeamB->IncDrawn(TeamA->Score);
	}

	return true;
}


// ----
bool		CGameData::UpdateKnockOut()
{
	KnockOutOpponent++;
	if (KnockOutOpponent >= (NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS))
	{
		return false;
	}
			
	PlayerOne		= Teams[0];
	PlayerTwo		= Teams[KnockOutOpponent];

	return true;
}

// ----
bool		CGameData::CupGenerateScores()
{
	// Don't want to generate scores if human drew.
	if (CupState != ECS_Drew)
	{
		// double safety to prevent us from generating scores for lots of rounds.
		if (CupLeg < 2)
		{
			int Column = rand() % SCORE_ENTRIES;

			// Need to generate next round.
			for (int i = (1 << (NUM_OF_CUP_ROUNDS - 1 - CupRound)); i--;)
			{
				// Select the highest scoring 
				int TeamIntA = CupFixtures[CupRound][i].A;
				int TeamIntB = CupFixtures[CupRound][i].B;

				CTeam	*TeamA	= GetTeamByIndex(TeamIntA);
				CTeam	*TeamB	= GetTeamByIndex(TeamIntB);

				TeamA->Score =	*(byte*)&ScoreData[TeamIntA * SCORE_ENTRIES + Column];
				TeamB->Score =	*(byte*)&ScoreData[TeamIntB * SCORE_ENTRIES + Column];

				// CPU teams MUST never draw (too difficult to making them play off)
				// Thus, modify the scores for TeamB.
				if (	(CupLeg == 1) 
					&&	(	(TeamA->CupScore + TeamA->Score) 
						==	(TeamB->CupScore + TeamB->Score)
						)
					)
				{
					int initialScore = TeamB->Score;
					while (TeamB->Score == initialScore)
					{
						Column = rand() % SCORE_ENTRIES;
						TeamB->Score =	*(byte*)&ScoreData[TeamIntB * SCORE_ENTRIES + Column];
					}
				}
			}

			// Set human and opponent team's score to zero.
			Teams[CupFixtures[CupRound][0].A]->Score = 0;
			Teams[CupFixtures[CupRound][0].B]->Score = 0;
			return true;
		}
	}
	return false;
}

// ----
void	CGameData::UpdateCupState()
{
	// Something to modify cup round.
	CTeam	*TeamA;
	CTeam	*TeamB;
	int		i, j;
	int		team;
	int		FixList[16];
	int		TeamIntA, TeamIntB;

	if (CupRound < 0) 
	{
		// Right, need to initialise teams for first round.
		CupRound	= 0;	// First leg.
		CupLeg		= 0;	// First round.

		FixList[0] = 0;
		for (i = 1; i < 16; i++)
		{
			bool repeat = true;
			while (repeat)
			{
				// Generate random team value.
				team = rand() >> 11;
				repeat = false;	// Assume that it is unique.

				// Check that it is actually unique.
				for (j = 0; j < i; j++)
				{
					if (FixList[j] == team)
					{
						// Not unique!
						repeat = true;
						break;
					}
				}
			} // End while loop

			// The 'team' will now be unique, thus add to list.
			FixList[i] = team;
		}

		// 
		for (i = NUM_OF_CUP_ROUNDS; i--;)
		{
			for (j = MAX_CUP_FIXTURES; j--;)
			{
				CupFixtures[i][j].A = -1; // Define all as un-used.
				CupFixtures[i][j].B = -1; // Define all as un-used.
			}
		}

		// Now all teams have been randomly drawn (apart form Teams[0] which is always at element zero).
		// Repackage data...
		CupFixtures[CupRound][0].A = FixList[0];
		CupFixtures[CupRound][0].B = FixList[1];
		CupFixtures[CupRound][1].A = FixList[2];
		CupFixtures[CupRound][1].B = FixList[3];
		CupFixtures[CupRound][2].A = FixList[4];
		CupFixtures[CupRound][2].B = FixList[5];
		CupFixtures[CupRound][3].A = FixList[6];
		CupFixtures[CupRound][3].B = FixList[7];
		CupFixtures[CupRound][4].A = FixList[8];
		CupFixtures[CupRound][4].B = FixList[9];
		CupFixtures[CupRound][5].A = FixList[10];
		CupFixtures[CupRound][5].B = FixList[11];
		CupFixtures[CupRound][6].A = FixList[12];
		CupFixtures[CupRound][6].B = FixList[13];
		CupFixtures[CupRound][7].A = FixList[14];
		CupFixtures[CupRound][7].B = FixList[15];

		PlayerOne		= Teams[0];			
		PlayerTwo		= Teams[CupFixtures[CupRound][0].B];
		//PlayerOne		= NULL;
		//PlayerTwo		= NULL;
		LastTeamPlayed	= NULL;
		CupState		= ECS_Normal;

		return;
	}
	else
	{
		LastTeamPlayed	= PlayerTwo;

		// If this is one of the two compulsory matches then add the scores
		if (CupLeg < 2)
		{
			// Store this legs points and clear the score for next time around.
			for (i = (1 << (NUM_OF_CUP_ROUNDS - 1 - CupRound)); i--;)
			{
				TeamA = Teams[CupFixtures[CupRound][i].A];
				TeamA->CupScore += TeamA->Score;

				TeamB = Teams[CupFixtures[CupRound][i].B];
				TeamB->CupScore += TeamB->Score;
				
				if (TeamA->Score > TeamB->Score)
				{
					TeamA->IncWin(TeamB->Score);
					TeamB->IncLost(TeamA->Score);
				}
				else if (TeamA->Score < TeamB->Score)
				{
					TeamB->IncWin(TeamA->Score);
					TeamA->IncLost(TeamB->Score);
				}
				else
				{
					TeamB->IncDrawn(TeamA->Score);
					TeamA->IncDrawn(TeamB->Score);
				}

				// Set match scores to zero.
				TeamA->Score = 0;
				TeamB->Score = 0;
			}
		}
		else
		{
			// Only add scores for human and opponents
			TeamA = Teams[CupFixtures[CupRound][0].A];
			TeamA->CupScore += TeamA->Score;

			TeamB = Teams[CupFixtures[CupRound][0].B];
			TeamB->CupScore += TeamB->Score;

			if (TeamA->Score > TeamB->Score)
			{
				TeamA->IncWin(TeamB->Score);
				TeamB->IncLost(TeamA->Score);
			}
			else if (TeamA->Score < TeamB->Score)
			{
				TeamB->IncWin(TeamA->Score);
				TeamA->IncLost(TeamB->Score);
			}
			else
			{
				TeamB->IncDrawn(TeamA->Score);
				TeamA->IncDrawn(TeamB->Score);
			}

			TeamA->Score = 0;
			TeamB->Score = 0;
		}

		// Right, firstly, have we played (at least) two matches yet?
		if (CupLeg > 0)
		{
			// Yes, we have played (at least) two matches - time to evaluate performance.

			// Erm, need to determine if the human player won, lost or drew.
			// Win - then line up next round
			// Lost	- return false - kill game. (set an additional state flag to say that it was because team lost)
			// Drew - the user can't see the results for the other games - thus simply reevaluate those games too.

			TeamA = Teams[0];							// Human team
			TeamB = Teams[CupFixtures[CupRound][0].B];	// Human opponents

			if (TeamA->CupScore > TeamB->CupScore)
			{
				// Won.
				
				// Won the whole Cup?
				if (CupRound >= (NUM_OF_CUP_ROUNDS - 1))
				{
					// Won the whole thing.
					// Return false
					// Set 'cup state' to won.
					CupState = ECS_WonCup;
					return;
				}
				else
				{
					// No, just won the last round.
					// Need to generate next round.
					for (i = (1 << (NUM_OF_CUP_ROUNDS - 1 - (CupRound + 1))); i--;)
					{
						// Select the highest scoring 
						TeamIntA = CupFixtures[CupRound][(2 * i) + 0].A;
						TeamIntB = CupFixtures[CupRound][(2 * i) + 0].B;
						CupFixtures[CupRound + 1][i].A =  ((Teams[TeamIntA]->CupScore >  Teams[TeamIntB]->CupScore) ? TeamIntA:TeamIntB);

						TeamIntA = CupFixtures[CupRound][(2 * i) + 1].A;
						TeamIntB = CupFixtures[CupRound][(2 * i) + 1].B;
						CupFixtures[CupRound + 1][i].B =  ((Teams[TeamIntA]->CupScore >  Teams[TeamIntB]->CupScore) ? TeamIntA:TeamIntB);

						// Clear the scores
						Teams[CupFixtures[CupRound + 1][i].A]->Score	= 0;
						Teams[CupFixtures[CupRound + 1][i].A]->CupScore	= 0;
						Teams[CupFixtures[CupRound + 1][i].B]->Score	= 0;
						Teams[CupFixtures[CupRound + 1][i].B]->CupScore	= 0;
					}

					// Update remaining state info, and exit.
					CupState	= ECS_Normal;	// Won through this round
					CupRound++;					// Inc round count
					CupLeg		= 0;			// First leg

					// Assign new teams
					PlayerOne = Teams[0];			
					PlayerTwo = Teams[CupFixtures[CupRound][0].B];
					return;
				}
			}
			else if (TeamA->CupScore < TeamB->CupScore)
			{
				// Lost game
						// Return false and set 'cup state' to failed. 
				CupState = ECS_Lost;		// Going home
				return;
			}
			else
			{
				// Must have drawn last game. 
				// Leave everything un-modified. 
				// Set a 'cup state' to replay.
				CupState = ECS_Drew;	// Drew game
				CupLeg++;				// Play it again (and keep on replaying it until there is an overall winner).
				
				// Just ensure that the players are correctly assigned.
				PlayerOne = Teams[0];			
				PlayerTwo = Teams[CupFixtures[CupRound][0].B];
				return;
			}
		}
		else
		{
			// Time to play the second leg.
			CupState	= ECS_Normal;
			CupLeg		= 1;	// Second leg.

			PlayerOne = Teams[0];			
			PlayerTwo = Teams[CupFixtures[CupRound][0].B];
			return;
		}
	}


	CupState		= ECS_Error;

/*
	// Cup array element zero will always be used to indicate the human team. (Is that clear?).
	for (i = NUM_OF_CUP_ROUNDS; i--;)
	{
		CupFixtures[i][0].A = 0;
	}

	PlayerOne		= Teams[CupFixtures[CupRound][0].A]; // Should always turn out to be Teams[0]. 
	LastTeamPlayed	= PlayerTwo;
	PlayerTwo		= Teams[CupFixtures[CupRound][0].B];
	*/
}

// ----
// Sort League Ascending Order
int SLAO(const void *elem1, const void *elem2)
{
	if (	(elem1)
		&&	(elem2))
	{
		CTeam*	TeamA	= *(CTeam**)elem1;
		CTeam*	TeamB	= *(CTeam**)elem2;
		int		ret = 0;

		// Points
		// For
		// Against

		if (TeamB->GetPoints() > TeamA->GetPoints())
			ret += 4;
		else if (TeamB->GetPoints() < TeamA->GetPoints())
			ret -= 4;

		if (TeamB->GetFor()	> TeamA->GetFor())
			ret += 2;
		else if (TeamB->GetFor() < TeamA->GetFor())
			ret -= 2;

		if (TeamA->GetAgainst()	> TeamB->GetAgainst())
			ret++;
		else if (TeamA->GetAgainst() < TeamB->GetAgainst())
			ret--;

		return ret;
	}
	return 0;
}

// Sort teams in leagues
bool		CGameData::SortLeagues()
{
	int i, j;

	// Copy data to reflect any changes in the division line-up.
	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (int j = NUM_OF_TEAMS_PER_DIVISION; j--;)
		{
			SortedDivision[i][j]	= Division[i][j];
		}
	}

	for (i = NUM_OF_DIVISIONS; i--;)
	{
		qsort((void*)&SortedDivision[i][0], NUM_OF_TEAMS_PER_DIVISION, sizeof(CTeam*), SLAO);
	}


	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
		{
			SortedDivision[i][j]->SetPlacing(j + 1);
		}
	}

	return true;
}

// ----
// Load a game
//bool	CGameData::LoadGame(CString*	_FileName)
//{
//	return true;
//}


// ----
/*CString*	CGameData::GetPlayerName(int _PlayerIndex)
{
	if (_PlayerIndex == 0)
		return &PlayerName[_PlayerIndex];
	else
		return NULL;
}*/

// ----
CTeam*		CGameData::GetActiveTeamByIndex(int _Index)
{
	switch (_Index)
	{
	case 0:
		return PlayerOne;
	case 1:
		return PlayerTwo;
	default:
		return NULL;
	}
}

// ----
CTeam*		CGameData::GetTeamFromDivision(	int _Index, 
											int _Division)
{
	int Div;
	if (_Division >= 0)
	{
		if (_Division < NUM_OF_DIVISIONS)
		{
			Div = _Division;
		}
		else		// Out of bound positive.
			return NULL;
	}
	else
	if (_Division == SELECT_FROM_HUMAN_PLAYERS_DIVISION)
		Div = PlayerOne->GetDivision(); // Careful! Is this what you want?
	else
		return NULL;
	// Div is now valid, or we have returned NULL.
	
	if (	(_Index >=	0)
		&&	(_Index <	NUM_OF_TEAMS_PER_DIVISION))
	{
		return Division[Div][_Index];
	}
	
	if (_Index == GET_HUMAN_TEAM)
		return PlayerOne;
	
	if (_Index == GET_CURRENT_OPPONENT)
		return PlayerTwo;

	if (_Index == GET_LAST_OPPONENTS)
		return LastTeamPlayed;

	// Nothing valid was selected.
	return NULL;
}

// ----
CTeam*		CGameData::GetTeamFromSortedDivision(	int _Index, 
													int _Division)
{
	int Div;
	if (_Division >= 0)
	{
		if (_Division < NUM_OF_DIVISIONS)
		{
			Div = _Division;
		}
		else		// Out of bound positive.
			return NULL;
	}
	else
	if (_Division == SELECT_FROM_HUMAN_PLAYERS_DIVISION)
		Div = PlayerOne->GetDivision();
	else
		return NULL;
	// Div is now valid, or we have returned NULL.
	
	if (	(_Index >=	0)
		&&	(_Index <	NUM_OF_TEAMS_PER_DIVISION))
	{
		return SortedDivision[Div][_Index];
	}
	
	// Nothing valid was selected.
	return NULL;
}

// ----
// Returns a team according to position in the underlying array structure.
// You should not really need this method too much.
CTeam*		CGameData::GetTeamByIndex(int _Index)
{
	if (	(_Index < 0)
		||	(_Index >= (NUM_OF_TEAMS_PER_DIVISION * NUM_OF_DIVISIONS)))
		return NULL;
	else
		return Teams[_Index];
}

// ----
void	CGameData::PrepareForLeaguePlayOff()
{
	// Sort the 'proper' division info by score.
	for (int i = NUM_OF_DIVISIONS; i--;)
	{
		qsort((void*)&Division[i][0], NUM_OF_TEAMS_PER_DIVISION, sizeof(CTeam*), SLAO);
	}
}

// ----
bool	CGameData::IsHumanInLeaguePlayOff()
{
	if (	(	(GameType	==	LEAGUE) 
			||	(GameType	==	LEAGUE_TEAM_MANAGER))
		&&	(IsLeagueOver()))
	{
		LeagueHumanPlayOff =	(	(Division[0][NUM_OF_TEAMS_PER_DIVISION - 2]->GetTeamNumber() == BRUTAL_DELUXE)
								||	(Division[1][1]->GetTeamNumber() == BRUTAL_DELUXE));
		return LeagueHumanPlayOff;
	}

	LeagueHumanPlayOff = false;
		 
	return false;
}

// ----
// Only to be called if 'IsHumanInLeaguePlayOff' returns true.
void	CGameData::AssignPlayOffTeams()
{
	if (Division[0][NUM_OF_TEAMS_PER_DIVISION - 2]->GetTeamNumber() == BRUTAL_DELUXE)
	{
		PlayerOne = Division[0][NUM_OF_TEAMS_PER_DIVISION - 2];
		PlayerTwo = Division[1][1];
	}
	else
	{
		PlayerOne = Division[1][1];
		PlayerTwo = Division[0][NUM_OF_TEAMS_PER_DIVISION - 2];
	}

	// Reset all counts for the two opponents.
	// PlayerOne
	PlayerOne->SetPlayed(0);
	PlayerOne->SetWon(0);
	PlayerOne->SetDrawn(0);
	PlayerOne->SetLost(0);

	PlayerOne->Score = 0;
	PlayerOne->SetPoints(0);
	PlayerOne->SetFor(0);
	PlayerOne->SetAgainst(0);

	// PlayerTwo
	PlayerTwo->SetPlayed(0);
	PlayerTwo->SetWon(0);
	PlayerTwo->SetDrawn(0);
	PlayerTwo->SetLost(0);

	PlayerTwo->Score = 0;
	PlayerTwo->SetPoints(0);
	PlayerTwo->SetFor(0);
	PlayerTwo->SetAgainst(0);

	// No last team played.
	LastTeamPlayed = NULL;
}

// ----
void	CGameData::PromoteRelegateNonplayOffTeams()
{
	CTeam*	TRef;
		
	// Top / bottom teams
	TRef = Division[0][NUM_OF_TEAMS_PER_DIVISION - 1];
	Division[0][NUM_OF_TEAMS_PER_DIVISION - 1]	= Division[1][0];
	Division[1][0] = TRef;

	// Update division info.
	Division[0][NUM_OF_TEAMS_PER_DIVISION - 1]->SetDivision(0);
	Division[1][0]->SetDivision(1);
}

void	CGameData::SwapPlayOffTeams()
{
	CTeam*	TRef;
		
	// PlayOff teams
	TRef = Division[0][NUM_OF_TEAMS_PER_DIVISION - 2];
	Division[0][NUM_OF_TEAMS_PER_DIVISION - 2]	= Division[1][1];
	Division[1][1] = TRef;

	// Update division info.
	Division[0][NUM_OF_TEAMS_PER_DIVISION - 2]->SetDivision(0);
	Division[1][1]->SetDivision(1);
}

// 
void	CGameData::ResolveCPUOnlyPlayOff()
{
	PromoteRelegateNonplayOffTeams();

	// Very complicated way of working out if the CPU teams will swap.
	if (rand() % 2)
	{
		SwapPlayOffTeams();
	}
}

// ----
// Returns false to indicate that the teams in question are still drawn.
// Otherwise returns true after swapping teams.
bool	CGameData::ResolveHumanPlayOff()
{
	if (PlayerOne->Score > PlayerTwo->Score)
	{
		PlayerOne->IncWin(PlayerTwo->Score);
		PlayerTwo->IncLost(PlayerOne->Score);
	}
	else if (PlayerOne->Score < PlayerTwo->Score)
	{
		PlayerOne->IncLost(PlayerTwo->Score);
		PlayerTwo->IncWin(PlayerOne->Score);
	}
	else
	{
		PlayerOne->IncDrawn(PlayerTwo->Score);
		PlayerTwo->IncDrawn(PlayerOne->Score);
	}

	// Reset scores.
	PlayerOne->Score = 0;
	PlayerTwo->Score = 0;


	// ----
	// Now check the points situation.
	if (PlayerOne->GetPoints() > PlayerTwo->GetPoints())
	{
		// Human won, but what does this mean? - Check which league the human is in...
		if (Teams[0]->GetDivision() == 0)
		{
			// In top league, this stay up.
			return true;
		}
		else
		{
			SwapPlayOffTeams();
			return true;
		}
	}
	else if (PlayerOne->GetPoints() < PlayerTwo->GetPoints())
	{
		// Human lost, but what does this mean? - Check which league the human is in...
		if (Teams[0]->GetDivision() == 0)
		{
			// In top league, going down!
			SwapPlayOffTeams();
			return true;
		}
		else
		{
			// Failed to win promotion.
			return true;
		}
	}

	// No winner / loser.
	return false;
}


// ----
void	CGameData::NewLeagueSeason()
{
	int i,j;

	LeagueHumanPlayOff = false;
	LeagueRound = 0;

	for (i = NUM_OF_DIVISIONS; i--;)
	{
		for (j = NUM_OF_TEAMS_PER_DIVISION; j--;)
		{
			// Games info
			Division[i][j]->SetPlayed(0);
			Division[i][j]->SetWon(0);
			Division[i][j]->SetDrawn(0);
			Division[i][j]->SetLost(0);

			// Score & points
			Division[i][j]->Score = 0;
			Division[i][j]->SetPoints(0);
			Division[i][j]->SetFor(0);
			Division[i][j]->SetAgainst(0);
		}
	}

	// Now need to ensure that Brutal is in element zero of one of the divisions.
	if (Division[Teams[0]->GetDivision()][0]->GetTeamNumber() != 0)
	{
		int BDAt = -1;

		// Not in the right place, therefore need to loop and find where we are...
		for (i = NUM_OF_TEAMS_PER_DIVISION; i--;)
		{
			if (Division[Teams[0]->GetDivision()][i]->GetTeamNumber() == 0)
			{
				BDAt = i;
				break;
			}
		}

		// BDAt should be valid (i.e. between 1 and 7, inclusive).
		ASSERT((BDAt > 0) && (BDAt < 8)); // Bound to go wrong...
		
		// Swap over the values.
		Division[Teams[0]->GetDivision()][BDAt]	= Division[Teams[0]->GetDivision()][0];
		Division[Teams[0]->GetDivision()][0]	= Teams[0];
	}

	// Sort the leagues.
	SortLeagues();

	// Randomise the fixture order
	RandomiseLeagueFixturesOrder();

	// Determine first opponent
	PlayerOne		= Teams[0];	// Human team. 
	PlayerTwo		= Division[Teams[0]->GetDivision()][LeagueFixtures[LeagueFixturesOrder[LeagueRound]][0].B];
	LastTeamPlayed  = NULL;
}


// ----
void	CGameData::GetFixture(IntCouple* _OpposingTeams, int _MatchIndex)//, int _Division)
{
	if (	(_OpposingTeams) 
		&&	(_MatchIndex >= 0)
		&&	(_MatchIndex < (NUM_OF_TEAMS_PER_DIVISION >> 1)))
	{
		//int TeamOffset = 0;
		//if (_Division == 0)	// i.e. Division 1
		//	TeamOffset = NUM_OF_TEAMS_PER_DIVISION; 

		_OpposingTeams->A	= LeagueFixtures[LeagueFixturesOrder[LeagueRound]][_MatchIndex].A;// + TeamOffset;
		_OpposingTeams->B	= LeagueFixtures[LeagueFixturesOrder[LeagueRound]][_MatchIndex].B;// + TeamOffset;
	}
	else
		OutputDebugString(TEXT("Error in GetFixture\n"));
}
