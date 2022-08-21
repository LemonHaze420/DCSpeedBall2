#ifndef		_TEAM_CPP
#define		_TEAM_CPP

/***-*-**-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
*
*	The team class holds references to all team members
*	and holds the values for team stats, money and
*  other stuff
*
*
* * * * * * * * * * * * * / * * * * * * */

#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"Actor_TeamMember.cpp"
//#include	"NotSoRandomStuff.h"
#include	"MyString.h"

#define UP		3
#define DOWN	1
#define STEADY	2

class CTeam : public CObject
{
protected:
	
	//CNotSoRandomStuff* random;
	//Team stuff
	
	CString			TeamName;
	CString			ShortTeamName;
	int				TeamNumber; //As defined in GameDefines
	int				Cash;
	CTeamMember*	Player;
	CTeamMember*	TeamPlayers[12]; //an array of pointers to the people who play for the team.
	
	//Game Stats
	int NumGamesPlayed;
	int NumGamesWon;
	int NumGamesDrawn;
	int NumGamesLost;
	
	//Team average stats
/*	int DefenceStrength;
	int MidfieldStrength;
	int AttackStrength;
	int SubstituesStrength;
*/	
	//League Specific Stuff
	int LeagueTable;	//division 1 or 2
	int LeaguePlacing;
	int LeagueMovement; //ie. going up down or steady.
	int PointsFor;
	int PointsAgainst;
	int Points;

	//substtiution count... keeps track of which sub to use;
	int subCount;
	
public:
	bool	ControlledBy; //true = human
	bool	ShootingUp;	 //false =shooting down
	int		Score;
	int		CupScore;		// Stores the combined score for the two cup games.

	//LastMatchStats
	int ShotsAtGoal;
	int GoalsScored;
	int GoalsSaved;
	int TimeInPossesion;
	int TimeInOppenentsHalf;
	int SuccesfulTackles;
	int Substitutions;
	int BonusPointsScored;
	
	//default constructor is empty... so far!! muhahahaha
	CTeam::CTeam()
	{
//		random = NULL;
		Init();
	}
	
	//deconstructor cleans up all the player objects.
	CTeam::~CTeam()
	{
		for(int i = 0 ; i < 12 ; i++)
		{
			if (TeamPlayers[i])
			{
				delete(TeamPlayers[i]);
			}
		}

		//SAFELY_DELETE(random);
	}

	// ----
	// Sets everything to reasonable values
	void CTeam::Init()
	{
	//	SAFELY_DELETE(random);
	//	NEW(random, CNotSoRandomStuff());
	//	random->SetRandomName();

		//TeamName;									// Pre-inited
		//ShortTeamName
		TeamNumber				= BRUTAL_DELUXE;
		Cash					= 0;
		Player					= NULL;

		for (int i = 0; i < 12; i++)
			TeamPlayers[i] = NULL;
	
		NumGamesPlayed			= 0;
		NumGamesWon				= 0;
		NumGamesDrawn			= 0;
		NumGamesLost			= 0;
//		NextFixture				= 0;
//		LastFixture				= 0;
	
		// Are these used? - I calculate them when I need them!!!
/*		DefenceStrength			= 0;
		MidfieldStrength		= 0;
		AttackStrength			= 0;
		SubstituesStrength		= 0;
*/		
		LeagueTable				= 1;	// Start in league 2
		LeaguePlacing			= 0;	// Don't know - does it matter?
		LeagueMovement			= 0;
		PointsFor				= 0;
		PointsAgainst			= 0;
		Points					= 0;
		
		ShotsAtGoal				= 0;
		GoalsScored				= 0;
		GoalsSaved				= 0;
		TimeInPossesion			= 0;
		TimeInOppenentsHalf		= 0;
		SuccesfulTackles		= 0;
		Substitutions			= 0;
		BonusPointsScored		= 0;
	
		subCount				= 0;	// Should this be an attribute of CMatch? Not sure what it is intended for.
	
		ControlledBy			= 0;
		ShootingUp				= 0;
		Score					= 0;
		CupScore				= 0;	// Cup score.
	}
	
