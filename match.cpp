#include	"Match.h"
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"

#include	"graphicsclasses.h"
#include	"GameDefines.h"

// ----****************************************************************
bool	CMatch::Process()		
{
	bool ret = true;

	// Ignore all the rest for now....
#ifndef DONT_PROCESS_GAME

#ifndef	BOXEDSALE
	if (	(!GameDataRef->GetPreferences()->Registered)	// If this is an unregistered product....
		&&	(GameDataRef->GetGameType() != DEMO))			// and not a demo
	{
		if (TimePlayed > 45)								
		{
			MatchState = MatchAbandoned;					// then abandon the game after 45 seconds
		}
	}
#endif

	if (	(GameDataRef->GetGameType() == DEMO)
		&&	(	(ControllerRef->IsButtonPressed(Select)) 
			||	(ControllerRef->IsButtonPressed(Backup))
			)
		)
	{
		MatchState = MatchOver;
	}

#ifndef DEMO_BUILD
	if (	(ControllerRef->IsButtonPressedDB(Backup))
		&&	(MatchState != MatchOver))		// Don't allow to pause if match is over...
	{
		if(MatchState != Paused)
			PreviousMatchState = MatchState;

		MatchState = Paused;
	}
#endif

	ConserveMemory();

	if (MatchState == StartingMatch)
	{
		if(PlayCrowd == false)
		{
			PLAY_CROWD_LOOP;
			PlayCrowd = true;
		}
		this->GetEveryoneIntoStartingPositions();
		ResetRandomNumbers();
		MatchState = LaunchingBall;
		PLAY_GET;
		//PLAY_CROWD2;

		
		StartPauseCount = 50;//pause before we let any of the players go for the ball
	}

	if (MatchState == LaunchingBall)
	{
		if (StartPauseCount > 0)
		{
			StartPauseCount--;
		}

		//if(StartPauseCount == 42)
			//PLAY_BALL_LAUNCH;

		if(StartPauseCount == 35)
		{
			this->LaunchBall();
			LeftChute->setState(NONE);
			RightChute->setState(NONE);
		}
		if(StartPauseCount > 35)
		{
			if(StartPauseCount == 45)
			{
				PLAY_READY;
			}
		}
		else
		{
			this->ProcessBall();
		}

		if(StartPauseCount <= 15)
		{
			MatchState = PlayingGame;
		}
	}

	if (MatchState == PlayingGame)
	{
		this->ProcessMoves();
	}
	else
	{
		// The game ain't playing, so reset the counter.
		PrevClock = GetTickCount();
	}

	if (MatchState == DoingInjury)
	{
		this->CallTheMedics();
	}

	if (MatchState == DoingGoalSideA)
	{
		if(this->ProcessGoal(GameDataRef->GetActiveTeamByIndex(0), false, GameDataRef->GetActiveTeamByIndex(1)))
			MatchState = StartingMatch;
		
	}

	if (MatchState == DoingGoalSideB)
	{
		if(this->ProcessGoal(GameDataRef->GetActiveTeamByIndex(1), false,GameDataRef->GetActiveTeamByIndex(0)))
			MatchState = StartingMatch;
		
	}

	if (MatchState == DoingHomeGoalSideA)
	{
		if(this->ProcessGoal(GameDataRef->GetActiveTeamByIndex(1), true,GameDataRef->GetActiveTeamByIndex(0)))
			MatchState = StartingMatch;
		
	}

	if (MatchState == DoingHomeGoalSideB)
	{
		if(this->ProcessGoal(GameDataRef->GetActiveTeamByIndex(0), true, GameDataRef->GetActiveTeamByIndex(1)))
			MatchState = StartingMatch;
	}

	if(MatchState == DoingWinA)
	{
		this->ProcessMatchOver();
	}

	if(MatchState == DoingWinB)
	{
		this->ProcessMatchOver();
	}

	if(MatchState == DoingDraw)
	{
		this->ProcessMatchOver();
	}

	if(MatchState == Dancing)
	{
		this->ProcessMatchOver();
	}


#endif

	// Draw everything.
	
	ret &= Draw();

	

	return ret;
}

// ----***********************************************************************
bool	CMatch::IsGameOver()	
{
	return (MatchState == MatchOver);
}

bool	CMatch::IsGameAbandoned()
{
	return (MatchState == MatchAbandoned);
}

//*****************************************************************************
bool	CMatch::ProcessMoves()
{

	
	
	if(GameDataRef->GetGameType() != PRACTICE)
	{
		if(ProcessWho == true)
		{
#ifndef	DISABLE_OPPONENTS
			this->ProcessTeam(GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0));
#endif
			this->ProcessTeam(GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1));
			ProcessWho = false;
		}
		else
		{
			this->ProcessTeam(GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1));
#ifndef	DISABLE_OPPONENTS
			this->ProcessTeam(GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0));
