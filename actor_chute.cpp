#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"GameDefines.h"
#include	"Actor_ball.cpp"

/***********************************************************
 *	This class provdes a basis for using Chutes
 *	in the game
 ***********************************************************/

class CChute : public CActor
{
protected:


	int	TeamA;
	int TeamB;
	float TeamAMulti;
	float TeamBMulti;
	int ballTravelSpeed;
	int LeftUp[49][2];
	int Height[49];
	int GetDirection;

public:
	CChute::CChute(int theXPos, int theYPos, int A, int B)
	{
		setxPos(theXPos);
		setyPos(theYPos);
		
		TeamA = A;
		TeamB = B;
		TeamAMulti = 1.0f;
		TeamBMulti = 1.0f;
		aniFrame = 0;

		LeftUp[0][0] = PITCH_BRX - 56;
		LeftUp[0][1] = 0;
		Height[0]	 = 4;
		LeftUp[1][0] = PITCH_BRX - 54;	
		LeftUp[1][1] = 4;
		Height[1]	 = 4;
		LeftUp[2][0] = PITCH_BRX - 52;
		LeftUp[2][1] = 8;
		Height[2]	 = 4;
		LeftUp[3][0] = PITCH_BRX - 50;
		LeftUp[3][1] = 12;
		Height[3]	 = 4;
		LeftUp[4][0] = PITCH_BRX - 48;
		LeftUp[4][1] = 16;
		Height[4]	 = 5;
		LeftUp[5][0] = PITCH_BRX - 48; 
		LeftUp[5][1] = 20;
		Height[5]	 = 5;
		LeftUp[6][0] = PITCH_BRX - 48;
		LeftUp[6][1] = 24;
		Height[6]	 = 5;
		LeftUp[7][0] = PITCH_BRX - 49;
		LeftUp[7][1] = 28;
		Height[7]	 = 4;
		LeftUp[8][0] = PITCH_BRX - 50;
		LeftUp[8][1] = 32;
		Height[8]	 = 4;
		LeftUp[9][0] = PITCH_BRX - 51;
		LeftUp[9][1] = 36;
		Height[9]	 = 5;
		LeftUp[10][0] = PITCH_BRX - 51;
		LeftUp[10][1] = 40;
		Height[10]	 = 5;
		LeftUp[11][0] = PITCH_BRX - 49;
		LeftUp[11][1] = 44;
		Height[11]	 = 5;
		LeftUp[12][0] = PITCH_BRX - 46;
		LeftUp[12][1] = 49;
		Height[12]	 = 5;
		LeftUp[13][0] = PITCH_BRX - 42;
		LeftUp[13][1] = 52;
		Height[13]	 = 6;
		LeftUp[14][0] = PITCH_BRX - 38;
		LeftUp[14][1] = 53;
		Height[14]	 = 6;
		LeftUp[15][0] = PITCH_BRX - 34;
		LeftUp[15][1] = 53;
		Height[15]	 = 6;
		LeftUp[16][0] = PITCH_BRX - 28;
		LeftUp[16][1] = 51;
		Height[16]	 = 6;
		LeftUp[17][0] = PITCH_BRX - 24;
		LeftUp[17][1] = 48;
		Height[17]	 = 6;
		LeftUp[18][0] = PITCH_BRX - 21;
		LeftUp[18][1] = 44;
		Height[18]	 = 6;
		LeftUp[19][0] = PITCH_BRX - 19;
		LeftUp[19][1] = 40;
		Height[19]	 = 7;
		LeftUp[20][0] = PITCH_BRX - 16;
		LeftUp[20][1] = 36;
		Height[20]	 = 7;
		LeftUp[21][0] = PITCH_BRX - 16;
		LeftUp[21][1] = 28;
		Height[21]	 = 7;
		LeftUp[22][0] = PITCH_BRX - 16;
		LeftUp[22][1] = 24;
		Height[22]	 = 7;
		LeftUp[23][0] = PITCH_BRX - 17;
		LeftUp[23][1] = 23;
		Height[23]	 = 7;
		LeftUp[24][0] = PITCH_BRX - 19;
		LeftUp[24][1] = 21;
		Height[24]	 = 6;
		LeftUp[25][0] = PITCH_BRX - 20;
		LeftUp[25][1] = 17;
		Height[25]	 = 6;
		LeftUp[26][0] = PITCH_BRX - 22;
		LeftUp[26][1] = 14;
		Height[26]	 = 6;
		LeftUp[27][0] = PITCH_BRX - 24;
		LeftUp[27][1] = 11;
		Height[27]	 = 6;
		LeftUp[28][0] = PITCH_BRX - 26;
		LeftUp[28][1] = 7;
		Height[28]	 = 6;
		LeftUp[29][0] = PITCH_BRX - 30;
		LeftUp[29][1] = 6;
		Height[29]	 = 5;
		LeftUp[30][0] = PITCH_BRX - 34;
		LeftUp[30][1] = 5;
		Height[30]	 = 5;
		LeftUp[31][0] = PITCH_BRX - 38;
		LeftUp[31][1] = 4;
		Height[31]	 = 4;
		LeftUp[32][0] = PITCH_BRX - 42;
		LeftUp[32][1] = 4;
		Height[32]	 = 4;
		LeftUp[33][0] = PITCH_BRX - 46;
		LeftUp[33][1] = 4;
		Height[33]	 = 4;
		LeftUp[34][0] = PITCH_BRX - 50;
		LeftUp[34][1] = 5;
		Height[34]	 = 4;
		LeftUp[35][0] = PITCH_BRX - 52;
		LeftUp[35][1] = 6;
		Height[35]	 = 4;
		LeftUp[36][0] = PITCH_BRX - 50;
		LeftUp[36][1] = 8;
		Height[36]	 = 5;
		LeftUp[37][0] = PITCH_BRX - 50;
		LeftUp[37][1] = 11;
		Height[37]	 = 5;
		LeftUp[38][0] = PITCH_BRX - 52;
		LeftUp[38][1] = 15;
		Height[38]	 = 5;
		LeftUp[39][0] = PITCH_BRX - 54;
		LeftUp[39][1] = 19;
		Height[39]	 = 5;
		LeftUp[40][0] = PITCH_BRX - 54;
		LeftUp[40][1] = 23;
		Height[40]	 = 5;
		LeftUp[41][0] = PITCH_BRX - 54;
		LeftUp[41][1] = 27;
		Height[41]	 = 4;
		LeftUp[42][0] = PITCH_BRX - 55;
		LeftUp[42][1] = 31;
		Height[42]	 = 4;
		LeftUp[43][0] = PITCH_BRX - 57;
		LeftUp[43][1] = 35;
		Height[43]	 = 4;
		LeftUp[44][0] = PITCH_BRX - 59;
		LeftUp[44][1] = 39;
		Height[44]	 = 4;
		LeftUp[45][0] = PITCH_BRX - 59;
		LeftUp[45][1] = 43;
		Height[45]	 = 4;
		LeftUp[46][0] = PITCH_BRX - 59;
		LeftUp[46][1] = 47;
		Height[46]	 = 4;
		LeftUp[47][0] = PITCH_BRX - 59;
		LeftUp[47][1] = 51;
		Height[47]	 = 4;
		LeftUp[48][0] = PITCH_BRX - 59;
		LeftUp[48][1] = 55;
		Height[48]	 = 4;
	}