	//constructor creates a default starting human team.
	CTeam::CTeam(int GameType)
	{
//		random = NULL;

		Init();
		
		/*if (	(GameType == LEAGUE)
			||	(GameType == LEAGUE_TEAM_MANAGER))
		{*/
			LeaguePlacing	= 1;
			LeagueMovement	= UP;
			PointsFor		= 0;
			PointsAgainst	= 0;
			Points			= 0;
		/*}
		else
		{
			LeaguePlacing	= -1;		// What does "-1" mean? Why?
			LeagueMovement	= -1;
			PointsFor		= -1;
			PointsAgainst	= -1;
			Points			= -1;
		}*/
		
		ControlledBy = true;			// true == human control
		TeamName = "Brutal Deluxe";
		ShortTeamName = "Brutal Dlx";
		TeamNumber = BRUTAL_DELUXE;
		
		if (GameType == CUP)
		{
			Cash = 2000;
		}
		else
		{
			Cash = 1100;
		}
		
#ifdef DEBUG
		Cash = 90000;
#endif
		//random->SetRandomName();

		CString TempString;
		for( int x = 0; x < 12; x++)
		{
			NEW(Player, CTeamMember());
			TeamPlayers[x] = Player;
			TeamPlayers[x]->setPicture(x);
			TeamPlayers[x]->setPosition(x);	
			
			switch (x)
			{
			case 0:
				TempString = "BARRY";
				break;
			case 1:
				TempString = "COLIN";
				break;
			case 2:
				TempString = "JUSTIN";
				break;
			case 3:
				TempString = "NIGEL";
				break;
			case 4:
				TempString = "DARREN";
				break;
			case 5:
				TempString = "GRAHAM";
				break;
			case 6:
				TempString = "ARNOLD";
				break;
			case 7:
				TempString = "ROBIN";
				break;
			case 8:
				TempString = "TREVOR";
				break;
			case 9:
				TempString = "STUART";
				break;
			case 10:
				TempString = "GORDON";
				break;
			case 11:
				TempString = "KEVIN";
				break;
			default:
				TempString = "FOX";
			}

			TeamPlayers[x]->setName(&TempString);
		}
		
		
		NumGamesPlayed = 0;
		NumGamesWon = 0;
		NumGamesDrawn = 0;
		NumGamesLost = 0;
		//NextFixture = FixtureList[0]; 
//		LastFixture = -1;
		
/*		DefenceStrength = 100;
		MidfieldStrength = 100;
		AttackStrength = 100;
		SubstituesStrength = 100;
*/		
		ShootingUp = false;
		
		Score = 0;
		CupScore				= 0;	// Cup score.

//		delete(random);
		
	}
	