#endif
			ProcessWho = true;
		}
	}
	else
	{
		this->ProcessTeam(GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1));
	}

	//add token and remove token and stuff
	this->ProcessTokens();
	//not too sure if this needs to be done... we will see.
	this->ProcessMapObjects();
	//process the ball
	this->ProcessBall();

	//GameDataRef->GetActiveTeamByIndex(0)->setScore(GetRandomNumber());

	TimeCount++;
	if(TimeCount > 20)
	{
		TimeCount = 0;
	}

	if (	((GetTickCount() - PrevClock) > 1000)	
		||	((GetTickCount() - PrevClock) < 0))
	{
		TimePlayed++;
		if(GameDataRef->GetActiveTeamByIndex(1)->GetPossesion())
			GameDataRef->GetActiveTeamByIndex(1)->TimeInPossesion++;
		else
			if(GameDataRef->GetActiveTeamByIndex(0)->GetPossesion())
				GameDataRef->GetActiveTeamByIndex(0)->TimeInPossesion++;

		PrevClock = GetTickCount();
	}
	
	if(		(TimePlayed == (GAME_PER_SIDE_DURATION - 1))
		&&	(ActualHalf != HalvesToPlay) 
		&&	(!IsAirhornPlaying))
	{
		PLAY_AIRHORN_HALF;
		IsAirhornPlaying = true;
	}

	if(		(TimePlayed == GAME_PER_SIDE_DURATION - 3)
			&&	(ActualHalf == HalvesToPlay) 
			&&	(!IsAirhornPlaying))
	{
		PLAY_AIRHORN_END;
		IsAirhornPlaying = true;
	}

	if(		(TimePlayed == GAME_PER_SIDE_DURATION)
		&&	(ActualHalf != HalvesToPlay) )
	{	
		CTeamMember* temp;
		

		for(int i = 0; i < 12; i++)
		{
			temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
			temp->DirectionOfPlay = !temp->DirectionOfPlay;
			temp->TokenEffectCount = 1;
		}

		for(i = 0; i < 12; i++)
		{
			temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
			temp->DirectionOfPlay = !temp->DirectionOfPlay;
			temp->TokenEffectCount = 1;
		}

		GameHalf = !GameHalf;
		ActualHalf++;

		MatchState	= StartingMatch;

		if(TeamOneHalf == 0)
			TeamOneHalf = 1;
		else
			TeamOneHalf = 0;

		TimePlayed = 0;
		PrevClock = GetTickCount();

		LeftBank->Reset();
		RightBank->Reset();
		LeftChute->setState(NONE);
		RightChute->setState(NONE);

		IsAirhornPlaying = false;
	}
	else
	{
		if(		(TimePlayed == GAME_PER_SIDE_DURATION)
			&&	(ActualHalf == HalvesToPlay) )
		{
			//PLAY_AIRHORN_END;
			int scoreOne = GameDataRef->GetActiveTeamByIndex(0)->Score;
			int scoreTwo = GameDataRef->GetActiveTeamByIndex(1)->Score;

			if( scoreOne > scoreTwo)
			{
				MatchState	= DoingWinA;
			}
			else
			{
				if(scoreTwo > scoreOne)
				{
					MatchState	= DoingWinB;
				}
				else
				{
					if(scoreOne == scoreTwo)
					{
						MatchState = DoingDraw;
					}
				}
			}

			CTeamMember* temp;

			for(int i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				temp->ReadyPlayer();
			}

			for(i = 0; i < 12; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				temp->ReadyPlayer();
			}

			IsAirhornPlaying = false;
		}
	}
	return true;

}
//*******************************************************************************
//**********************************************************************************
bool	CMatch::ProcessBall()
{
	//this method just updates the balls position and height;
	//if(TheBall->getState() != IN_CHUTE)
		TheBall->Process(TeamOneHalf, ControllerRef->IsButtonPressed(Select), ControllerRef->GetDirection());

	if(		(!TheBall->IsBallHeld())
		&&	(TheBall->getState() != IN_CHUTE))
	{
		BallCollision(TheBall);
	}
	else
	{
		if(TheBall->getState() == IN_CHUTE)
		{
			if(		(TheBall->AccelCount < 27)
				&&	(TheBall->AccelCount > 25 ) )
			{
				PLAY_BELL;
			}
		}
	}
	//if the ball isn't being held, update it's movement
	return true;
}
//**********************************************************************************
//******************************************************************************
bool	CMatch::ProcessTokens()
{
	//return true;
	//stuff like coins and armour.. put more on the map.. 
	int rand;

	//firstly check through all the tokens and remove any inactive ones.
	for(int i = 0; i < MAX_TOKENS; i++)
	{
		if(TokenArray[i] == NULL)
			continue;

		if(TokenArray[i]->getState() == GONE)
		{
			delete TokenArray[i];
			TokenArray[i] = NULL;
		}
		else
		{
			TokenArray[i]->Process();
		}
	}

	//Now activate any new ones
	//do we want to add one this turn... there should be a very low chance so about 2/255 should do

		rand = (GetRandomNumber());
	//int randTwo = GetRandomNumber();

	if(rand <= 4)
	{
		//find a free array location
		for(i = 0; i < MAX_TOKENS ; i++)
		{
			if(TokenArray[i] == NULL)
			{
				int type = 0;
				for(int p = 0; p < MAX_TOKENS; p++)
				{
					if(TokenArray[p] != NULL)
					{
						if(TokenArray[p]->getType() == 20)	
						{
							rand = GetRandomNumber();
							type = (( (rand * 40) / 255));
							break;
						}
						else
						{
							type = 20;
						}
					}
				}

				if(type > 20)
					type = 20;

				if(type == 1)
					continue;
				
				if(		(type >= 12)
					&&	(type != 20) )
				{
					for(int p = 0; p < MAX_TOKENS; p++)
					{
						if(TokenArray[p] != NULL)
						{
							if(		(TokenArray[p]->getType() >= 12)
								&&	(TokenArray[p]->getType() <= 19) )
							{
								return true;
							}
						}
					}
				}
				//and a random position for it to appear in
				rand = GetRandomNumber();
				int Ypos = 508 - ((rand * 1016) / 255);
				rand = GetRandomNumber();
				int XPos = 246 - ((rand * 492) / 255);
				TokenArray[i] = new CToken(type, XPos, Ypos);
				break;
			}
		}
	}

	return true;
}
//********************************************************************************
bool	CMatch::ProcessMapObjects()
{
	//processes all the bumpers and stuff
	LeftDome->Process();
	RightDome->Process();

	TopShroom->Process();
	BottomShroom->Process();

	LeftChute->Process(TheBall);
	RightChute->Process(TheBall);

	//still have to do stars

	TeamAMulti = LeftChute->GetTeamAMulti();
	TeamBMulti = RightChute->GetTeamBMulti();

	LeftBank->Process(GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1),
						TeamAMulti, TeamBMulti);
	RightBank->Process(GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1),
						TeamAMulti, TeamBMulti);

	return true;
}
//************************************************************************************
bool	CMatch::ProcessTeam(CTeam* theTeam, CTeam* oppTeam)
{
	ClosestPlayer = this->FindClosestPlayer(theTeam);
	CTeamMember* temp				= NULL;
	CTeamMember* ControlledPlayer	= NULL;
	
	int dist;
	
	temp = ClosestPlayer;
	if (temp->IsControlled)
	{
		ControlledPlayer = temp;
		//if the player is on screen
		//if the player is human
		if (GameDataRef->GetTeamByIndex(temp->getTeam())->ControlledBy)
		{
			dist = WorkOutDist((CActor*) temp, (CActor*) TheBall);
			if(dist < 250 )
			{
				ProcessControls(temp);
			}
			else
			{
				temp->SetDestination(TheBall->getXPos(), TheBall->getYPos());
			}
		}
		else
		{
			
			if(		(temp->getState() != THROWING)
				&&	(temp->getState() != FALLING)
				&&	(temp->getState() != FLOORED) )
				ProcessMainComputerPlayer(temp);
			
		}
		
		if(		(temp->getState() == FALLING)
			||	(temp->getState() == FLOORED)	)
		{
			if(temp->GetPossesion() == true)
			{
				TheBall->HoldOrLetGoOfBall();
			}
		}
		PlayerCollision(temp, theTeam, oppTeam);
		temp->Process();
		
	}
	
	
	//Now here is the complicated bit.
	/*
	what we do is we only want to affect players who are within 400 and in thier zone
	If they are not in their zone or within 400 then we want them to go to a default
	and wait
	
	  If they are to be affected where the controlled player is tells them where to go
	*/
	for(int i = 0; i < 9; i++)
	{
		temp = theTeam->GetTeamMember(i);
		if(temp->IsControlled)
			continue;
		
		dist = WorkOutDist((CActor*)temp, (CActor*)ControlledPlayer);
		if(dist > 800)
		{
			GoToDefaultPosition(temp);
			temp->Process();
			continue;
		}
		
		if(IsPlayerInZone(temp))
		{
			GoToDefaultPosition(temp);
			PlayerCollision(temp, theTeam, oppTeam);
			temp->Process();
			continue;
		}
		
		this->GoToPosition(temp, ControlledPlayer, theTeam, oppTeam);
		PlayerCollision(temp, theTeam, oppTeam);
		temp->Process();
	}
	
	return true;
}
//***********************************************************************************
//***********************************************************************************
bool	CMatch::GetEveryoneIntoStartingPositions()
{
	//this will put everyone into positions... 
	CTeamMember* temp;
	for(int i = 0; i < 9; i++)
	{
		if(GameHalf == FIRST_HALF)
		{
			temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
		}
		else
		{
			temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
		}
		if(!temp)
			continue;

		temp->setState(NONE, true);
		temp->SetDirection(N);
		temp->ReadyPlayer();
		int pos = temp->getPosition();
		

		switch(pos)
		{
		case CFWD:
			temp->SetXY(0, -48);
		
			break;
		case RWNG:
			temp->SetXY(100, -5);
			
			break;
		case LWNG:
			temp->SetXY(-250, -20);
		
			break;
		case CMID:
			temp->SetXY(0, -192);
		
			break;
		case RMID:
			temp->SetXY(150, -192);
			
			break;
		case LMID:
			temp->SetXY(-150, -192);
		
			break;
		case RDEF:
			temp->SetXY(150, -384);
			
			break;
		case LDEF:
			temp->SetXY(-150, -384);
			break;
		case GOAL:
			temp->SetXY(0, -540);
			break;
		}
	}		

	for(i = 0; i < 9; i++)
	{
		if(GameHalf == SECOND_HALF)
		{
			temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
		}
		else
		{
			temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
		}
		temp->setState(NONE, true);
		temp->SetDirection(S);
		int pos = temp->getPosition();
		switch(pos)
		{
		case CFWD:
			temp->SetXY(0, 48);
		
			break;
		case RWNG:
			temp->SetXY(-100, 15);
			
			break;
		case LWNG:
			temp->SetXY(250, 20);
		
			break;
		case CMID:
			temp->SetXY(0, 192);
		
			break;
		case RMID:
			temp->SetXY(-150, 192);
			
			break;
		case LMID:
			temp->SetXY(150, 192);
		
			break;
		case RDEF:
			temp->SetXY(-150, 384);
			
			break;
		case LDEF:
			temp->SetXY(150, 384);
			break;
		case GOAL:
			temp->SetXY(0, 540);
			break;
		}
	}

	return true;
	
}
//*************************************************************************************
bool	CMatch::CallTheMedics()
{
	//process medic stuff
	
	CTeamMember* injury	= NULL;
	CTeamMember* temp	= NULL;
	

	for(int i = 0; i < 9; i++)
	{
		temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
		
		if(temp->getCurrentStamina() < 0)
		{
			injury = temp;
		}
	}

	for(i = 0; i < 9; i++)
	{
		temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
		
		if(temp->getCurrentStamina() < 0)
		{
			injury = temp;
		}
	}

	if(InjuryCount == 0)
	{
		//put ball position to where the guy is lying.
		TheBall->HoldOrLetGoOfBall();
		TheBall->SetXY(injury->getXPos(), injury->getYPos());
		TheBall->SetVisible(false);
		TheBall->SetSpeed(0);

		//get your medics and send them to positions either side of him
		ThingA->Init(	injury->getXPos(),
						injury->getYPos(),
						true);
		ThingB->Init(	injury->getXPos(),
						injury->getYPos(),
						false);
	}
	else
	{
		if(InjuryCount > 20)
		{
			if(InjuryCount == 21)
			{
				ThingA->Init(	injury->getXPos(),
								injury->getYPos(),
								true);
				ThingB->Init(	injury->getXPos(),
								injury->getYPos(),
								false);
				TheBall->SetXY(injury->getXPos(), injury->getYPos());
				PLAY_CROWD2;
			}

			

			if(		(ThingA->There)
				&&	(ThingB->There) )
			{
				//TheBall->SetXY(injury->getXPos(), injury->getYPos());
				if(TheBall->getXPos() >= 0)
				{
					injury->SetXY((injury->getXPos() - 4), injury->getYPos());
				}
				else
				{
					injury->SetXY((injury->getXPos() + 4), injury->getYPos());
				}
			}
			
			ThingA->Process(ThingB);
			ThingB->Process(ThingA);
		}
		else
		{
			//PlayerCollision(injury, GameDataRef->GetTeamByIndex(injury->getTeam()), GameDataRef->GetTeamByIndex(injury->getTeam()));
		}

		if(InjuryCount == 115)
		{
			PLAY_ICECREAM;
		}

		for(int i = 0; i < 9; i++)
		{
			temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
			temp->Process();
			PlayerCollision(temp, GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1));
			temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
			temp->Process();
			PlayerCollision(temp, GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0));
		}
		//process tokens
		for(i = 0; i < MAX_TOKENS; i++)
		{
			if(TokenArray[i] == NULL)
				continue;

			if(TokenArray[i]->getState() == GONE)
			{
				delete TokenArray[i];
				TokenArray[i] = NULL;
			}
			else
			{
				TokenArray[i]->Process();
			}
		}

		PLAY_SIREN;
	}
	InjuryCount++;

	if(		(InjuryCount > 175)
		||	(  (ControllerRef->IsButtonPressed(Select)) && (InjuryCount > 30)  ) )
	{
	//chnge dead guys state so he is picked up
	//move all three towards the edge of the ma
		GameDataRef->GetTeamByIndex(injury->getTeam())->DoSubstituion(injury);
		GameDataRef->GetTeamByIndex(injury->getTeam())->Substitutions++;

		if(GameDataRef->GetTeamByIndex(0)->GetTeamNumber()	!= injury->getTeam() )
		{
			GameDataRef->GetTeamByIndex(0)->IncreaseScore(10 * TeamAMulti);
		}
		else
		{
			GameDataRef->GetTeamByIndex(1)->IncreaseScore(10 * TeamBMulti);
		}

		for(int i = 0; i < 9; i++)
		{
			temp = GameDataRef->GetTeamByIndex(0)->GetTeamMember(i);
			temp->ReadyPlayer();

			temp = GameDataRef->GetTeamByIndex(1)->GetTeamMember(i);
			temp->ReadyPlayer();
		}
		MatchState = StartingMatch;
		InjuryCount = 0;
		ThingA->SetXY(5000, 5000);
		ThingB->SetXY(5000, 5000);
		ThingA->setState(FLOORED);
		ThingB->setState(FLOORED);
		ThingA->There = false;
		ThingB->There = false;
		TheBall->SetXY(0,0);
	}
	return true;
}
//************************************************************************************
CTeamMember*	CMatch::FindClosestPlayer(CTeam* TheTeam)
{
	//finds the closest player to the ball

	CTeamMember* temp; //pointer to the temporarily held player data
	CTeamMember* closest = NULL;
	int closedist= 15000;
	int tempdist;

	if(TheTeam->GetPossesion() == true)
	{
		for (int i = 0; i < 9 ; i++)
		{
			temp = TheTeam->GetTeamMember(i);
			
			if(temp->GetPossesion() == true)
			{
				closest = temp;
			}

			temp->IsControlled = false;
			temp->IsClosest = false;
		}

		ClosestPlayer = closest;
		closest->IsControlled = true;
		closest->IsClosest = true;

		return closest;
	}
	else
	{
		for(int x = 0; x < 9; x++)
		{
			temp = TheTeam->GetTeamMember(x);
			tempdist = WorkOutDist((CActor*)TheBall, (CActor*)temp);
			if (tempdist < closedist)
			{
				closest = temp;
				closedist = tempdist;
			}
			//basically reset every player to not controlled then as we leave the loop 
			//set closest
			temp->IsControlled = false;
			temp->IsClosest = false;
			
		}
		
		closest->IsControlled = true;
		closest->IsClosest = true;
		ClosestPlayer = closest;
	}
	
	return closest;
}
//*******************************************************************************************
bool	CMatch::LaunchBall()
{
	//does the ball launch routine at kick off
	TheBall->LaunchIt();
	return true;
}
//******************************************************************************************
bool	CMatch::ProcessGoal(CTeam* TheTeam, bool HomeGoal, CTeam* oppTeam)
{
	//does all the celebrations and stuff
	
	CTeamMember* temp;
	int Rand = 0;

	bool Esc = true;

	if(ReplayTime > 0)
	{
		if(PlayReplay == true)
		{
			PLAY_REPLAY;
			PlayReplay = false;
		}


		if(!(ControllerRef->IsButtonPressed(Select)))
			return false;
		else
			return true;
	}

	if(HasSetActions == false)
	{
		if(HomeGoal)
		{
			TheBall->HoldOrLetGoOfBall(TheTeam->GetTeamMember(CFWD));

			for(int i = 0; i < 9; i++)
			{
				
				TheBall->SetVisible(false);
				temp = oppTeam->GetTeamMember(i);
				RunToStartingPosition(temp);

				temp = TheTeam->GetTeamMember(i);
				if(temp->getPosition() != CFWD)
				{
					Rand = GetRandomNumber();

					if((		Rand > 170)
						&&	(temp->getPosition() != LWNG)
						&&	(temp->getPosition() != CFWD))
					{
						RunToStartingPosition(temp);
						temp->Dance();
					}
					else
					{
							RunToStartingPosition(temp);
					}
				}
				else
				{
					if(temp->DirectionOfPlay == UP_MAP)
						temp->KneeSlide();
					else
						temp->KneeSlide();
				}
			}
		}
		else
		{
			TheBall->HoldOrLetGoOfBall(TheTeam->GetTeamMember(CFWD));

			for(int i = 0; i < 9; i++)
			{
				
				TheBall->SetVisible(false);
				temp = oppTeam->GetTeamMember(i);
				RunToStartingPosition(temp);

				temp = TheTeam->GetTeamMember(i);
				if(temp->GetPossesion() == false)
				{
					Rand = GetRandomNumber();

					if(		(Rand > 170)
						&&	(temp->getPosition() != RWNG)
						&&	(temp->getPosition() != CFWD))
					{
						RunToStartingPosition(temp);
						temp->Dance();
					}
					else
					{
							RunToStartingPosition(temp);
					}
				}
				else
				{
					if(temp->DirectionOfPlay == UP_MAP)
						temp->KneeSlide();
					else
						temp->KneeSlide();
				}
			}
		}
		HasSetActions = true;
		Esc = false;
		PLAY_CROWD2;
	}
	else
	{
		for(int i = 0; i < 9; i++)
		{
			temp = TheTeam->GetTeamMember(i);
			temp->Process();

			if(		(temp->GetDirection() != -1)
				&&	(temp->getXPos()	  != 0)	
				&&	(temp->getPosition() == CFWD) )
					Esc = false;
		
			temp = oppTeam->GetTeamMember(i);
			temp->Process();

			if(		(temp->GetDirection() != -1)
				&&	(temp->getXPos()	  != 0)	
				&&	(temp->getPosition() == CFWD) )
					Esc = false;

			
		}
		TheBall->Process(TeamOneHalf, ControllerRef->IsButtonPressed(Select), ControllerRef->GetDirection());
		for(i = 0; i < MAX_TOKENS; i++)
		{
			if(TokenArray[i] == NULL)
				continue;

			if(TokenArray[i]->getState() == GONE)
			{
				delete TokenArray[i];
				TokenArray[i] = NULL;
			}
			else
			{
				TokenArray[i]->Process();
			}
		}
	}

	if(		(GetRandomNumber() > 220)
		&&	(PlayIcecream == true) )
	{
		PLAY_ICECREAM;
		PlayIcecream = false;
	}

	if(		(Esc == true)
		||	(ControllerRef->IsButtonPressed(Select)))
	{
		for(int i = 0; i < 9; i++)
		{
			temp = TheTeam->GetTeamMember(i);
			temp->ReadyPlayer();

			temp = oppTeam->GetTeamMember(i);
			temp->ReadyPlayer();

		}
		PlayReplay = true;
		PlayIcecream = true;
		HasSetActions = false;
		TheBall->SetXY(0,0);
		LeftChute->setState(NONE);
		RightChute->setState(NONE);
		return true;
	}
	return false;
}
//*********************************************************************************************
bool	CMatch::ProcessMatchOver()
{
	//lines up all the players at the end of a match
	GameDataRef->GetActiveTeamByIndex(0)->TimeInOppenentsHalf = TheBall->TimeInTeamTwoHalf / 20;
	GameDataRef->GetActiveTeamByIndex(1)->TimeInOppenentsHalf = TheBall->TimeInTeamOneHalf / 20;
	
	CTeamMember* temp;

	for(int i = 0; i < 12; i++)
	{
		temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
		temp->ReadyPlayer();
	}

	for(i = 0; i < 12; i++)
	{
		temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
		temp->ReadyPlayer();
	}

	
	MatchState = MatchOver;

	return true;
}
//******************************************************************************************
bool	CMatch::ProcessControls(CTeamMember* person)
{
	// Stylus hack, erm I mean hook.
	ConvertMouseCoOrdsToCursorMovement();

	int Direction = ControllerRef->GetDirection();
	int x = person->getXPos();
	int y = person->getYPos();

	


	ControllerRef->IsButtonPressed(Select);

	if(person->GetPossesion() == false)
	{
		if(ControllerRef->IsButtonPressed(Select))
		{
			if(		(TheBall->GetHeight() == 2)
				&&	(TheBall->IsBallHeld() == false) )
			{
				int dist = WorkOutDist((CActor*)person, (CActor*)TheBall);
				if(dist < 50)
				{
					person->Jump(Direction);
				}
			}
			else
			{
				if(person->getPosition() != GOAL)
				{
					if(Direction != STILL)
					{
						person->Slide(Direction);
					}
					else
					{
						person->Punch(Direction);
						if(		(person->getState() == NONE))
							PLAY_PUNCH;
					}
				}
				else
				{
					if(		(Direction != STILL)
						&&	(Direction != N)
						&&	(Direction != S)	)
					{
						person->Dive(Direction);
					}
					else
					{
						person->Slide(Direction);
					}
				}
			}
		}
	}
	else
	{
		if(ControllerRef->IsButtonPressedDB(Select))
		{
			TheBall->PrepareThrow(Direction, person->getThr(), false, true, false);
			PLAY_THROW;
			//OutputDebugString(TEXT("trying to throw ball \n"));
			if(person->DirectionOfPlay == UP_MAP)
			{
				if(		(Direction == N)
					||	(Direction == NW)
					||	(Direction == NE) )
				{
					if(person->getYPos() > 300)
					{
						GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					}
				}
			}
			else
			{
				if(		(Direction == S)
					||	(Direction == SW)
					||	(Direction == SE) )
				{
					if(person->getYPos() < -300)
					{
						GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					}
				}			
			}
		}
	}
	
	if(		(person->getState() != FALLING)
		||	(person->getState() != FLOORED) )
	{
	switch(Direction)
		{
		case N:
			person->SetDestination(x, y + 30);
			break;
		case NE:
			person->SetDestination(x + 30, y + 30);
			break;
		case E:
			person->SetDestination(x + 30, y);
			break;
		case SE:
			person->SetDestination(x + 30, y - 30);
			break;
		case S:
			person->SetDestination(x, y - 30);
			break;
		case SW:
			person->SetDestination(x - 30, y - 30);
			break;
		case W:
			person->SetDestination(x - 30, y);
			break;
		case NW:
			person->SetDestination(x - 30, y + 30);
			break;
		default:
			person->SetDestination(x, y);
		}
	}
	return true;
}
//******************************************************************************************
//process what the contolled player for the computers team will do....
bool	CMatch::ProcessMainComputerPlayer(CTeamMember* person)
{
	if(person->getState() == THROWING)
		return true;
	
	//If the computer team is epecially dense they may not react to thier surroundings
	//as fast as more intelligent teams
	//so if they fail an intelligence check (which will not happen too often)
	//they will not act this turn
	
	if(GameDataRef->GetActiveTeamByIndex(1)->GetTeamNumber() == person->getTeam())
	{

		FirstAICount += 1;

		if(person->getInt() > 150)
			FirstAICount += 1;

		if(person->getInt() > 200)
			FirstAICount += 1;

		if(FirstAICount < 3)
		{
			return true;
		}
		else
		{
			FirstAICount = 0;
		}
	}
	else
	{
		SecondAICount += 1;

		if(person->getInt() > 150)
			SecondAICount += 1;

		if(person->getInt() > 200)
			SecondAICount += 1;

		if(SecondAICount < 3)
		{
			return true;
		}
		else
		{
			SecondAICount = 0;
		}	
	}
	
	//Thus idiot teams process thier moves about twice in a second
	//		dim	 teams process thier moves about 3 times a second	
	//		smart teams process thier moves about 4 times a second.
	if(person->GetPossesion())
	{

		if(person->getPosition() == GOAL)
		{
			//just pass it. don't try to run or waste time thinking of your girlfreind
			if(DoPass(person))
				return true;

			if(DoRun(person))
				return true;
		}

		if (GameHalf == FIRST_HALF)
		{
			if (person->DirectionOfPlay == DOWN_MAP)
			{
				if(DoShot(person, GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0)))
				{
					GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					return true;
				}
			}
			else
			{
				if(DoShot(person, GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1)))
				{
					GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					return true;
				}
			}
		}
		else
		{
			if (person->DirectionOfPlay == DOWN_MAP)
			{
				if(DoShot(person, GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1)))
				{
					GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					return true;
				}
			}
			else
			{
				if(DoShot(person, GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0)))
				{
					GameDataRef->GetTeamByIndex(person->getTeam())->ShotsAtGoal++;
					return true;
				}
			}
		}
		//or go for a run
		if(DoRun(person))
			return true;

		//pass to someone
		if(DoPass(person))
			return true;
	}
	else
	{
		if(person->getPosition() == GOAL)
		{
			int dir = WorkOutDirection(person->getXPos(), person->getYPos(), TheBall->getXPos(), TheBall->getYPos());
			int dist = WorkOutDist((CActor*)person, (CActor*)TheBall);

			if(dist < (300 - person->getInt()))
			{
				DoRun(person);
				return true;
			}
			else
			{
				if(		(dist < (GetRandomNumber() / 2))
					&&	(dist > 25))
				{
					if(		(dir == NW)
						||	(dir == W)
						||	(dir == SW))
					{
						person->Dive(dir);
						return true;
					}

					if(		(dir == NE)
						||	(dir == E)
						||	(dir == SE))
					{
						person->Dive(dir);
						return true;
					}
				}
				else
				{
					if (GameHalf == FIRST_HALF)
					{
						if (person->DirectionOfPlay == DOWN_MAP)
						{
							if(GoToPosition(person,person, GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0)))
								return true;
						}
						else
						{
							if(GoToPosition(person,person, GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1)))
								return true;
						}
					}
					else
					{
						if (person->DirectionOfPlay == DOWN_MAP)
						{
							if(GoToPosition(person,person, GameDataRef->GetActiveTeamByIndex(0), GameDataRef->GetActiveTeamByIndex(1)))
								return true;
						}
						else
						{
							if(GoToPosition(person,person, GameDataRef->GetActiveTeamByIndex(1), GameDataRef->GetActiveTeamByIndex(0)))
								return true;
						}
					}
				}
			}
		}

		//hmmm
		
		if(		(TheBall->getState() == HIGH)
			||	(TheBall->getState() == LOW) )
		{
			if(PredictCatch(TheBall, person))
				return true;
		}
		
		if(DoRun(person))
			return true;
	}
	return true;
}
//*********************************************************************************************
CTeamMember*	CMatch::FindClosestOpponent(CTeamMember* person, CTeam* theTeam)
{
	int closedist= 150000;
	int tempdist;
	CTeamMember* temp;
	CTeamMember* closest = 0;
	CTeam* TheTeam = theTeam;

	for(int x = 0; x < 9; x++)
	{
		temp = TheTeam->GetTeamMember(x);
		tempdist = WorkOutDist((CActor*)person, (CActor*)temp);
		if (tempdist < closedist)
		{
			closest = temp;
			closedist = tempdist;
		}
	}
	return closest;
}
//*************************************************************************************
int		CMatch::GetLeftDiagX(CTeamMember* person)
{
	int X = person->getXPos();
	int Y = person->getYpos();
	if(		(person->GetDirectionFacing() ==  N) 
		||	(person->GetDirectionFacing() == NW) 
		||	(person->GetDirectionFacing() == NE)
		||	(person->GetDirectionFacing() ==  E))
	{
		while(	((X > -266) && (X < 266))
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X--;
			Y++;
		}
	}
	else
	{
		while(	((X > -266) && (X < 266)) 
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X--;
			Y--;
		}
	}
	return X;
}
//**********************************************************************************
int		CMatch::GetLeftDiagY(CTeamMember* person)
{
	int X = person->getXPos();
	int Y = person->getYpos();
	if(		(person->GetDirectionFacing() ==  N) 
		||	(person->GetDirectionFacing() == NW) 
		||	(person->GetDirectionFacing() == NE)
		||	(person->GetDirectionFacing() ==  E))
	{
		while(	((X > -266) && (X < 266))
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)))
		{
			X--;
			Y++;
		}
	}
	else
	{
		while(	((X > -266) && (X < 266))
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X--;
			Y--;
		}
	}
	return Y;
}
//*********************************************************************************
int		CMatch::GetRightDiagX(CTeamMember* person)
{
	int X = person->getXPos();
	int Y = person->getYpos();
	if(		(person->GetDirectionFacing() ==  N) 
		||	(person->GetDirectionFacing() == NW) 
		||	(person->GetDirectionFacing() == NE)
		||	(person->GetDirectionFacing() ==  E))
	{
		while(	((X > -266) && (X < 266))
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X++;
			Y++;
		}
	}
	else
	{
		while(	((X > -266) && (X < 266))
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X++;
			Y--;
		}
	}
	return X;
}
//**********************************************************************************
int		CMatch::GetRightDiagY(CTeamMember* person)
{
	int X = person->getXPos();
	int Y = person->getYpos();
	if(		(person->GetDirectionFacing() ==  N) 
		||	(person->GetDirectionFacing() == NW) 
		||	(person->GetDirectionFacing() == NE)
		||	(person->GetDirectionFacing() ==  E))
	{
		while(	((X > -266) && (X < 266)) 
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X++;
			Y++;
		}
	}
	else
	{
		while(	((X > -266) && (X < 266)) 
			&&	((Y > PITCH_BLY) && (Y < PITCH_TRY)) )
		{
			X++;
			Y--;
		}
	}
	return Y;
}
//*******************************************************************************************
bool	CMatch::PredictCatch(CBall* theBall, CTeamMember* thePerson)
{
	int randomFactor = this->GetRandomNumber();
	randomFactor -= 125;
	int ballDestX = theBall->DestX;
	int ballDestY = theBall->DestY;
	int ballXPos  = theBall->getXPos();
	int ballYPos  = theBall->getYpos();
	int playerXPos = thePerson->getXPos();
	int playerYPos = thePerson->getYpos();

	int halfwayX  = (ballDestX - (ballDestX - ballXPos))/2;
	int halfwayY  = (ballDestY - (ballDestY - ballYPos))/2;

	//I think the best way to do this without having lots of calculation overhead is
	//to work out the point halfway along the balls trajectory and make him go there
	//if he has the time, otherwise to go where the ball is due to land.
	thePerson->CalcMoveSpeed();
	int playerMoveSpeed = thePerson->GetMoveSpeed();
	if(playerMoveSpeed == 0)
		return false;

	int ballMoveSpeed	= theBall->GetSpeed();
	if(ballMoveSpeed == 0)
		return false;

	int ballCountY = 0;
	int ballCountX = 0;
	int playerCountY = 0;
	int playerCountX = 0;

	if(	ballXPos < halfwayX)
	{
		while(ballXPos < halfwayX)
		{
			ballXPos += ballMoveSpeed;
			ballCountX++;
		}
	}
	else
	{
		while(ballXPos > halfwayX)
		{
			ballXPos -= ballMoveSpeed;
			ballCountX++;
		}
		
	}
		
	if(ballYPos < halfwayY)
	{
		while(ballYPos < halfwayY)
		{
			ballYPos += ballMoveSpeed;
			ballCountY++;
		}
	}
	else
	{
		while(ballYPos > halfwayY)
		{
			ballYPos -= ballMoveSpeed;
			ballCountY++;
		}	
	}

	if(playerXPos < halfwayX)
	{
		while(playerXPos < halfwayX)
		{
			playerXPos += playerMoveSpeed;
			playerCountX++;		
		}
	}
	else
	{
		while(playerXPos > halfwayX)
		{
			playerXPos -= playerMoveSpeed;
			playerCountX++;		
		}
		
	}

	if(playerYPos < halfwayY)
	{
		while(playerYPos < halfwayY)
		{
			playerYPos += playerMoveSpeed;
			playerCountY++;		
		}
	}
	else
	{
		while(playerYPos > halfwayY)
		{
			playerYPos -= playerMoveSpeed;
			playerCountY++;		
		}
		
	}
	if(GetRandomNumber() < 75)
	{
		if( ((playerCountX + playerCountY) < (ballCountY + ballCountX) ) && (thePerson->getSpd() > TheBall->GetSpeed()))
		{
			thePerson->SetDestination((halfwayX + randomFactor), (halfwayY + randomFactor));
			return true;
		}
		else
		{
			thePerson->SetDestination((ballDestX + randomFactor), (ballDestY + randomFactor));
			return true;
		}
	}

	return false;
}
//******************************************************************************************
int		CMatch::GetRandomNumber()
{
	//random factor like a tractor.. weave your magic spell
	int rand = RandomArray[RandomArrayLoc];
	RandomArrayLoc++;
	if(RandomArrayLoc > 300)
	{
		ResetRandomNumbers();
		RandomArrayLoc = 0;
	}
	return rand;
}
//-*****************************************************************************************
//defines for the sectors you can be in
#define		MAX_LEFT		-150
#define		MAX_RIGHT		 150