	CChute::~CChute()
	{
	}
	
	int	CChute::GetFlag()
	{
		if	(	(TeamAMulti > 1.4f)
			&&	(TeamAMulti < 1.6f))
			return 1;

		if	(	(TeamAMulti > 1.9f)
			&&	(TeamAMulti < 2.1f))
			return 2;

		if	(	(TeamBMulti > 1.4f)
			&&	(TeamBMulti < 1.6f))
			return 3;

		if	(	(TeamBMulti > 1.9f)
			&&	(TeamBMulti < 2.1f))
			return 4;

		return 0;
	}

	void CChute::Process(CBall* theBall)
	{
		if(	this->getState() == IN_USE)
		{
			if(aniFrame == 0)
			{
				GetDirection = theBall->GetDirection();
			}
			aniFrame++;
		
			if(aniFrame < (CHUTE_FRAMES - 1))
			{
				
				if( getxPos() > 0)
				{
					if(GetDirection == N)
					{
						theBall->SetXY(LeftUp[aniFrame][0] + 56, LeftUp[aniFrame][1]);
						theBall->ChuteFrame = Height[aniFrame];
					}
					else
					{
						if(GetDirection == S)
						{
							theBall->SetXY(LeftUp[49 - aniFrame][0] + 56, LeftUp[49 - aniFrame][1]);
							theBall->ChuteFrame = Height[49 - aniFrame];
						}
					}
				}
				else
				{
					if(GetDirection == N)
					{
						theBall->SetXY(PITCH_BLX -	(LeftUp[49 - aniFrame][0] + 56 - PITCH_BRX),( -1 * LeftUp[49 - aniFrame][1]) - 8);
					}
					else
					{
						if(GetDirection == S)
						{
							theBall->SetXY(	PITCH_BLX -	(LeftUp[aniFrame][0] + 56 - PITCH_BRX),( -1 * LeftUp[aniFrame][1]) - 8);
							theBall->ChuteFrame = Height[49 - aniFrame];
						}
							
					}
				}
			}
				
			else
			{
				if(aniFrame == (CHUTE_FRAMES -1))
				{
					if(GetDirection == N)
					{
						theBall->SetSpeed(ballTravelSpeed + 75);
						theBall->setState(ONFLOOR);
					}
					else
					{
						theBall->SetSpeed(ballTravelSpeed + 75);
						theBall->setState(ONFLOOR);
					}
				}

				if(aniFrame >= (CHUTE_FRAMES))
				{
					state = NONE;
					aniFrame = 0;
				}
			}
			
		}
		else
		{
			aniFrame = 0;
			//do nothing
		}
	}

	void CChute::DoChute(CBall* theBall, int half)
	{
		if(state == IN_USE)
			return;

		state = IN_USE;
		ballTravelSpeed = theBall->GetSpeed();

		if(theBall->LastHeldBy->getTeam() == TeamA)
		{
			if(TeamBMulti <= 1.1f)
			{
				TeamAMulti += 0.5f;
			}
			else
			{
				TeamBMulti -= 0.5f;
			}
		}
		else
		{
			if(TeamAMulti <= 1.1f)
			{
				TeamBMulti += 0.5f;
			}
			else
			{
				TeamAMulti -= 0.5f;
			}
		}

		if(TeamAMulti > 2)
			TeamAMulti = 2;

		if(TeamBMulti > 2)
			TeamBMulti = 2;
	}

	float	CChute::GetTeamAMulti()
	{
		return TeamAMulti;
	}

	float   CChute::GetTeamBMulti()
	{
		return TeamBMulti;
	}

};