	CTeam::CTeam(int GameType, int theTeamNumber, bool isComputer, int Diff)
	{
//		random = NULL;
		Init();

		//NEW(random,CNotSoRandomStuff());
		/*if (	(GameType == LEAGUE)
			||	(GameType == LEAGUE_TEAM_MANAGER))
		{*/
			LeaguePlacing = TeamNumber;
			LeagueMovement = UP;
			PointsFor = 0;
			PointsAgainst = 0;
			Points = 0;
		/*}
		else
		{
			LeaguePlacing = -1;
			LeagueMovement = -1;
			PointsFor = -1;
			PointsAgainst = -1;
			Points = -1;
		}*/
		
		ControlledBy = false;
		
		TeamNumber = theTeamNumber;
		
		
		for( int x = 0; x < 12; x++)
		{
			switch(theTeamNumber)
			{
			case 0:
				TeamName		= "Brutal Deluxe";
				ShortTeamName	= "Brutal Dlx";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 100,100,100,100,100,100,100,100,Diff));
					TeamPlayers[x] = Player;
					break;
					
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 100,100,100,100,100,100,100,100,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 100,100,100,100,100,100,100,100,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 100,100,100,100,100,100,100,100,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 1:
				TeamName		= "Revolver";
				ShortTeamName	= "Revolver";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 130,120,130,120,140,120,120,140,Diff));
					TeamPlayers[x] = Player;
					break;
					
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 	100,100,100,100,110,100,100,110,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 120,110,120,110,130,110,110,120,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 100,100,100,100,110,100,100,110,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 2:
				TeamName		= "Raw Messiahs";
				ShortTeamName	= "Messiahs";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x,150,120,110,110,110,110,110,130,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x,150,150,130,120,120,140,140,140,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x,150,140,120,120,120,130,130,130,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x,140,120,110,100,100,110,110,120,Diff ));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 3:
				TeamName		= "Violent Desire";
				ShortTeamName	= "Violent DS";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 110,150,150,160,160,150,150,160,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 110,120,120,130,130,120,120,140,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 110,130,130,140,140,130,130,140,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 110,120,120,130,130,120,120,130,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 4:
				TeamName		= "Baroque";
				ShortTeamName	= "Baroque";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 130,130,130,140,130,130,130,140,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,140,140,150,140,140,140,150,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,160,160,170,160,160,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 120,130,130,140,130,130,130,140,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 5:
				TeamName		= "The Renegades";
				ShortTeamName	= "Renegades";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,140,140,140,140,140,140,150,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 150,150,170,170,160,160,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 150,150,170,170,150,160,160,170,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,140,150,150,140,140,140,150,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 6:
				TeamName		= "Damocles";
				ShortTeamName	= "Damocles";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x,140,190,180,190,190,170,180,190,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 130,150,150,180,160,150,150,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,160,160,190,160,160,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x,130,160,160,170,160,150,150,170,Diff ));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 7:
				TeamName		= "Steel Fury";
				ShortTeamName	= "Steel Fury";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 150,160,160,190,160,160,160,180,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x,150,170,170,200,170,170,170,190,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 150,180,180,200,180,180,180,190,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 140,160,160,180,160,160,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 8:
				TeamName		= "Powerhouse";
				ShortTeamName	= "Powerhouse";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,150,150,160,150,150,150,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,160,170,170,170,160,160,180,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,170,180,180,170,180,180,190,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,150,150,160,160,150,150,170,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 9:
				TeamName		= "Rage";
				ShortTeamName	= "Rage";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 200,190,180,180,180,190,180,190,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 200,180,170,170,170,190,170,180,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 190,170,160,160,160,190,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,170,160,160,160,180,160,170,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 10:
				TeamName		= "Mean Machine";
				ShortTeamName	= "MN Machine";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,170,190,170,170,170,190,180,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,180,200,180,180,180,200,200,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x,160,190,210,190,190,190,210,200,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 150,170,190,170,170,170,190,180,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 11:
				TeamName		= "Explosive Lords";
				ShortTeamName	= "Expl Lords";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,200,210,210,210,200,200,220,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,200,210,210,210,200,200,210,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,170,190,190,170,170,170,190,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,180,190,190,180,180,180,180,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 12:
				TeamName		= "Lethal Formula";
				ShortTeamName	= "Lethal Frm";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 190,200,190,190,190,200,190,200,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 200,210,200,200,200,210,200,210,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 200,230,220,220,220,230,220,230,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,200,190,190,190,200,190,200,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 13:
				TeamName		= "Turbo Hammers";
				ShortTeamName	= "Turbo Hmrs";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x,180,240,240,220,240,230,230,230,Diff ));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,210,210,220,230,210,210,230,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,200,200,220,220,200,200,220,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,210,210,210,220,200,200,220,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			case 14:
				TeamName		= "Fatal Justice";
				ShortTeamName	= "Fatal Jstc";
 				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,210,210,230,210,230,220,220,Diff	));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,220,220,240,220,240,230,240,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,240,240,250,230,250,240,250,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,210,210,230,210,210,230,220,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			default:
				TeamName		= "Super Nashwan";
				ShortTeamName	= "Nashwan";
				switch(x)
				{
				case 0:
				case 1:
				case 2:
					NEW(Player, CTeamMember(theTeamNumber, x, 170,220,240,240,240,220,220,240,Diff));
					TeamPlayers[x] = Player;
					break;
				case 3:
				case 4:
				case 5:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,230,250,250,230,230,230,250,Diff));
					TeamPlayers[x] = Player;
					break;
				case 6:
				case 7:
				case 8:
					NEW(Player, CTeamMember(theTeamNumber, x, 180,230,250,250,230,230,230,250,Diff));
					TeamPlayers[x] = Player;
					break;
				case 9:
				case 10:
				case 11:
					NEW(Player, CTeamMember(theTeamNumber, x, 160,220,240,240,220,220,220,240,Diff));
					TeamPlayers[x] = Player;
					break;
				}
				break;
			}	  
		}
		
		for(int y = 0; y < 8; y++)
		{
			int p = y+theTeamNumber;
			while(p > 8)
			{
				p = p - 9 ;
			}
			//			FixtureList[y] = random->SetLeagueFixtures(y);
		}
		
		NumGamesPlayed = 0;
		NumGamesWon = 0;
		NumGamesDrawn = 0;
		NumGamesLost = 0;
		//		NextFixture = FixtureList[theTeamNumber]; //for comp teams... I think if they add their team num
		//To the fixture index It will work out.