#define		TOP_THIRD		 360
#define		BOTTOM_THIRD	-360
//If you are not controlled you should go to a position on the map.
//this puts you into it.
bool	CMatch::GoToPosition(CTeamMember* thePlayer, CTeamMember* ControlledPlayer, CTeam* theTeam, CTeam* oppTeam)
{
	int PlayerPosition = thePlayer->getPosition();
	int ControlledPlayerZone = this->WhichZoneIsPlayerIn(ControlledPlayer);
	int directionChange;
	int xDest;
	int yDest;
	int xMax;
	int	xMin;
	int yMax;
	int yMin;
	
	//check reactions
	if((thePlayer->getInt() + GetRandomNumber()) < GetRandomNumber())
	{
		return true;
	}

	if(thePlayer->DirectionOfPlay == UP_MAP)
	{
		directionChange = 1;
	}
	else
	{
		directionChange = -1;
	}

	if(PlayerPosition == ControlledPlayerZone)
	{
		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if(PlayerPosition > ControlledPlayer->getPosition())
			{
				yDest = ControlledPlayer->getYPos() - (100 + (GetRandomNumber() / 5));
				xDest = ControlledPlayer->getXPos();
			}
			else
			{
				yDest = ControlledPlayer->getYPos() + (100 + (GetRandomNumber() / 5));
				xDest = ControlledPlayer->getXPos();
			}
		}
		else
		{
			if(PlayerPosition > ControlledPlayer->getPosition())
			{
				yDest = ControlledPlayer->getYPos() + (100 + (GetRandomNumber() / 5));
				xDest = ControlledPlayer->getXPos();
			}
			else
			{
				yDest = ControlledPlayer->getYPos() - (100 + (GetRandomNumber() / 5));
				xDest = ControlledPlayer->getXPos();
			}
		}

		/*******
		 * This code put in for a small bug fix
		 *******/
		if(yDest >= PITCH_TLY)
		{
			yDest = ControlledPlayer->getYPos() - (100 + (GetRandomNumber() / 5));
		}
		
		if(yDest <= PITCH_BLY)
		{
			yDest = ControlledPlayer->getYPos() + (100 + (GetRandomNumber() / 5));
		}

		//****************************************

		thePlayer->SetDestination(xDest, yDest);
		return true;;
	}
	//******************************************************
	//		TEST CODE COMIN UP!!!!!!!!!!!!!!!!!!
	//******************************************************
	/*
		This next chunk of code is put in for a reason.
		I want intelligence to factor more into what your players do
		so that they may or may not be in position when you pass to them
		also I want them to try and attack people close to them if thier aggresion is
		very high

		The calclation I am going to use for these is 
		intellignce (or aggresion) + (a random number for 0 - 255) <= (rand 0 - 255)
		
		This means that lower aggression or intelligence teams will fuck up
		more often
	*/

	//code starts

	//this turn they may not be on the ball enough to realise what is happening
	if( (thePlayer->getInt() + (GetRandomNumber() / 2) ) <= GetRandomNumber() )
		return true;

	//this turn they may decide to go clobber somebody
	CTeamMember* closestOpp = FindClosestOpponent(thePlayer, oppTeam);
	int closestOppDist = WorkOutDist(thePlayer, closestOpp);

	if(closestOppDist < 75)
	{
		if( (thePlayer->getAgr() - GetRandomNumber()) >= GetRandomNumber())
		{
			thePlayer->SetDestination(closestOpp->getXPos(), closestOpp->getYPos());
			return true;
		}
	}
	//code ends

	switch(PlayerPosition)
	{
	case GOAL:
		xMax = 70;
		yMax = -510;
		xMin = -70;
		yMin = PITCH_BLY;

		if(		(TheBall->getXPos() < 65)
			&&	(TheBall->getXPos() > -65) )
		{
			xDest = TheBall->getXPos();
		}
		else
		{
			if(thePlayer->DirectionOfPlay == UP_MAP)
			{
				xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), SW);
				if(		(xDest < 65)
				&&		(xDest > -65) )
				{
				}
				else
				{
					xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), SE);
					if(		(xDest < 65)
					&&		(xDest > -65) )
					{
					}
					else
					{
						xDest = 0;
					}
				}
			}
			else
			{
				xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), NW);
				if(		(xDest < 65)
				&&		(xDest > -65) )
				{
				}
				else
				{
					xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), NE);
				}
			}
		}
		if(thePlayer->DirectionOfPlay == UP_MAP)
			yDest = PITCH_BLY + 5;
		else
			yDest = PITCH_TLY - 5;

		break;

	case LDEF:
		xMax = 0;
		yMax = -262;
		xMin = -300;
		yMin = PITCH_BLY;

		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if( ( (theTeam->GetTeamMember(GOAL))->getXPos() )  < 0)
			{
				xDest = -100;
				yDest = -450;
			}
			else
			{
				xDest = (theTeam->GetTeamMember(GOAL))->getXPos() - 75;
				yDest = (PITCH_BLY + 150) + (theTeam->GetTeamMember(GOAL))->getXPos(); 
			}
		}
		
		else
		{
			if( ( (theTeam->GetTeamMember(GOAL))->getXPos() )  > 0)
			{
				xDest = 100;
				yDest = 450;
			}
			else
			{
				xDest = (theTeam->GetTeamMember(GOAL))->getXPos() + 75;
				yDest = (PITCH_TLY - 150) - (theTeam->GetTeamMember(GOAL))->getXPos() ; 
			}	
		}
		
		
		break;

	case RDEF:
		xMax = 300;
		yMax = -262;
		xMin = 0;
		yMin = PITCH_BLY;

		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if( ( (theTeam->GetTeamMember(GOAL))->getXPos() )  > 0)
			{
				xDest = 100;
				yDest = -450;
			}
			else
			{
				xDest = (theTeam->GetTeamMember(GOAL))->getXPos() + 75;
				yDest = (PITCH_BLY + 150) - (theTeam->GetTeamMember(GOAL))->getXPos(); 
			}
		}
		
		else
		{
			if( ( (theTeam->GetTeamMember(GOAL))->getXPos() )  < 0)
			{
				xDest = -100;
				yDest = 450;
			}
			else
			{
				xDest = (theTeam->GetTeamMember(GOAL))->getXPos() - 75;
				yDest = (PITCH_TLY - 150) + (theTeam->GetTeamMember(GOAL))->getXPos() ; 
			}	
		}
		
		break;

	case CMID:
		xMax = 100;
		yMax = 362;
		xMin = -100;
		yMin = -262;

		xDest = (theTeam->GetTeamMember(CFWD))->getXPos();
		yDest = (theTeam->GetTeamMember(CFWD))->getYPos() - 200;

		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if(yDest > yMax)
			{
				yDest = (theTeam->GetTeamMember(CFWD))->getYPos() - 300;
			}
			if(TheBall->getYPos() < -150)
			{
				yDest = TheBall->getYPos();
			}

			if(yDest < yMin)
				yDest = yMin;
		}
		else
		{
			if(yDest < -262)
			{
				yDest = (theTeam->GetTeamMember(CFWD))->getYPos() - 300;
			}

			if(TheBall->getYPos() > 150)
			{
				yDest = TheBall->getYPos();
			}

			if(yDest > 362)
				yDest = 362;
		}
		break;

	case LMID:
		xMax = -100;
		yMax = 362;
		xMin = -300;
		yMin = -262;

		xDest = ((theTeam->GetTeamMember(CMID))->getXPos()) - 100;

		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if((theTeam->GetTeamMember(CMID))->getYPos() > 0)
			{
				yDest = (theTeam->GetTeamMember(CMID))->getYPos();

			}
			else
			{
				yDest = (theTeam->GetTeamMember(CMID))->getYPos() + 100;
			}
		}
		else
		{
			if((theTeam->GetTeamMember(CMID))->getYPos() < 0)
			{
				yDest = -(theTeam->GetTeamMember(CMID))->getYPos();

			}
			else
			{
				yDest = -(theTeam->GetTeamMember(CMID))->getYPos() + 100;
			}
		}
		break;

	case RMID:
		xMax = 300;
		yMax = 362;
		xMin = 100;
		yMin = -262;

		xDest = ((theTeam->GetTeamMember(CMID))->getXPos()) + 100;
		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if((theTeam->GetTeamMember(CMID))->getYPos() > 0)
			{
				yDest = (theTeam->GetTeamMember(CMID))->getYPos();

			}
			else
			{
				yDest = (theTeam->GetTeamMember(CMID))->getYPos() + 100;
			}
		}
		else
		{
			if((theTeam->GetTeamMember(CMID))->getYPos() < 0)
			{
				yDest = -(theTeam->GetTeamMember(CMID))->getYPos();

			}
			else
			{
				yDest = -(theTeam->GetTeamMember(CMID))->getYPos() + 100;
			}
		}
		break;

	case CFWD:
		xMax = 100;
		yMax = PITCH_TRY;
		xMin = -100;
		yMin = 262;

		/*
		xDest = (oppTeam->GetTeamMember(GOAL))->getXPos();
		
		yDest = (TheBall->getYPos());

		if(yDest > yMax - 50)
		{
			yDest = yMax -50;
		}
		else
		{
			if(thePlayer->DirectionOfPlay != UP_MAP)
			{
				yDest = (yDest * -1);
			}
		}
		*/
		xDest = (theTeam->GetTeamMember(CMID))->getXPos();
		//yDest = (theTeam->GetTeamMember(CMID))->getYPos() + 75;
		yDest = (TheBall->getYPos());


		//if(thePlayer->DirectionOfPlay == UP_MAP)
		//{
			if(yDest > yMax)
			{
				yDest = 450;
			}

			while(yDest < yMin)
			{
				yDest += 100;
			}
		//}
		//else
		/*{

			while(yDest > -yMin)
			{
				yDest -= 100;
			}

			if(yDest < -yMax)
			{
				yDest = -450;
			}
		}
		*/
		break;

	case RWNG:
		xMax = 300;
		yMax = PITCH_TRY;
		xMin = 100;
		yMin = 262;

		xDest = ((theTeam->GetTeamMember(CFWD))->getXPos()) + 100 ;
		if((theTeam->GetTeamMember(CFWD))->getYPos() < 400)
		{
			yDest = ((theTeam->GetTeamMember(CFWD))->getYPos()) + 100;
			
			if(thePlayer->DirectionOfPlay == DOWN_MAP)
				yMax = -yMax;
			if(yDest > yMax)
			{
				yDest = ((theTeam->GetTeamMember(CFWD))->getYPos());
			}
		}
		else
		{
			yDest = ((theTeam->GetTeamMember(CFWD))->getYPos());
		}

		if(thePlayer->DirectionOfPlay == DOWN_MAP)
			yMax = -yMax;
		break;

	case LWNG:
		xMax = -100;
		yMax = PITCH_TRY;
		xMin = -300;
		yMin = 262;

		xDest = ((theTeam->GetTeamMember(CFWD))->getXPos()) - 100;

		if((theTeam->GetTeamMember(CFWD))->getYPos() < 400)
		{
			yDest = ((theTeam->GetTeamMember(CFWD))->getYPos());

			if(thePlayer->DirectionOfPlay == DOWN_MAP)
				yMax = -yMax;
			if(yDest > yMax)
			{
				yDest = ((theTeam->GetTeamMember(CFWD))->getYPos()) - 100;
			}
		}
		else
		{
			yDest = ((theTeam->GetTeamMember(CFWD))->getYPos());
		}

		if(thePlayer->DirectionOfPlay == DOWN_MAP)
			yMax = -yMax;
		break;

	default:
		return false;
		break;
	
	}

	if(		(thePlayer->getPosition() == GOAL)
		||  (thePlayer->getPosition() == LDEF)
		||	(thePlayer->getPosition() == RDEF))
	{

		if(directionChange == -1)
		{	
			int temp;
			temp = xMax;
			xMax = xMin;
			xMax = xMax * -1;
			xMin = temp;
			xMin = xMin * -1;
			
			temp = yMax;
			yMax = yMin;
			yMax = yMax * -1;
			yMin = temp;
			yMin = yMin * -1;
		}
	}

	if(xDest > xMax)
	{
		xDest = xMax;
	}
	if(xDest < xMin)
	{
		xDest = xMin;
	}
	if(yDest > yMax)
	{
		yDest = yMax;
	}
	if(yDest < yMin)
	{
		yDest = yMin;
	}

	//now just a little bit of code to make sure two players don't try to go to the same place
	CTeamMember* temp = FindClosestOpponent(thePlayer, oppTeam);

	int dist = WorkOutDist(thePlayer, temp);

	if(dist < 20)
	{
		xDest -= 20;
		yDest -= 20;
	}

	//goalies are a special case.
	//beacuse of all the calculations that go into determining their positions
	//all the work for both teams is done inside the switch statement
	if(		(thePlayer->getPosition() != GOAL)
		&&  (thePlayer->getPosition() != LDEF)
		&&	(thePlayer->getPosition() != RDEF))
	{
		thePlayer->SetDestination((xDest * directionChange) ,(yDest * directionChange));
		//thePlayer->SetDestination((xDest) ,(yDest));
	}
	else
	{
		thePlayer->SetDestination(xDest ,yDest);
	}
	
	return true;
}
//****************************************************************************************
/* Computer control algorithms. These define what happen to the controlled player on
		the computers side;
*/
//****************************************************************************************
//****************************************************************************************
//Other supporting methods for AI
//****************************************************************************************
bool	CMatch::DoShot(CTeamMember* thePlayer, CTeam* theTeam, CTeam* oppTeam)
{
	
	if(!thePlayer && !thePlayer->GetPossesion())
		return false;

	int distance;
	int throwDistance = thePlayer->getThr() * 3; //rough approximation of the orginal
	//shot angles
	bool forward = false;
	bool left = false;
	bool right = false;
	int direction;
	int ThrowTargetY;

	if(thePlayer->DirectionOfPlay == UP_MAP)
	{
		thePlayer->SetDirection(N);
		direction = N;
		distance = this->WorkOutDist((CActor*)thePlayer, NULL, 0, PITCH_TRY);
	}
	else
	{
		thePlayer->SetDirection(S);
		direction = S;
		distance = this->WorkOutDist((CActor*)thePlayer, NULL, 0, PITCH_BLY);	
	}

	if(		(distance < throwDistance)
		&&	(distance < 250)		)
	{
		if((this->GetLeftDiagX(thePlayer) < GOAL_X_END) && (this->GetLeftDiagX(thePlayer) > GOAL_X_START))
		{
			left = true;
		}

		if((this->GetRightDiagX(thePlayer) < GOAL_X_END) && (this->GetRightDiagX(thePlayer) > GOAL_X_START))
		{
			right = true;
		}

		if((thePlayer->getXPos() < GOAL_X_END) && (thePlayer->getXPos() > GOAL_X_START))
		{
			forward = true;
		}

		if(		(left == true)
			&&	(thePlayer->getInt() > GetRandomNumber()))
		{
			if(thePlayer->DirectionOfPlay == UP_MAP)
				direction = NW;
			else
				direction = SW;
			

			int endX = (this->GetEndXOfLine(thePlayer->getXPos(), thePlayer->getYpos(), direction));

			if((GOAL_X_START < endX) && (endX < GOAL_X_END) )
			{
				if(		(direction == N) 
					||	(direction == NW) 
					||	(direction == NE))
				{
					ThrowTargetY = PITCH_TRY;
				}
				else
				{
					ThrowTargetY = PITCH_BLY;
				}

				if(this->CheckLOS( thePlayer->getXPos(), 
									thePlayer->getYpos(), 
									endX, 
									ThrowTargetY, 
									thePlayer,
									distance,
									oppTeam))
				{
					if(thePlayer->getInt() < (this->GetRandomNumber() + 50))
					{
						//return false;
					}
				}

				if(distance < 350)
				{
					
					TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											  false,
											  false,
											  false);
					PLAY_THROW;
				}
				else
				{
					
					TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											  true,
											  false,
											  false);
					PLAY_THROW;
				}

				thePlayer->SetDirection(direction);
				return true;
			}
			
		}

		if(		(right == true)
			&&	(thePlayer->getInt() > GetRandomNumber()))
		{
			if(thePlayer->DirectionOfPlay == UP_MAP)
				direction = NE;
			else
				direction = SE;

			int endX = (this->GetEndXOfLine(thePlayer->getXPos(), thePlayer->getYpos(), direction));

			if((GOAL_X_START < endX) && (endX < GOAL_X_END) )
			{
				
				if(		(direction == N) 
					||	(direction == NW) 
					||	(direction == NE))
				{
					ThrowTargetY = PITCH_TRY;
				}
				else
				{
					ThrowTargetY = PITCH_BLY;
				}
					
				if(this->CheckLOS( thePlayer->getXPos(), 
								thePlayer->getYpos(), 
								endX, 
								ThrowTargetY, 
								thePlayer,
								distance,
								oppTeam))
				{
					if(thePlayer->getInt() < (this->GetRandomNumber() + 50))
					{
						//return false;
					}
				}


				if(distance < 350)
				{
					
					TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											  false,
											  false,
											  false);
					PLAY_THROW;
				}
				else
				{
					
						TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											  true,
											  false,
											  false);
						PLAY_THROW;
				}

				thePlayer->SetDirection(direction);
				return true;
			}
		}

		if(		(forward == true)
			&&	(thePlayer->getInt() > GetRandomNumber()))
		{
			

			if((GOAL_X_START < thePlayer->getXPos()) && (thePlayer->getXPos() < GOAL_X_END) )
			{
				if(		(direction == N) 
					||	(direction == NW) 
					||	(direction == NE))
				{
					ThrowTargetY = PITCH_TRY;
				}
				else
				{
					ThrowTargetY = PITCH_BLY;
				}

				if(this->CheckLOS( thePlayer->getXPos(), 
								thePlayer->getYpos(), 
								thePlayer->getXPos(), 
								ThrowTargetY, 
								thePlayer,
								distance,
								oppTeam))
				{
					if(thePlayer->getInt() > (this->GetRandomNumber() + 75))
					{
						TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											false,
											false,
											false);
						PLAY_THROW;
						if(GetRandomNumber() > 175)
						{
							TheBall->SetAfterTouch(W);
						}
						else
						{
							TheBall->SetAfterTouch(E);
						}
						
					}
				}



				if(distance < 350)
				{
					
						TheBall->PrepareThrow(direction,
												thePlayer->getThr(),
												false,
												false,
												false);
						PLAY_THROW;
				}
				else
				{
					
						TheBall->PrepareThrow(direction,
											thePlayer->getThr(),
											  true,
											  false,
											  false);
						PLAY_THROW;
				}

				thePlayer->SetDirection(direction);
				return true;
			}

		}

	}
	else
	{
		CActor*  Target = FindClosestTarget(thePlayer);
		int TargetDistance = WorkOutDist((CActor*)thePlayer, Target);

		if(		(TargetDistance < 200)
			&&	((thePlayer->getInt() - 50) > GetRandomNumber() ) )
		{
			if(		(Target->getyPos() ==  32)
				||	(Target->getyPos() == -32) )
			{
				if(thePlayer->getyPos() >= 0 )
					direction = S;
				else
					direction = N;
			}
			else
			{
				
				direction = WorkOutDirection(	thePlayer->getXPos(),
												thePlayer->getYPos(),
												Target->getXPos(),
												Target->getYPos());
			}

			TheBall->PrepareThrow(direction,  thePlayer->getThr(),
											  false,
											  false,
											  false);
			PLAY_THROW;
			return true;
		}
	}

	return false;
}
//***************************************************************************************
//the player is the person who is throwoing the ball.
//line to pass to is the position you would pefer the ball to go to
// 0 == FWDS
// 3 == MID
// 2 == DEF
bool	CMatch::DoPass(CTeamMember* thePlayer, int LineToPassTo)
{
	CTeamMember* temp;
	CTeam*		 team;
	CTeam*		 oppTeam;
	int			 dist;
	
	if((thePlayer->GetPossesion()) == false)
	{
		return false;
	}

	if (GameHalf == FIRST_HALF)
	{
		if (thePlayer->DirectionOfPlay == DOWN_MAP)
		{
			team	= GameDataRef->GetActiveTeamByIndex(1);
			oppTeam = GameDataRef->GetActiveTeamByIndex(0);
		}
		else
		{
			team	= GameDataRef->GetActiveTeamByIndex(0);
			oppTeam = GameDataRef->GetActiveTeamByIndex(1);
		}
	}
	else
	{
		if (thePlayer->DirectionOfPlay == DOWN_MAP)
		{
			team	= GameDataRef->GetActiveTeamByIndex(0);
			oppTeam = GameDataRef->GetActiveTeamByIndex(1);
		}
		else
		{
			team	= GameDataRef->GetActiveTeamByIndex(1);
			oppTeam = GameDataRef->GetActiveTeamByIndex(0);
		}
	}
/*
	if(thePlayer->getTeam())
	{
		team	= GameDataRef->GetActiveTeamByIndex(1);
		oppTeam = GameDataRef->GetActiveTeamByIndex(0);
	}
	else
	{
		team	= GameDataRef->GetActiveTeamByIndex(0);
		oppTeam = GameDataRef->GetActiveTeamByIndex(1);
	}
*/
	int throwDistance = thePlayer->getThr() * 2.5; //rough approximation of the orginal
	//find all players on your team within that distance
	for(int i = 0; i < 9 ; i++)
	{
		temp = team->GetTeamMember(i);
		//don't even try passing it too yourself dude!
		if (i == thePlayer->getPosition())
			continue;

		//if(temp->getPosition() > (thePlayer->getPosition() + 2))
		//continue;

#define TOP_GOAL_Y		538
#define	BOTTOM_GOAL_Y  -538
		
		dist = this->WorkOutDist((CActor*)thePlayer, (CActor*)temp);

		if( dist > throwDistance)
			continue;

		int playergoalDist = this->WorkOutDist((CActor*)thePlayer, (CActor*)oppTeam->GetTeamMember(GOAL));
		int tempgoalDist = 0;
		if(temp->DirectionOfPlay == UP_MAP)
		{
			int distA = this->WorkOutDist((CActor*)temp, NULL, GOAL_X_START, TOP_GOAL_Y);
			int distB = this->WorkOutDist((CActor*)temp, NULL,  GOAL_X_END, TOP_GOAL_Y);
			if(distA < distB)
				tempgoalDist = distA;
			else
				tempgoalDist = distB;
		}
		else
		{
			int distA = this->WorkOutDist((CActor*)temp, NULL, GOAL_X_START, BOTTOM_GOAL_Y);
			int distB = this->WorkOutDist((CActor*)temp, NULL,  GOAL_X_END, BOTTOM_GOAL_Y);
			if(distA < distB)
				tempgoalDist = distA;
			else
				tempgoalDist = distB;
		}
		//if they can throw that far have a look for other things
		//and make sure they are not throwing it to someone who is next to them
		if (	(dist > 50)			
			&&	(tempgoalDist<playergoalDist) )
		{
			int direction = this->WorkOutDirection(thePlayer->getXPos(), 
													thePlayer->getYpos(), 
													temp->getXPos(), 
													temp->getYpos());
			//if there is someone in the way
			if(CheckLOS(thePlayer->getXPos(), thePlayer->getYpos(), temp->getXPos(), temp->getYpos(), thePlayer, throwDistance, oppTeam))
			{
				//and they fail an intelligence check
				if( (thePlayer->getInt() < this->GetRandomNumber()) )
				{
					//if they fail another
					if( (thePlayer->getInt() < this->GetRandomNumber()) )
					{
						//then throw low
						TheBall->PrepareThrow(direction,
												thePlayer->getThr(),
											  false,
											  false,
											  false);
						PLAY_THROW;
											  
						return true;
					}
					else
					{
						//otherwise thorw high... they're not being that dum
						
						TheBall->PrepareThrow(direction,
											  thePlayer->getThr(),
											  true,
											  false,
											  false);
						PLAY_THROW;
						
						return true;
					}
				}
				else
				{
					//if they are smart take another check
					if( (thePlayer->getInt() < this->GetRandomNumber()) )
					{
						//being stoopid but a bit smart throw high
						
						TheBall->PrepareThrow(direction,
												thePlayer->getThr(),
												true,
												false,
												false);
						PLAY_THROW;
						return true;
					}
					else
					{
						//they are not stupid.. don't throw at all
						continue;
					}
				}
			}
			else
			{
				//if there is no one in the way
				if(dist < (thePlayer->getThr() * 1.5) )
				{
				
					TheBall->PrepareThrow(direction,
												thePlayer->getThr(),
											    false,
											    false,
											    false);
					PLAY_THROW;
					return true;
				}
				else
				{
					
					TheBall->PrepareThrow( direction,
											  thePlayer->getThr(),
											  true,
											  false,
											  false);
					PLAY_THROW;
					return true;
				}
			}
		}
		
	}
	//it's a no throw!
	//but wait.. if they are dum.. being random or just want to throw it away
	//i.e. in the goalie's case

	//a small fix here to make sure that people aren't stupid in the corners
	//JUST STOP BLOODY THORWING RANDOMLY WHEN THERE IS A PERFECTLY GOOD GOAL TO SHOOT AT!!!!!!!!!!
	/*
	if(		( (thePlayer->getInt() + GetRandomNumber()) < GetRandomNumber())
		||	(thePlayer->getPosition() == GOAL) )
	{
		int random = GetRandomNumber();
		if(thePlayer->DirectionOfPlay == UP_MAP)
		{
			if(random < 80)
			{
				if(random < 40)
				{
					
					TheBall->PrepareThrow( N, thePlayer->getThr(),
										   true,
							   			   false,
										   false);
					return true;
				}
				else
				{
					
					TheBall->PrepareThrow( N, thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}

			if(random < 160)
			{
				if(random < 120)
				{
					
					TheBall->PrepareThrow( NW, thePlayer->getThr(),
											  true,
											  false,
											  false);
					return true;
				}
				else
				{
				
					TheBall->PrepareThrow( NW, thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}

			if(random < 240)
			{
				if(random < 200)
				{
					
					TheBall->PrepareThrow( NE, thePlayer->getThr(),
											  true,
											  false,
											  false);
					
					return true;
				}
				else
				{
					
					TheBall->PrepareThrow( NE,thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}
		}
		else
		{
			if(random < 80)
			{
				if(random < 40)
				{
					
					TheBall->PrepareThrow( S, thePlayer->getThr(),
										   true,
							   			   false,
										   false);
					return true;
				}
				else
				{
					
					TheBall->PrepareThrow( S, thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}

			if(random < 160)
			{
				if(random < 120)
				{
					
					TheBall->PrepareThrow( SW, thePlayer->getThr(),
											  true,
											  false,
											  false);
					return true;
				}
				else
				{
				
					TheBall->PrepareThrow( SW, thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}

			if(random < 240)
			{
				if(random < 200)
				{
					
					TheBall->PrepareThrow( SE, thePlayer->getThr(),
											  true,
											  false,
											  false);
					return true;
				}
				else
				{
					
					TheBall->PrepareThrow( SE, thePlayer->getThr(),
											  false,
											  false,
											  false);
					return true;
				}
			}	
		}
	}
	*/
	return false;
	
}
//****************************************************************************************
bool	CMatch::DoRun(CTeamMember* thePlayer)
{
	int distanceToBall = this->WorkOutDist((CActor*) thePlayer, (CActor*) TheBall); 
	int	directionToBall = this->WorkOutDirection(	thePlayer->getXPos(),
													thePlayer->getYPos(),
													TheBall->getXPos(),
													TheBall->getYPos());
	CToken* nearToken = FindClosestToken(thePlayer);
	//first things first.. if there is a token nearby they might want to grab it
	if( nearToken != NULL)
	{
		if( (WorkOutDist(thePlayer, nearToken)) < 75)
		{
			if(thePlayer->getInt() > GetRandomNumber())
			{
				thePlayer->SetDestination( nearToken->getXPos(), nearToken->getYPos() );
				return true;
			}
		}
	}

	//if they haven't got possesion then go for the ball
	if(thePlayer->GetPossesion() == false)
	{
		if(thePlayer->getPosition() == GOAL)
		{
			int ballDist = WorkOutDist((CActor*)thePlayer, (CActor*)TheBall);

			if(ballDist < 75)
			{
				thePlayer->SetDestination(TheBall->getXPos(), thePlayer->getYPos());
				if(TheBall->GetHeight() > 1)
				{
					if(distanceToBall < 50)
					{
						thePlayer->Jump(directionToBall);
						return true;
					}
				}
				if(	( (thePlayer->getAgr()) > GetRandomNumber() ) )
				//	&&	(ballDist > 25))
				{
					thePlayer->Slide(directionToBall);
					return true;
				}
				return true;
			}
			else
			{
				int xDest;
				int yDest;

				if(		(TheBall->getXPos() < 65)
					&&	(TheBall->getXPos() > -65) )
				{
					xDest = TheBall->getXPos();
				}
				else
				{
					if(thePlayer->DirectionOfPlay == UP_MAP)
					{
						xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), SW);

						if(		(xDest < 65)
						&&		(xDest > -65) )
						{
						}
						else
						{
							xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), SE);

							if(		(xDest < 65)
							&&		(xDest > -65) )
							{
							}
							else
							{
								xDest = 0;
							}
						}
					}
					else
					{
						xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), NW);
						if(		(xDest < 65)
						&&		(xDest > -65) )
						{
						}
						else
						{
							xDest = this->GetEndXOfLine(TheBall->getXPos(), TheBall->getYPos(), NE);
						}
					}
				}
				if(thePlayer->DirectionOfPlay == UP_MAP)
					yDest = PITCH_BLY + 32;
				else
					yDest = PITCH_TLY - 32;

				thePlayer->SetDestination(xDest, yDest);
				return true;
			}
		}
		//if you are not a goalie
		thePlayer->SetDestination(TheBall->getXPos(), TheBall->getYPos());

		if(TheBall->GetHeight() > 1)
		{
			if(distanceToBall < 50)
			{
				if(thePlayer->getInt() > GetRandomNumber())
				{
					thePlayer->Jump(directionToBall);
					return true;
				}
			}
		}
		if(	(	(thePlayer->getAgr() - GetRandomNumber()) > GetRandomNumber() ))
		{
			thePlayer->Slide(directionToBall);
			return true;
		}

		if(		( TheBall->IsBallHeld() )
			&&	( distanceToBall < 25)  )
		{
			thePlayer->Slide(directionToBall);
		}
		return true;
	}
	
	CTeamMember* ClosestOpponent;
	if (GameHalf == FIRST_HALF)
	{
		if (thePlayer->DirectionOfPlay == DOWN_MAP)
			ClosestOpponent	= this->FindClosestOpponent(thePlayer, GameDataRef->GetActiveTeamByIndex(0));
		else
			ClosestOpponent	= this->FindClosestOpponent(thePlayer, GameDataRef->GetActiveTeamByIndex(1));
	}
	else
	{
		if (thePlayer->DirectionOfPlay == DOWN_MAP)
			ClosestOpponent	= this->FindClosestOpponent(thePlayer, GameDataRef->GetActiveTeamByIndex(1));
		else
			ClosestOpponent	= this->FindClosestOpponent(thePlayer, GameDataRef->GetActiveTeamByIndex(0));
	}
	//don't need to work these out unless you have the ball
	//CTeamMember* ClosestOpponent	= this->FindClosestOpponent(thePlayer, GameDataRef->GetActiveTeamByIndex(0));
	int distanceToClosestOpponent	= this->WorkOutDist((CActor*)thePlayer, (CActor*)ClosestOpponent);

	int directionOfClosestOpponent	= this->WorkOutDirection(	thePlayer->getXPos(),
																thePlayer->getYPos(),
																ClosestOpponent->getXPos(),
																ClosestOpponent->getYPos());

	//This next bit of code is to work out if the closest opponent is in front of the comp
	
	int dir = thePlayer->GetDirectionFacing();
	int tempdir = dir;
	int intelligence = thePlayer->getInt();
	int aggression	 = thePlayer->getAgr();
	int rand = GetRandomNumber();
	int Xchange = 0;
	
	if(		((dir == directionOfClosestOpponent)
		||	((dir + 1) == directionOfClosestOpponent)
		||	((dir - 1) == directionOfClosestOpponent))
		&&	(distanceToClosestOpponent < (thePlayer->getInt() / 2))	)
	{
		if(aggression < rand)
		{
			DoPass(thePlayer);
			return false;
		}
		
		if(intelligence < (rand - 50))
		{
			if(rand < 85)
			{
				Xchange += 75;
			}
			else
			{
				if(rand > 170)
				{
					Xchange -= 75;
				}
				else
				{
					DoPass(thePlayer);
					return true;
				}
			}
		}
	}

	rand = GetRandomNumber();

	tempdir -= 1;
	if(	(tempdir < 0))
		tempdir = 7;

	if(		((dir == directionOfClosestOpponent)
		||	((dir + 1) == directionOfClosestOpponent)
		||	((dir - 1) == directionOfClosestOpponent))
		&&	(distanceToClosestOpponent < (thePlayer->getInt() / 2))	)
	{
		if(aggression < rand)
		{
			DoPass(thePlayer);
			return true;
		}
		
		if(intelligence < (rand - 50))
		{
			if(rand < 85)
			{
				Xchange += 75;
			}
			else
			{
				if(rand > 170)
				{
					Xchange += 150;
				}
				else
				{
					DoPass(thePlayer);
					return true;
				}
			}
		}
	}

	rand = GetRandomNumber();

	tempdir += 1;
	if(tempdir > 7)
		tempdir = 0;

	tempdir += 1;
	if(tempdir > 7)
		tempdir = 0;

	if(		((dir == directionOfClosestOpponent)
		||	((dir + 1) == directionOfClosestOpponent)
		||	((dir - 1) == directionOfClosestOpponent))
		&&	(distanceToClosestOpponent < (thePlayer->getInt() / 2))	)
	{
		if(aggression < rand)
		{
			DoPass(thePlayer);
			return true;
			
		}

		rand = GetRandomNumber();

		if(intelligence < (rand - 50))
		{
			if(rand < 85)
			{
				Xchange -= 75;
			}
			else
			{
				if(rand > 170)
				{
					Xchange -= 150;
				}
				else
				{
					DoPass(thePlayer);
					return true;
				}
			}
		}
	}
	//complex but it hopefully works;
	int dirChange; //= (WorkOutDirection(thePlayer->getXPos(), thePlayer->getYPos(), 0, TOP_GOAL_Y));

	if(thePlayer->DirectionOfPlay == UP_MAP)
	{
		if(		(thePlayer->getXPos() > 40)
		&&	(thePlayer->getYPos() > 250))
		{
			Xchange -= (thePlayer->getXPos() - 40);
		}
		
		if(		(thePlayer->getXPos() < -40)
			&&	(thePlayer->getYPos() > 250))
		{
			Xchange += (- 40 - thePlayer->getXPos());
		}

		dirChange = (WorkOutDirection(thePlayer->getXPos(), thePlayer->getYPos(), 0, TOP_GOAL_Y));
		//************
		//this line added to make sure the player doesn't run to the wall
		/*
		if(		(dirChange != N)
			&&	(dirChange != NW)
			&&	(dirChange != NE) )
		{
			if(intelligence < GetRandomNumber())
			{
				thePlayer->SetDestination(0, (TOP_GOAL_Y - 50));
				return true;
			}
			else
			{
				//I know this really shouldn't be here but this would be thier last resort
				DoPass(thePlayer);
				return true;
			}
		}
		*/
		if(	thePlayer->getYPos() >  328)
		{
			if(thePlayer->getXPos() < -40)
			{
				thePlayer->SetDestination(	(thePlayer->getXPos() + GetRandomNumber() ),
											(TOP_GOAL_Y - GetRandomNumber())	);
				return true;
			}
			else
			{
				if(thePlayer->getXPos() >  40)
				{
					thePlayer->SetDestination(	(thePlayer->getXPos() - GetRandomNumber() ),
												(TOP_GOAL_Y - GetRandomNumber())	);
					return true;
				}
			}
		}
		else
		{
			if(CheckLOS(thePlayer->getXPos(),thePlayer->getYPos(), (thePlayer->getXPos() + Xchange),(thePlayer->getYPos() + 20), thePlayer, thePlayer->getInt(), GameDataRef->GetActiveTeamByIndex(1)))
			{
				if(GetRandomNumber() < 127)
				{
					thePlayer->SetDestination((thePlayer->getXPos() + 60), (thePlayer->getYPos() + 20));
				}
				else
				{
					//***
					thePlayer->SetDestination((thePlayer->getXPos() - 60), (thePlayer->getYPos() + 20));
				}
			}
			else
			{
				thePlayer->SetDestination( (thePlayer->getXPos() + Xchange), (thePlayer->getYPos() + 50) );
			}
		}
		return true;
	}
	else
	{
		if(		(thePlayer->getXPos() > 40)
		&&	(thePlayer->getYPos() < -250))
		{
			Xchange -= (thePlayer->getXPos() - 40);
		}
		
		if(		(thePlayer->getXPos() < -40)
			&&	(thePlayer->getYPos() < -250))
		{
			Xchange += (- 40 - thePlayer->getXPos());
		}

		dirChange = (WorkOutDirection(thePlayer->getXPos(), thePlayer->getYPos(), 0, BOTTOM_GOAL_Y));
/*
		if(		(dirChange != S)
			&&	(dirChange != SW)
			&&	(dirChange != SE) )
		{
			if(intelligence < GetRandomNumber())
			{
				thePlayer->SetDestination(0, (BOTTOM_GOAL_Y + 50));
				return true;
			}
			else
			{
				//I know this really shouldn't be here but this would be thier last resort
				DoPass(thePlayer);
				return true;
			}
		}
*/

		if(	thePlayer->getYPos() < -328)
		{
			if(thePlayer->getXPos() < -40)
			{
				thePlayer->SetDestination(	(thePlayer->getXPos() + GetRandomNumber() ),
											(BOTTOM_GOAL_Y + GetRandomNumber())	);
				return true;
			}
			else
			{
				if(thePlayer->getXPos() >  40)
				{
					thePlayer->SetDestination(	(thePlayer->getXPos() - GetRandomNumber() ),
												(BOTTOM_GOAL_Y + GetRandomNumber())	);
					return true;
				}
			}
		}

		else
		{
			if(CheckLOS(thePlayer->getXPos(),thePlayer->getYPos(), (thePlayer->getXPos() + Xchange),(thePlayer->getYPos() + 20), thePlayer, thePlayer->getInt(), GameDataRef->GetActiveTeamByIndex(0)))
			{
				if(GetRandomNumber() < 127)
				{
					thePlayer->SetDestination((thePlayer->getXPos() + 60), (thePlayer->getYPos() - 20));
				}
				else
				{
					thePlayer->SetDestination((thePlayer->getXPos() - 60), (thePlayer->getYPos() - 20));
				}
			}
			else
			{
				thePlayer->SetDestination( (thePlayer->getXPos() + Xchange), (thePlayer->getYPos() - 50) );
			}
		}
		return true;
	}

	return false;

}
//****************************************************************************************
bool	CMatch::DoTarget(CTeamMember* thePlayer)
{

	return false;
}
//****************************************************************************************
//****************************************************************************************
// Some useful utility functions
//****************************************************************************************
int		CMatch::WorkOutDist(CActor* ObjectOne, CActor* ObjectTwo, int endX, int endY)
{
	int x, y;
	if(ObjectTwo != NULL)
	{
		x = (ObjectOne->getXPos()) - (ObjectTwo->getXPos());
		y = (ObjectOne->getYpos()) - (ObjectTwo->getYpos());
	}
	else
	{
		x = (ObjectOne->getXPos()) - (endX);
		y = (ObjectOne->getYpos()) - (endY);
	}
	
	int t = sqrt(((x * x) + (y * y)));
	return t;
}
//****************************************************************************************
bool		CMatch::CheckLOS(int startX, int startY, int xDest, int yDest,CTeamMember* thePlayer, int distance, CTeam* oppTeam)
{
	
	int direction = WorkOutDirection(startX, startY, xDest, yDest);
	int i = 0;
	float ratioX = 0, ratioY = 0;
	bool	pitw = false; //pitw PlayerInTheWay

	if(oppTeam != NULL)
	{
		CTeamMember* temp;
		switch(direction)
		{
		case N:
			yDest += distance;
			for(i = 0; i < 9; i++)
			{
				temp = oppTeam->GetTeamMember(i);
				if(startY < temp->getYpos())
				{
					if((temp->getXPos() < (startX + 16)) && (temp->getXPos() > (startX - 16)))
					{
						if(		(temp->getState() != FALLING)
							||	(temp->getState() != FLOORED) )
							pitw = true;		
					}
				}
			}

			if(startY < TSHROOM_Y)
			{
				if((-24 <= (startX - 16)) && (24 >= (startX + 16)))
					pitw = true;
			}
			break;
		case S:
			yDest -=distance;
			for(i = 0; i < 9; i++)
			{
				temp = oppTeam->GetTeamMember(i);
				if(startY > temp->getYpos())
				{
					if((temp->getXPos() < (startX + 16)) && (temp->getXPos() > (startX - 16)))
					{
						if(		(temp->getState() != FALLING)
							||	(temp->getState() != FLOORED) )
							pitw = true;				
					}
				}
			}

			if(startY > BSHROOM_Y)
			{
				if((-24 <= (startX - 16)) && (24 >= (startX + 16)))
					pitw = true;
			}
			break;
		case E:
			xDest += distance;
			for(i = 0; i < 9; i++)
			{
				temp = oppTeam->GetTeamMember(i);
				if(startX < temp->getXPos())
				{
					if((temp->getYpos() < (startY + 16)) && (temp->getYpos() > (startY - 16)))
					{
						if(		(temp->getState() != FALLING)
							||	(temp->getState() != FLOORED) )
							pitw = true;				
					}
				}
			}
			break;
		case W:
			xDest -=distance;
			for(i = 0; i < 9; i++)
			{
				temp = oppTeam->GetTeamMember(i);
				if(startX > temp->getXPos())
				{
					if((temp->getYpos() < (startY + 16)) && (temp->getYpos() > (startY - 16)))
					{
						if(		(temp->getState() != FALLING)
							||	(temp->getState() != FLOORED) )
							pitw = true;				
					}
				}
			}
			break;
		default:
			switch(direction)
			{
			case NE:
				yDest += distance;
				xDest += distance;
				break;
			case NW:
				yDest += distance;
				xDest -= distance;
				break;
			case SE:
				yDest -= distance;
				xDest += distance;
				break;
			case SW:
				yDest -= distance;
				xDest -= distance;
				break;
			}
			if( startX != 0)
			{
				ratioX = (xDest / startX);
			}
			else
			{
				startX = 1;
				ratioX = 0.000001f;
			}

			if(startY != 0)
			{
				ratioY = (yDest / startY);
			}
			else
			{
				startY= 1;
				ratioY = 0.000001f;
			}
			
			for(i = 0; i < 9; i++)
			{
				temp = oppTeam->GetTeamMember(i);
				
				if(((temp->getXPos() / startX) < ratioX+6) && ((temp->getXPos() / startX) > ratioX-6))
				{
					if(((temp->getYpos() / startY) < ratioY+6) && ((temp->getYpos() / startY) > ratioY-6))
					{
						if(		(temp->getState() != FALLING)
							||	(temp->getState() != FLOORED) )
							pitw = true;
					}
				}
			}
		}

		return pitw;
	}
	else
	{
		return false;
	}
	
	return false;
}
//****************************************************************************************
int		CMatch::WorkOutDirection(int startX, int startY, int xDest, int yDest, int Conduit)
{
	if (	((startY + Conduit) > yDest) 
		&&	((startY - Conduit) < yDest))
	{
		if (startX < xDest)
		{
			return E;
		}
		else
		{
			return W;
		}
	}

	if (	((startX + Conduit) > xDest) 
		&&	((startX - Conduit) < xDest))
	{
		if (startY < yDest)
		{
			return N;
		}
		else
		{
			return S;
		}
	}

	if (startY < yDest)
	{
		if (startX < xDest)
		{
			return NE;
		}
		else
		{
			return NW;
		}
	}

	if (startY > yDest)
	{
		if (startX < xDest)
		{
			return SE;
		}
		else
		{
			return SW;
		}
	}
	return UP;
}
//********************************************************************************
//Two functions for working out where a line will end
//********************************************************************************
int		CMatch::GetEndXOfLine(int startX, int startY, int direction)
{
	while(((startX < PITCH_TRX) && (startX > PITCH_TLX)) && ((startY < PITCH_TLY) && (startY > PITCH_BLY)))
	{
		switch(direction)
		{
			case N:
				
				startY++;
				break;
			case NE:
				startX++;
				startY++;
				break;
			case E:
				startX++;
				
				break;
			case SE:
				startX++;
				startY--;
				break;
			case S:
				
				startY--;
				break;
			case SW:
				startX--;
				startY--;
				break;
			case W:
				startX--;
				
				break;
			case NW:
				startX--;
				startY++;
				break;
			default:
				return -1;
		}
	}
	return startX;
}

int		CMatch::GetEndYOfLine(int startX, int startY, int direction)
{
	if(		(	(startX < 325) && (startX > -325)	) 
		&&	(	(startY < 500) && (startY > -500)	)	)
	{
		switch(direction)
		{
			case N:
				
				startY++;
				break;
			case NE:
				startX++;
				startY++;
				break;
			case E:
				startX++;
				
				break;
			case SE:
				startX++;
				startY--;
				break;
			case S:
				
				startY--;
				break;
			case SW:
				startX--;
				startY--;
				break;
			case W:
				startX--;
				
				break;
			case NW:
				startX--;
				startY++;
				break;
			default:
				return -1;
		}
	}
	return startY;
}
//*********************************************************************************
CToken*		CMatch::FindClosestToken(CTeamMember* thePlayer)
{
	CToken* temp;
	int dist;

	for( int i = 0; i < MAX_TOKENS; i++)
	{
		if(TokenArray[i] != NULL)
		{
			temp = TokenArray[i];

			dist = WorkOutDist((CActor*)thePlayer, (CActor*)temp);

			if(dist < 200)
			{
				return temp;
			}
		}
	}
	return NULL;
}
//*********************************************************************************
//*********************************************************************************
//This next method finds if a Player collides with anything.
//If it does it process a reaction to that object for the player
//it shouldn't matter wether the player is controlled or not.... just skip everything
//but wall collisions
int	CMatch::PlayerCollision(CTeamMember* thePlayer,CTeam* theTeam,  CTeam* oppTeam)
{
	bool collide;
	bool burnt = false;
	
	if(		((thePlayer->getState() != FALLING)
		&&	(thePlayer->getState() != FLOORED)) 
		&&	(MatchState != DoingInjury)		 )
	{
		//check collions with the ball
		collide = CheckCollision((CActor*)thePlayer, (CActor*)TheBall, PLAYER_WIDTH, BALL_WIDTH);
		if(IsActorOnScreen((CActor*)thePlayer)) 
		{	
			if( (collide) && (thePlayer->NotHoldBall == false) && (TheBall->IsBallHeld() == false) && (TheBall->getState() != IN_CHUTE))
			{ 
				if(	TheBall->IsBallBurning())
				{	
					if(TheBall->WhichTeamToBurn == thePlayer->DirectionOfPlay)
					{
						TheBall->BurnCount--;
						thePlayer->FallOver(thePlayer->GetDirection());
						thePlayer->TakeDamage(10); //I'm not to sure if this is correct
						PLAY_TUMBLE;
						PLAY_ARGGH;
						burnt = true;
					}
				}

				if(burnt == false)
				{
					if(thePlayer->getState() != DIVING)
					{	
						if( ((TheBall->GetHeight() != 2) && (thePlayer->GetHeight() != 2)) ||
							((TheBall->GetHeight() == 2) && (thePlayer->GetHeight() == 2)) )
						{
							TheBall->HoldOrLetGoOfBall(thePlayer);
							if(thePlayer->getPosition() == GOAL)
							{
								GameDataRef->GetTeamByIndex(thePlayer->getTeam())->GoalsSaved++;
							}
						}
					}
					else
					{
						//All this is for when the goalie dives for the ball
						int DirectionTravel = TheBall->GetDirection();

						if(thePlayer->DirectionOfPlay == UP_MAP)
						{
							switch(DirectionTravel)
							{
							case N:
								DirectionTravel = N;
								break;
							case NE:
								DirectionTravel = NE;
								break;
							case E:
								DirectionTravel = NW;
								break;
							case SE:
								DirectionTravel = NW;
								break;
							case S:
								switch(thePlayer->GetDirectionFacing())
								{
								case N:
								case S:
									DirectionTravel = N;
									break;
								case NE:
								case E:
								case SE:
									DirectionTravel = NE;
									break;
								case SW:
								case W:
								case NW:
									DirectionTravel = NW;
									break;
								default:
									DirectionTravel = N;
									break;
								}
								break;
							case SW:
								DirectionTravel = NE;
								break;
							case W:
								DirectionTravel = NE;
								break;
							case NW:
								DirectionTravel = NW;
								break;
							default:
								TheBall->HoldOrLetGoOfBall(thePlayer);
								break;
							}
						}
						else
						{
							switch(DirectionTravel)
							{
							case N:
								switch(thePlayer->GetDirectionFacing())
								{
								case N:
								case S:
									DirectionTravel = S;
									break;
								case NE:
								case E:
								case SE:
									DirectionTravel = SE;
									break;
								case SW:
								case W:
								case NW:
									DirectionTravel = SW;
									break;
								default:
									DirectionTravel = S;
									break;
								}
								break;
								break;
							case NE:
								DirectionTravel = SW;
								break;
							case E:
								DirectionTravel = SW;
								break;
							case SE:
								DirectionTravel = SE;
								break;
							case S:
								DirectionTravel = S;
								break;
							case SW:
								DirectionTravel = SW;
								break;
							case W:
								DirectionTravel = SE;
								break;
							case NW:
								DirectionTravel = SE;
								break;
							default:
								TheBall->HoldOrLetGoOfBall(thePlayer);
								break;
							}
						}

						if(TheBall->IsBallHeld() == false)
							TheBall->GetThrown(thePlayer->getThr(), true, true, DirectionTravel);

						GameDataRef->GetTeamByIndex(thePlayer->getTeam())->GoalsSaved++;
					}
				}
			}

			//*******
			
			//**********************

			//check collisions with opposing team members
			CTeamMember* temp;
			
			for(int i = 0; i < 9; i++)
			{
				if(GameDataRef->GetGameType() == PRACTICE)
					continue;

				temp = oppTeam->GetTeamMember(i);
				if(!IsActorOnScreen((CActor*)temp))
					continue;

				collide = CheckCollision((CActor*) thePlayer,(CActor*) temp, PLAYER_WIDTH, PLAYER_WIDTH);
				if(	(temp->getState() == FLOORED))
				{
					if(collide)
					{
						temp->setState(FLOORED);
					}
					continue;
				}

				if( (temp->getState() == FALLING) )
				{
					continue;
				}

				if(collide)
				{
					//****
					if(temp->getState() != SLIDING)
					{
						if(thePlayer->getXPos() > temp->getXPos())
						{
							thePlayer->SetXY( (temp->getXPos() + (PLAYER_WIDTH *2)), thePlayer->getYPos());
						}

						if(thePlayer->getXPos() < temp->getXPos())
						{
							thePlayer->SetXY( (temp->getXPos() - (PLAYER_WIDTH *2)), thePlayer->getYPos());
						}

						if(thePlayer->getYPos() > temp->getYPos())
						{
							thePlayer->SetXY( thePlayer->getXPos(), (temp->getYPos() + (PLAYER_WIDTH *2)));
						}

						if(thePlayer->getYPos() < temp->getYPos())
						{
							thePlayer->SetXY( thePlayer->getXPos(), (temp->getYPos() - (PLAYER_WIDTH *2)));
						}
					}
					int dir = WorkOutDirection( thePlayer->getXPos(),
												thePlayer->getYPos(),
												temp->getXPos(),
												temp->getYPos());
					
					//****
					//if you are sliding or puching.. check stats against them to see if you
					//take them out.
					if(		(thePlayer->getState() == SLIDING)
						||	(thePlayer->getState() == PUNCHING)	
						||	(thePlayer->getState() == DIVING))
					{
						int attack = thePlayer->getAtt() + GetRandomNumber();
						int defence = temp->getDef() + GetRandomNumber();

						if(temp->getState() == JUMPING)
						{
							attack += 64;
						}
						
						//dir == direction from thePlayer to temp
						//so if dir == temp->facingDirection then he must be facing away
						int	tempFace = temp->GetDirectionFacing();
						
						if(dir == tempFace)
							attack += 64;
						if(attack > defence)
						{
							//This bit of code will basically make it so that if two people slide into
							//one another there is a chnce that they both go down
							if(temp->getState() == SLIDING)
							{
								int  defenceB = thePlayer->getDef() + GetRandomNumber();
								int  attackB  = temp->getAtt() + GetRandomNumber();
								if(attackB > defenceB)
								{
									thePlayer->FallOver(temp->GetDirection());
									thePlayer->TakeDamage(temp->getPow()/15);
								}
							}

							GameDataRef->GetTeamByIndex(thePlayer->getTeam())->SuccesfulTackles++;
							temp->FallOver(temp->GetDirection());
							temp->TakeDamage(thePlayer->getPow()/15);
							if(TheBall->IsBallHeld())
							{
								if(temp->HasPossesion)
								{
									//temp->DropBall();
									TheBall->HoldOrLetGoOfBall();

									if(thePlayer->NotHoldBall == false)
									{
										TheBall->HoldOrLetGoOfBall(thePlayer);
										if(TheBall->IsBallBurning())
										{
											TheBall->WhichTeamToBurn = temp->DirectionOfPlay;
										}
									}
									
									PLAY_TUMBLE;
									if(GetRandomNumber() > 220)
										PLAY_CROWD2;
								}
							}
							else
							{
								PLAY_TUMBLE;
							}

							if(GetRandomNumber() > 123)
							{
								PLAY_OOF;
							}
							else
							{
								PLAY_ARGGH;
							}

							if(temp->getCurrentStamina() < 0)
							{
								MatchState = DoingInjury;
							//	OutputDebugString(TEXT("Injury Caused\n"));
							}
						}
					}
					else
					{
						if(		(thePlayer->getAgr() > GetRandomNumber() )
							&&	(thePlayer->IsControlled != true)		)
						{
							thePlayer->Slide(thePlayer->GetDirectionFacing());
						}
					}	
				}
			}
		}
	}

	//check collisions with walls
	if(thePlayer->getXPos() < PITCH_TLX)
		thePlayer->SetXY(PITCH_TLX, thePlayer->getYpos());

	if(thePlayer->getXPos() > PITCH_BRX)
		thePlayer->SetXY(PITCH_BRX, thePlayer->getYpos());

	if(thePlayer->getYpos() > PITCH_TLY)
		thePlayer->SetXY(thePlayer->getXPos(), PITCH_TLY);

	if(thePlayer->getYpos() < PITCH_BLY)
		thePlayer->SetXY(thePlayer->getXPos(), PITCH_BLY);
	
	CToken* notPerm;
	//check collisions with tokens
	if(thePlayer->IsControlled)
	{
		for( int i = 0; i < MAX_TOKENS; i++)
		{
			if(		(TokenArray[i] != NULL)
				&&	(TokenArray[i]->getState() != WEAPON_GOT) )
			{
				notPerm = TokenArray[i];
				if(this->CheckCollision((CActor*) thePlayer, (CActor*) notPerm, PLAYER_WIDTH, TOKEN_WIDTH))
				{
					if(GameDataRef->GetGameType() != CUP)
						notPerm->OnPickUp(theTeam, oppTeam, GameHalf, TheBall);
					else
						notPerm->OnPickUp(theTeam, oppTeam, GameHalf, TheBall, true);

					PLAY_POWERUP;
				}
			}
		}
	}
	return 1;
}
//*********************************************************************************
//This method finds out if a ball has collided with anything
//that it can collide with
int CMatch::BallCollision(CBall* theBall)
{
	//firstly check to see if the ball has collided with a wall
	//then check to see if the location on the wall was in fact a bonus thing or a goal
	if(		(theBall->getXPos() < (PITCH_TLX - 8) )
		&&	(theBall->MoveX < 0)	)
	{
		
		//check here to check for star hits
		if (	(theBall->getYpos() > LEFT_STARBANK_SY) 
			&&	(theBall->getYpos() < LEFT_STARBANK_EY))
		{
			if(LeftBank->GetHit(theBall->LastHeldBy, theBall->getYpos()))
			{
				PLAY_STAR;
			}
			else
			{
				PLAY_WALL;
			}
			theBall->DoBounce();
			//quick calc to put the ball on the right side of the wall
			int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_TLX - 16) ) * 2);
			theBall->SetXY( newX, theBall->getYPos() );
		}
		else
		{
			//check for dome hits
			if((theBall->getYpos() > DOME_LEFT_EY) && (theBall->getYpos() < DOME_LEFT_SY))
			{
				if(theBall->getState() == HIGH)
				{
					PLAY_WALL;
					theBall->DoBounce();
					int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_TLX - 16) ) * 2);
					theBall->SetXY( newX, theBall->getYPos() );
					return 0;
				}

				if(TheBall->LastHeldBy->getTeam() == GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber())
				{
					LeftDome->OnHit(theBall, TeamAMulti);
					PLAY_ELECTRO;
				}
				else
				{
					LeftDome->OnHit(theBall, TeamBMulti);
					PLAY_ELECTRO;
				}
			}
			else
			{
				//check for warp hits
				if (	(theBall->getYpos() > TOP_WARP_SY) 
					&&	(theBall->getYpos() < TOP_WARP_EY))
				{
					if(theBall->getState() == HIGH)
					{
						PLAY_WALL;
						theBall->DoBounce();
						int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_TLX - 16) ) * 2);
						theBall->SetXY( newX, theBall->getYPos() );
						return 0;
					}

					theBall->SetXY(( theBall->getXPos() * -1), (theBall->getYpos() + 10));
					return 1;
				}
				else
				{
					if (	(theBall->getYpos() < BOTTOM_WARP_SY)
						&&	(theBall->getYpos() > BOTTOM_WARP_EY))
					{
						if(theBall->getState() == HIGH)
						{
							PLAY_WALL;
							theBall->DoBounce();
							int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_TLX - 16) ) * 2);
							theBall->SetXY( newX, theBall->getYPos() );
							return 0;
						}
						theBall->SetXY( (theBall->getXPos() * -1), (theBall->getYpos() + 10));
						return 1;
					}

					theBall->DoBounce();
					PLAY_WALL;
					//quick calc to put the ball on the right side of the wall
					int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_TLX - 16) ) * 2);
					theBall->SetXY( newX, theBall->getYPos() );
				}
			}
		}
		
	}

	if(		(theBall->getXPos() > (PITCH_BRX + 8))
		&&	(theBall->MoveX > 0)	)
	{
		
		//check here to check for star hits
		if (	(theBall->getYpos() < RIGHT_STARBANK_SY) 
			&&	(theBall->getYpos() > RIGHT_STARBANK_EY))
		{
			if(RightBank->GetHit(theBall->LastHeldBy, theBall->getYpos()))
			{
				PLAY_STAR
			}
			else
			{
				PLAY_WALL;
			}
			theBall->DoBounce();
			
			int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_BRX + 16)) * 2);
			theBall->SetXY( newX, theBall->getYPos() );
		}
		else
		{
			if (	(theBall->getYPos() > DOME_RIGHT_SY) 
				&&	(theBall->getYpos() < DOME_RIGHT_EY))
			{
				if(theBall->getState() == HIGH)
				{
					PLAY_WALL;
					theBall->DoBounce();
					int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_BRX + 16)) * 2);
					theBall->SetXY( newX, theBall->getYPos() );
					return 0;
				}

				if(TheBall->LastHeldBy->getTeam() == GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber())
				{
					RightDome->OnHit(theBall, TeamAMulti);
					PLAY_ELECTRO;
				}
				else
				{
					RightDome->OnHit(theBall, TeamBMulti);
					PLAY_ELECTRO;
				}
			}
			else
			{
				//if you hit a warp, go to the other side
				if (	(theBall->getYpos() > TOP_WARP_SY) 
					&&	(theBall->getYpos() < TOP_WARP_EY))
				{
					if(theBall->getState() == HIGH)	
					{
						PLAY_WALL;
						theBall->DoBounce();
						int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_BRX + 16)) * 2);
						theBall->SetXY( newX, theBall->getYPos() );
						return 0;
					}
					theBall->SetXY(( theBall->getXPos() * -1), (theBall->getYpos() + 20));
					return 1;
				}
				else
				{
					if (	(theBall->getYpos() < BOTTOM_WARP_SY) 
						&&	(theBall->getYpos() > BOTTOM_WARP_EY))
					{
						if(theBall->getState() == HIGH)
						{
							PLAY_WALL;
							theBall->DoBounce();
							int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_BRX + 16)) * 2);
							theBall->SetXY( newX, theBall->getYPos() );
							return 0;
						}
						theBall->SetXY( (theBall->getXPos() * -1), (theBall->getYpos() + 20));
						return 1;
					}

					theBall->DoBounce();
					PLAY_WALL;
					int newX = theBall->getXPos() - (( theBall->getXPos() - (PITCH_BRX +16)) * 2);
					theBall->SetXY( newX, theBall->getYPos() );
				}
			}
		}	
	}

	if(		(theBall->getYpos() > (PITCH_TLY + 16))
		&&	(theBall->MoveY > 0)	)
	{
		if (	(theBall->getXPos() > GOAL_X_START) 
			&&	(theBall->getXPos() < GOAL_X_END))
		{
			if(		(TheBall->GetHeight() < 2)
				&&	(!CheckForGoalDoor(1))	)
			{
				//someone has shot it into the goal mouth!!!
				if(GameHalf == FIRST_HALF)
				{
					if(TheBall->LastHeldBy->DirectionOfPlay == UP_MAP)
					{
						GameDataRef->GetActiveTeamByIndex(0)->IncreaseScore(10*TeamAMulti);
						GameDataRef->GetActiveTeamByIndex(0)->GoalsScored++;
						MatchState = DoingGoalSideA;
						PLAY_AIRHORN;
					}
					else
					{
						GameDataRef->GetActiveTeamByIndex(0)->IncreaseScore(10*TeamAMulti);
						GameDataRef->GetActiveTeamByIndex(0)->GoalsScored++;
						MatchState = DoingHomeGoalSideB;
						PLAY_AIRHORN;
					}
				}
				else
				{
					if(TheBall->LastHeldBy->DirectionOfPlay != UP_MAP)
					{
						GameDataRef->GetActiveTeamByIndex(1)->IncreaseScore(10*TeamBMulti);
						GameDataRef->GetActiveTeamByIndex(1)->GoalsScored++;
						MatchState = DoingHomeGoalSideA;
						PLAY_AIRHORN;
					}
					else
					{
						GameDataRef->GetActiveTeamByIndex(1)->IncreaseScore(10*TeamBMulti);
						GameDataRef->GetActiveTeamByIndex(1)->GoalsScored++;
						MatchState = DoingGoalSideB;
						PLAY_AIRHORN;
					}
					
				}
			}
			else
			{
				theBall->DoBounce();
				PLAY_WALL;
				int newY = theBall->getYpos() - (( theBall->getYpos() - (PITCH_TLY + 16 ) )* 2);
					theBall->SetXY( theBall->getXPos(), newY);
				
			}
		}
		else
		{
				theBall->DoBounce();
				PLAY_WALL;
			int newY = theBall->getYpos() - (( theBall->getYpos() - (PITCH_TLY + 16) ) * 2);
				theBall->SetXY( theBall->getXPos(), newY);
		
		}
	}

	if(		(theBall->getYpos() < (PITCH_BLY - 16))
		&&	(theBall->MoveY < 0)	)
	{
		if (	(theBall->getXPos() > GOAL_X_START)
			&&	(theBall->getXPos() < GOAL_X_END))
		{
			if(		(theBall->GetHeight() < 2)
				&&	(!CheckForGoalDoor(0))   )
			{
				//someone has shot it into the goal mouth!!!
				if(GameHalf == FIRST_HALF)
				{
					if(TheBall->LastHeldBy->DirectionOfPlay == UP_MAP)
					{
						GameDataRef->GetActiveTeamByIndex(1)->IncreaseScore(10*TeamBMulti);
						GameDataRef->GetActiveTeamByIndex(1)->GoalsScored++;
						MatchState = DoingHomeGoalSideA;
						PLAY_AIRHORN;
					}
					else
					{
						GameDataRef->GetActiveTeamByIndex(1)->IncreaseScore(10*TeamBMulti);
						GameDataRef->GetActiveTeamByIndex(1)->GoalsScored++;
						MatchState = DoingGoalSideB;
						PLAY_AIRHORN;
					}
				}
				else
				{
					if(TheBall->LastHeldBy->DirectionOfPlay != UP_MAP)
					{
						GameDataRef->GetActiveTeamByIndex(0)->IncreaseScore(10*TeamAMulti);
						GameDataRef->GetActiveTeamByIndex(0)->GoalsScored++;
						MatchState = DoingGoalSideA;
						PLAY_AIRHORN;
					}
					else
					{
						
						GameDataRef->GetActiveTeamByIndex(0)->IncreaseScore(10*TeamAMulti);
						GameDataRef->GetActiveTeamByIndex(0)->GoalsScored++;
						MatchState = DoingHomeGoalSideB;
						PLAY_AIRHORN;
					}
				}
			}
			else
			{
				theBall->DoBounce();
				PLAY_WALL;
				int newY = theBall->getYpos() - (( theBall->getYpos() - (PITCH_BLY - 16)) * 2);
					theBall->SetXY( theBall->getXPos(), newY);
				
			}
		}
		else
		{
			theBall->DoBounce();
			PLAY_WALL;
			int newY = theBall->getYpos() - (( theBall->getYpos() - (PITCH_BLY - 16) ) * 2);
			theBall->SetXY( theBall->getXPos(), newY);
			
		}
	}

	//if the ball is in the air it waon't hit any of these
	if(TheBall->GetHeight() < 2)
	{
	//then work out if it has collided with any map bonus stuff
		if( CheckCollision(TheBall, TopShroom, BALL_WIDTH, SHROOM_WIDTH) )
		{
			if(TheBall->LastHeldBy->getTeam() == GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber())
			{
				TopShroom->OnHit(TheBall, GameDataRef->GetTeamByIndex(TheBall->LastHeldBy->getTeam()), TeamAMulti);
				PLAY_BOUNCE;
			}
			else
			{
				TopShroom->OnHit(TheBall,GameDataRef->GetTeamByIndex(TheBall->LastHeldBy->getTeam()), TeamBMulti);
				PLAY_BOUNCE;
			}
		}

		if( CheckCollision(TheBall, BottomShroom, BALL_WIDTH, SHROOM_WIDTH) )
		{
			if(TheBall->LastHeldBy->getTeam() == GameDataRef->GetActiveTeamByIndex(0)->GetTeamNumber())
			{
				BottomShroom->OnHit(TheBall, GameDataRef->GetTeamByIndex(TheBall->LastHeldBy->getTeam()), TeamAMulti); 
				PLAY_BOUNCE;
			}
			else
			{
				BottomShroom->OnHit(TheBall, GameDataRef->GetTeamByIndex(TheBall->LastHeldBy->getTeam()), TeamBMulti);
				PLAY_BOUNCE;
			}
		}

		//or collided with a chute.
		if (	(TheBall->GetDirection() == N) 
			||	(TheBall->GetDirection() == S))
		{
			if (TheBall->getXPos() > 246)
			{
				if (	(TheBall->getYpos() > RAMP_RIGHT_Y1) 
					&&	(TheBall->getYpos() < RAMP_RIGHT_Y2))
				{
					if(RightChute->getState() != IN_USE)
					{
						//set the ball going round the chute
						
						LeftChute->DoChute(TheBall, GameHalf);
						LeftChute->setState(NONE);
						RightChute->DoChute(TheBall,GameHalf);
						TheBall->doChute();
						//PLAY_BELL;
					}
				}
			}

			if (TheBall->getXPos() < -246)
			{
				if (	(TheBall->getYpos() < RAMP_LEFT_Y1) 
					&&  (TheBall->getYpos() > RAMP_LEFT_Y2))
				{
					if(LeftChute->getState() != IN_USE)
					{
						//set the ball going round the chute
						
						RightChute->DoChute(TheBall,GameHalf);
						RightChute->setState(NONE);
						LeftChute->DoChute(TheBall, GameHalf);
						TheBall->doChute();
						//PLAY_BELL;
					}
				}
			}
		}
	}

	if(TheBall->GetSpeed() <= 0 )
	{
		if(TheBall->getXPos() < PITCH_TLX)
			TheBall->SetXY(PITCH_TLX, TheBall->getYpos());

		if(TheBall->getXPos() > PITCH_BRX)
			TheBall->SetXY(PITCH_BRX, TheBall->getYpos());

		if(TheBall->getYpos() > PITCH_TLY)
			TheBall->SetXY(TheBall->getXPos(), PITCH_TLY);

		if(TheBall->getYpos() < PITCH_BLY)
			TheBall->SetXY(TheBall->getXPos(), PITCH_BLY);
	}

	return 1;
}
//*********************************************************************************
//This takes two objects and finds any points of collision between them
bool	CMatch::CheckCollision(int startX1, int startY1,
							   int endX1,   int endY1,
							   int startX2, int startY2,
							   int endX2,   int endY2)
{

	if (	(	(startX2 >= startX1) 
			&&	(startX2 <= endX1)) 
		||	(	(endX2 >= startX1) 
			&&	(endX2 <= endX1))
		)
	{
		if (	(	(startY2 >= startY1) 
				&&	(startY2 <= endY1))
			||	(	(endY2 >= startY1)
				&&	(endY2 <= endY1))
		)
		{
			return true;
		}
	}
	
	return false;
}
//overloaded function to check colisions between two objects 
bool	CMatch::CheckCollision(CActor* ObjectOne, CActor* ObjectTwo, int widthOne, int widthTwo)
{
	int dist = WorkOutDist(ObjectOne, ObjectTwo);
	
	if( ((dist - widthOne) - widthTwo) < 1)
	{
		return true;
	}

	return false;
		
}
//*********************************************************************************
int		CMatch::WhichZoneIsPlayerIn(CTeamMember* thePlayer)
{
	int X	= thePlayer->getXPos();
	int Y	= thePlayer->getYPos();
	int pos = 0;

	//theMaximums and minimums of they're position

	int xMax;
	int yMax;
	int xMin;
	int yMin;
	for( int i = 0; i < 9 ; i++)
	{
		int pos = i;
		switch(pos)
		{
		case GOAL:
			xMax = 70;
			yMax = -540;
			xMin = -70;
			yMin = PITCH_BLY;
			break;

		case LDEF:
			xMax = 0;
			yMax = -262;
			xMin = -300;
			yMin = PITCH_BLY;
			break;

		case RDEF:
			xMax = 300;
			yMax = -262;
			xMin = 0;
			yMin = PITCH_BLY;
			break;

		case CMID:
			xMax = 100;
			yMax = 362;
			xMin = -100;
			yMin = -262;
			break;

		case LMID:
			xMax = -100;
			yMax = 362;
			xMin = -300;
			yMin = -262;
			break;

		case RMID:
			xMax = 300;
			yMax = 362;
			xMin = 100;
			yMin = -262;
			break;

		case CFWD:
			xMax = 100;
			yMax = PITCH_TRY;
			xMin = -100;
			yMin = 262;
			break;

		case RWNG:
			xMax = 300;
			yMax = PITCH_TRY;
			xMin = 100;
			yMin = 262;
			break;

		case LWNG:
			xMax = -100;
			yMax = PITCH_TRY;
			xMin = -300;
			yMin = 262;
			break;

		default:
			return false;
			break;
		}

		if(thePlayer->DirectionOfPlay != UP_MAP)
		{
			int temp;

				temp = xMax;
				xMax = xMin;
				xMax = xMax * -1;
				xMin = temp;
				xMin = xMin * -1;

				temp = yMax;
				yMax = yMin;
				yMax = yMax * -1;
				yMin = temp;
				yMin = yMin * -1;
		}

		if(		(X < xMax)
			&&	(X > xMin)
			&&	(Y < yMax)
			&&	(Y > yMin)	)
		{
			return pos;
		}
	}
	return pos;
}
//*********************************************************************************
bool	CMatch::IsPlayerInZone(CTeamMember* thePlayer)
{
	int pos = thePlayer->getPosition();
	int X	= thePlayer->getXPos();
	int Y	= thePlayer->getYPos();

	//theMaximums and minimums of they're position

	int xMax;
	int yMax;
	int xMin;
	int yMin;

	switch(pos)
	{
	case GOAL:
		xMax = 70;
		yMax = -510;
		xMin = -70;
		yMin = PITCH_BLY;
		break;

	case LDEF:
		xMax = 0;
		yMax = -262;
		xMin = -300;
		yMin = PITCH_BLY;
		break;

	case RDEF:
		xMax = 300;
		yMax = -262;
		xMin = 0;
		yMin = PITCH_BLY;
		break;

	case CMID:
		xMax = 100;
		yMax = 262;
		xMin = -100;
		yMin = -262;
		break;

	case LMID:
		xMax = -100;
		yMax = 262;
		xMin = -300;
		yMin = -262;
		break;

	case RMID:
		xMax = 300;
		yMax = 362;
		xMin = 100;
		yMin = -262;
		break;

	case CFWD:
		xMax = 100;
		yMax = PITCH_TRY;
		xMin = -100;
		yMin = 262;
		break;

	case RWNG:
		xMax = 300;
		yMax = PITCH_TRY;
		xMin = 100;
		yMin = 262;
		break;

	case LWNG:
		xMax = -100;
		yMax = PITCH_TRY;
		xMin = -300;
		yMin = 262;
		break;

	default:
		return false;
		break;
	}

	if(thePlayer->DirectionOfPlay != UP_MAP)
	{
		int temp;

		temp = xMax;
		xMax = xMin;
		xMax = xMax * -1;
		xMin = temp;
		xMin = xMin * -1;

		temp = yMax;
		yMax = yMin;
		yMax = yMax * -1;
		yMin = temp;
		yMin = yMin * -1;
	}

	if(X > xMax)
		return true;

	if(X < xMin)
		return true;

	if(Y > yMax)
		return true;

	if(Y < yMin)
		return true;

	return false;
}
//******************************************************************************************************
//this method sends a player to his default position
void	CMatch::GoToDefaultPosition(CTeamMember* thePlayer)
{
	int pos = thePlayer->getPosition();

	//theMaximums and minimums of they're position

	int xDest;
	int yDest;

	switch(pos)
	{
	case GOAL:
		xDest = 0;
		yDest = -522;
		break;

	case LDEF:
		xDest = -100;
		yDest = -422;
		break;

	case RDEF:
		xDest = 100;
		yDest = -422;
		break;

	case CMID:
		xDest = 0;
		yDest = -200;
		break;

	case LMID:
		xDest = -150;
		yDest = -200;
		break;

	case RMID:
		xDest = 150;
		yDest = -200;
		break;

	case CFWD:
		xDest = 0;
		yDest = 440;
		break;

	case RWNG:
		xDest = 200;
		yDest = 400;
		break;

	case LWNG:
		xDest = -200;
		yDest = 400;
		break;

	default:
		return;
		break;
	}

	if(thePlayer->DirectionOfPlay != UP_MAP)
	{
		xDest = -1 * xDest;
		yDest = -1 * yDest;
	}
	
	thePlayer->SetDestination(xDest, yDest);
}
//*************************************************************************************************
CActor*		CMatch::FindClosestTarget(CTeamMember* thePlayer)
{
	//bottom
	if(thePlayer->getYPos() < 0)
	{
		//left/
		if(thePlayer->getXPos() < -100)
		{
			if(thePlayer->getXPos() < -220)
				return (CActor*)LeftChute;
			else
			{
				return (CActor*)LeftDome;
			}
		}
		else // right
		{	
			if(thePlayer->getXPos() > 220)
				return (CActor*)RightChute;
			else
			{
				if(		(thePlayer->getXPos() > 50)
					&&	(thePlayer->DirectionOfPlay == DOWN_MAP) )
				{
					return (CActor*)RightBank;
				}
				else
				{
					if(thePlayer->DirectionOfPlay == DOWN_MAP) 
						return (CActor*)BottomShroom;
				}
			}
		}
	}
	else //top
	{
		
		{
			//left
			if(thePlayer->getXPos() < -100)
			{
				if(thePlayer->getXPos() < -220)
					return (CActor*)LeftChute;
				else
				{
					if((thePlayer->DirectionOfPlay == UP_MAP) )
						return (CActor*)LeftBank;
				}
			}	
			else //right
			{
				if(thePlayer->getXPos() > 220)
					return (CActor*)RightChute;
				else
				{
					if(thePlayer->getXPos() > 100)
					{
						if(thePlayer->DirectionOfPlay == UP_MAP)
							return (CActor*)RightDome;
					}
					else
					{
						if(thePlayer->DirectionOfPlay == UP_MAP)
							return (CActor*)TopShroom;
					}
				}
			}
		}
	}
	return NULL;
}
//****************************************************************************************
void	CMatch::ResetRandomNumbers()
{
	//Re-seeds the random number table at launch based on the system time
	SYSTEMTIME Time;

	GetSystemTime(&Time);

	int TimeInSecs = 0;
	//TimeInSecs += ((Time.wDay			* 60 * 60 * 24));
	//TimeInSecs += ((Time.wHour			* 60 * 60));
	//TimeInSecs += ((Time.wMinute		* 60));
	//TimeInSecs += ((Time.wSecond));
	TimeInSecs = ((Time.wSecond << 7) ^ ((Time.wMinute << 2) & 0xFF00)) + Time.wHour;
	srand(TimeInSecs);

	for (int i = 0; i < 300; i++)
	{
		RandomArray[i] = (rand() >> 7); // Values in the range 0->255. //thank you are ........
	}
	RandomArrayLoc = 0;
}