//		LastFixture = -1;
		
	/*	DefenceStrength = WorkOutAverageDefenceStrength();
		MidfieldStrength = WorkOutAverageMidfieldStrength();
		AttackStrength = WorkOutAverageAttackStrength();
		SubstituesStrength = WorkOutAverageSubStrength();
	 */

		ShootingUp = true;
		Score = 0;
		CupScore				= 0;	// Cup score.
		
//		delete(random);
	}
	
	int		CTeam::WorkOutAverageDefenceStrength()
	{
		int Total;
		for(int i = 0 ; i < 3 ; i++)
		{
			Total += TeamPlayers[i]->GetAverageStats();
		}
		Total = Total/3;
		return Total;
	}
	
	int		CTeam::WorkOutAverageMidfieldStrength()
	{
		int Total;
		for(int i = 3 ; i < 6 ; i++)
		{
			Total += TeamPlayers[i]->GetAverageStats();
		}
		Total = Total/3;
		return Total;	
	}
	
	int		CTeam::WorkOutAverageAttackStrength()
	{
		int Total;
		for(int i = 6 ; i < 9 ; i++)
		{
			Total += TeamPlayers[i]->GetAverageStats();
		}
		Total = Total/3;
		return Total;	
	}
	
	int		CTeam::WorkOutAverageSubStrength()
	{
		int Total = 0;
		for(int i = 9 ; i < 12 ; i++)
		{
			Total += TeamPlayers[i]->GetAverageStats();
		}
		Total = Total/3;
		return Total;	
	}
	
	bool	CTeam::GetPossesion()
	{
		for(int i = 0; i < 9; i++)
		{
			if(TeamPlayers[i]->GetPossesion())
			{
				return true;
			}
		}
		return false;
	}
	
	CTeamMember* CTeam::GetTeamMember(int pos)
	{
		return TeamPlayers[pos];
	}

	void CTeam::SetTeamMember(int pos, CTeamMember* _TMRef)
	{
		TeamPlayers[pos] = _TMRef;
	}

	// Swap two players.
	bool CTeam::SwapTeamMembers(unsigned int posA, unsigned int posB)
	{
		if ((posA < 12) && 
			(posB < 12))
		{
			// Swap position info
			int pA = TeamPlayers[posA]->getPosition();		//	Store A's
			TeamPlayers[posA]->setPosition(TeamPlayers[posB]->getPosition()); // Copy B's to A's
			TeamPlayers[posB]->setPosition(pA); // Copy A's to B's

			// Swap players
			CTeamMember*	TMRef = TeamPlayers[posA];	
			TeamPlayers[posA] = TeamPlayers[posB];		
			TeamPlayers[posB] = TMRef;

			return true;
		}
		return false;
	}
	
	CTeamMember* CTeam::GetControlledTeamMember()
	{
		CTeamMember* temp;
		for(int i = 0; i < 9; i++)
		{
			temp = TeamPlayers[i];
			if (temp->IsControlled)
				return temp;
		}
		return NULL; //PLEASE DON'T LET THIS EVER HAPPEN... ever
	}
	
	void	CTeam::IncreaseScore(int points)
	{
		Score += points;
	}

	void	CTeam::setScore(int score)
	{
		Score = score;
	}
	
	void	CTeam::IncreaseBonusPoints(int points)
	{
		BonusPointsScored += points;
	}
	
	void	CTeam::DoSubstituion(CTeamMember* thePlayer)
	{
		if((subCount == 0) || (subCount > 11) )
		{
			subCount = 9;
		}
		
		CTeamMember* temp;
		int index = thePlayer->getPosition();
		
		SAFELY_DELETE(TeamPlayers[index]);
		
		TeamPlayers[index] = TeamPlayers[subCount];
		TeamPlayers[index]->setPosition(index);
		
		TeamPlayers[subCount] = NULL;
		
		NEW(temp, CTeamMember(subCount));
		TeamPlayers[subCount] = temp;

		CString	TempString;
		switch ((((rand() >> 7) * 12) / 256))
		{
		case 0:
			TempString = "GARETH";
			break;
		case 1:
			TempString = "ANDREW";
			break;
		case 2:
			TempString = "ANTHONY";
			break;
		case 3:
			TempString = "JOHN";
			break;
		case 4:
			TempString = "DAVE";
			break;
		case 5:
			TempString = "PHILIP";
			break;
		case 6:
			TempString = "SAM";
			break;
		case 7:
			TempString = "STEVE";
			break;
		case 8:
			TempString = "HARRY";
			break;
		case 9:
			TempString = "DAN";
			break;
		case 10:
			TempString = "JOE";
			break;
		case 11:
			TempString = "KURT";
			break;
		default:
			TempString = "ERIC";
		}
		TeamPlayers[subCount]->setName(&TempString);
		//now set who they play for and stuff
		TeamPlayers[subCount]->setTeam(TeamNumber);

		TeamPlayers[subCount]->DirectionOfPlay = TeamPlayers[0]->DirectionOfPlay;

		TeamPlayers[subCount]->setAgr(TeamPlayers[index]->getAgr());
		TeamPlayers[subCount]->setAtt(TeamPlayers[index]->getAtt());
		TeamPlayers[subCount]->setInt(TeamPlayers[index]->getInt());
		TeamPlayers[subCount]->setDef(TeamPlayers[index]->getDef());
		TeamPlayers[subCount]->setThr(TeamPlayers[index]->getThr());
		TeamPlayers[subCount]->setPow(TeamPlayers[index]->getPow());
		TeamPlayers[subCount]->setSta(TeamPlayers[index]->getSta());
		TeamPlayers[subCount]->setSpd(TeamPlayers[index]->getSpd());
		
		subCount++;
	}
	
	// Returns the amount of cash the team has available to spend.
	int	CTeam::GetBalance()
	{
		return	Cash;
	}
	
	void CTeam::SetBalance(int theCash)
	{
		Cash = theCash;
	}

	// Credit the team's account
	void	CTeam::CreditAccount(int theCash)
	{
		Cash += theCash;
	}

	// Debit the account
	void	CTeam::DebitAccount(int theCash)
	{
		Cash -= theCash;
	}

	// ----
	// Returns the teams average.
	int CTeam::GetAverageStats()
	{
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->GetAverageStats();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAverageInt()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getInt();
		}
		return (ret / 12);
	}
	
	// ---- 
	int	CTeam::GetAverageAgr()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getAgr();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAverageAtt()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getAtt();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAverageThr()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getThr();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAveragePow()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getPow();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAverageDef()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getDef();
		}
		return (ret / 12);
	}

	// ---- 
	int	CTeam::GetAverageSta()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getSta();
		}
		return (ret / 12);
	}
	
	// ---- 
	int	CTeam::GetAverageSpd()
	{	
		int ret = 0;
		for (int i = 12; i--;)
		{
			ret += TeamPlayers[i]->getSpd();
		}
		return (ret / 12);
	}

	// ----
	// Returns the defence average.
	int CTeam::GetAverageDefenceStats()
	{
		int ret = 0;
		for (int i = 3; i--;)
		{
			ret += TeamPlayers[i + 6]->GetAverageStats();
		}
		return (ret / 3);
	}

	// ----
	// Returns the attack average.
	int CTeam::GetAverageAttackStats()
	{
		int ret = 0;
		for (int i = 3; i--;)
		{
			ret += TeamPlayers[i]->GetAverageStats();
		}
		return (ret / 3);
	}

	// ----
	// Returns the midfield average.
	int CTeam::GetAverageMidfieldStats()
	{
		int ret = 0;
		for (int i = 3; i--;)
		{
			ret += TeamPlayers[i + 3]->GetAverageStats();
		}
		return (ret / 3);
	}

	// ----
	// Returns the substitute average.
	int CTeam::GetAverageSubsStats()
	{
		int ret = 0;
		for (int i = 3; i--;)
		{
			ret += TeamPlayers[i + 9]->GetAverageStats();
		}
		return (ret / 3);
	}

	// ----
	CString*	GetTeamName()			{return &TeamName;}
	int			GetTeamNumber()			{return TeamNumber;}
	CString*	GetShortTeamName()		{return &ShortTeamName;}
	int			GetPlayed()				{return NumGamesPlayed;}
	int 		GetWon()				{return NumGamesWon;}
	int 		GetDrawn()				{return NumGamesDrawn;}
	int 		GetLost()				{return NumGamesLost;}
	int 		GetDivision()			{return LeagueTable;}
	int 		GetPlacing()			{return LeaguePlacing;}
	int 		GetMovement()			{return LeagueMovement;} //ie. going up down or steady.
	int 		GetFor()				{return PointsFor;}
	int 		GetAgainst()			{return PointsAgainst;}
	int 		GetPoints()				{return Points;}

	//CString*	SetTeamName()			{return &TeamName;}
	void		SetTeamNumber(int _TeamNumber)	{TeamNumber = _TeamNumber;}
	void		SetShortTeamName(CString* _a)		{ShortTeamName = _a;}
	void		SetPlayed(int _a)				{NumGamesPlayed = _a;}
	void 		SetWon(int _a)					{NumGamesWon = _a;}
	void 		SetDrawn(int _a)				{NumGamesDrawn = _a;}
	void 		SetLost(int _a)					{NumGamesLost = _a;}
	void 		SetDivision(int _a)				{LeagueTable = _a;}
	void 		SetPlacing(int _a)				{LeaguePlacing = _a;}
	void 		SetMovement(int _a)				{LeagueMovement = _a;}
	void 		SetFor(int _a)					{PointsFor = _a;}
	void 		SetAgainst(int _a)				{PointsAgainst = _a;}
	void 		SetPoints(int _a)				{Points = _a;}

	// Modify the points, etc.
	void		IncWin(int _PointsAgainst)
	{
		NumGamesPlayed++; 
		NumGamesWon++; 
		Points			+= POINTS_FOR_LEAGUE_WIN;
		Points			+= (Score / 10);
		PointsFor		+= (Score / 10);
		PointsAgainst	+= (_PointsAgainst / 10);
	}
	void		IncDrawn(int _PointsAgainst)
	{
		NumGamesPlayed++; 
		NumGamesDrawn++; 
		Points			+= POINTS_FOR_LEAGUE_DRAW;
		Points			+= (Score / 10);
		PointsFor		+= (Score / 10);
		PointsAgainst	+= (_PointsAgainst / 10);
	}

	void		IncLost(int _PointsAgainst)
	{
		NumGamesPlayed++; 
		NumGamesLost++;
		Points			+= POINTS_FOR_LEAGUE_LOSE;
		Points			+= (Score / 10);
		PointsFor		+= (Score / 10);
		PointsAgainst	+= (_PointsAgainst / 10);
	}

	void	CTeam::ResetLastMatchStats()
	{
		ShotsAtGoal				= 0;
		GoalsScored				= 0;
		GoalsSaved				= 0;
		TimeInPossesion			= 0;
		TimeInOppenentsHalf		= 0;
		SuccesfulTackles		= 0;
		Substitutions			= 0;
		BonusPointsScored		= 0;
	}

	//void		ConvertScoreToPoints()	{Points += (Score / 10);}
};
#endif