//****************************************************************************************
//***********************************************************************************
void	CMatch::RunToStartingPosition(CTeamMember* temp)
{
	//this will put everyone into positions... 
	
	if(temp->DirectionOfPlay == UP_MAP)
	{
		temp->setState(NONE, true);

		int pos = temp->getPosition();
		switch(pos)
		{
		case CFWD:
			temp->SetDestination(0, -48);
		
			break;
		case RWNG:
			temp->SetDestination(100, -5);
			
			break;
		case LWNG:
			temp->SetDestination(-250, -20);
		
			break;
		case CMID:
			temp->SetDestination(0, -192);
		
			break;
		case RMID:
			temp->SetDestination(150, -192);
			
			break;
		case LMID:
			temp->SetDestination(-150, -192);
		
			break;
		case RDEF:
			temp->SetDestination(150, -384);
			
			break;
		case LDEF:
			temp->SetDestination(-150, -384);
			break;
		case GOAL:
			temp->SetDestination(0, -540);
			break;
		}
	}
	else
	{

		temp->setState(NONE, true);

		int pos = temp->getPosition();
		switch(pos)
		{
		case CFWD:
			temp->SetDestination(0, 48);
		
			break;
		case RWNG:
			temp->SetDestination(-100, 15);
			
			break;
		case LWNG:
			temp->SetDestination(250, 20);
		
			break;
		case CMID:
			temp->SetDestination(0, 192);
		
			break;
		case RMID:
			temp->SetDestination(-150, 192);
			
			break;
		case LMID:
			temp->SetDestination(150, 192);
		
			break;
		case RDEF:
			temp->SetDestination(-150, 384);
			
			break;
		case LDEF:
			temp->SetDestination(150, 384);
			break;
		case GOAL:
			temp->SetDestination(0, 540);
			break;
		}
	}
}
//*************************************************************************************
bool	CMatch::CheckForGoalDoor(int Goal)
{
	for(int i = 0; i < MAX_TOKENS; i++)
	{
		if(TokenArray[i] == NULL)
			continue;

		if(Goal == 0)
			if(TokenArray[i]->GoalProtecting == 0)
				return true;


		if(Goal == 1)
			if(TokenArray[i]->GoalProtecting == 1)
				return true;		
	}

	return false;
}
//************************************************************************************
bool	CMatch::GamePause()
{
/*
	if(	ControllerRef->IsButtonPressedDB(Select) )
	{
		MatchState = PlayingGame;
		return true;
	}
*/
	DrawPauseScreen();

	return true;
}

// ----
void	CMatch::ConserveMemory(bool _Force)
{
	MEMORYSTATUS	StartMem;
	StartMem.dwLength = sizeof(StartMem);
	GlobalMemoryStatus(&StartMem);

	if (	(_Force) 
		||	(StartMem.dwAvailPhys < 1048576)) // Collect memory when less than one meg left.
	{
		// Get ride of the largest and least frequently used items first. 
		// Based on judgement - not fact!
		if (!SoundSystemRef->IsSamplePlaying(TheSirenSample))		{SAFELY_DELETE(TheSirenSample);}
		if (!SoundSystemRef->IsSamplePlaying(TheBallLaunchSample))	{SAFELY_DELETE(TheBallLaunchSample);}
		if (!SoundSystemRef->IsSamplePlaying(TheIcecreamSample))	{SAFELY_DELETE(TheIcecreamSample);}
		if (!SoundSystemRef->IsSamplePlaying(TheAllStarSample))		{SAFELY_DELETE(TheAllStarSample);}

		/*
		// Delete the ThePopUpGraphic object if it is not being used.
		switch (MatchState)
		{
		case StartingMatch:
		case LaunchingBall:
		case MatchOver:
		case DoingGoalSideA:
		case DoingGoalSideB:
		case DoingHomeGoalSideA:
		case DoingHomeGoalSideB:
		case DoingInjury:
			// Could be in use.
			break;

		default:
			// Not in use.
			SAFELY_DELETE(ThePopUpGraphic);
			break;
		}
		*/
	}

	if(StartMem.dwAvailPhys < 314572)
	{
		OutputDebugString(TEXT("Bit tight on memory"));
	}
}
