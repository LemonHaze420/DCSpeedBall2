#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"Actor_Star.cpp"

/***********************************************************
 *	This class provdes a basis for using StarBanks
 *	in the game
 ***********************************************************/

class CStarBank : public CObject
{
public:
//protected:
	int count;

	bool left;
	CStar* TheStarBank[5];
	float MultiA;
	float MultiB;

	int BlueTeamNumber;

public:
	CStarBank::CStarBank(bool Left, int Blue)
	{
		if(Left)
		{
			for( int i = 0; i < 5; i++)
			{
				CStar* temp = new CStar(LEFT_STARBANK_SY + 16 + (i * 32), Blue);
				TheStarBank[i] = temp;
			}
		}
		else
		{
			for( int i = 0; i < 5; i++)
			{
				CStar* temp = new CStar(RIGHT_STARBANK_SY - 16 - (i * 32), Blue);
				TheStarBank[i] = temp;
			}
		}

		BlueTeamNumber = Blue;
		left = Left;
		count = 0;
	}

	CStarBank::~CStarBank()
	{
		for(int i = 0; i < 5; i++)
		{
			delete(TheStarBank[i]);
			TheStarBank[i] = NULL;
		}
	}

	void CStarBank::Process(CTeam* theTeam, CTeam* oppTeam, float TeamOneMulti, float TeamTwoMulti)
	{
		MultiA = TeamOneMulti;
		MultiB = TeamTwoMulti;

		bool lit = true;
		for(int i = 0; i < 5; i++)
		{
			if(	(TheStarBank[i]->OnOff == false))
			{
				if(TheStarBank[i]->wasLitLastTurn == true)
				{
					if(TheStarBank[0]->TeamControlling == BLUE)
					{
						theTeam->IncreaseBonusPoints(-2 * MultiA);
						theTeam->IncreaseScore(-2 * MultiA);
					}
					else
					{
						oppTeam->IncreaseBonusPoints(-2 * MultiB);
						oppTeam->IncreaseScore(-2 * MultiB);
					}
					TheStarBank[i]->wasLitLastTurn = false;
				}

				lit = false;
			}
			else
			{
				if(TheStarBank[i]->wasLitLastTurn == false)
				{
					TheStarBank[i]->wasLitLastTurn = true;
					if(TheStarBank[0]->TeamControlling == BLUE)
					{
						theTeam->IncreaseBonusPoints(2 * MultiA);
						theTeam->IncreaseScore(2 * MultiA);
					}
					else
					{
						oppTeam->IncreaseBonusPoints(2 * MultiB);
						oppTeam->IncreaseScore(2 * MultiB);
					}
				}
			}
		}

		if(lit == true)
		{
			count++;
			if(count > 5)
			{
				if(TheStarBank[0]->TeamControlling == BLUE)
				{
					theTeam->IncreaseBonusPoints(10 * MultiA);
					theTeam->IncreaseScore(10 * MultiA);
				}
				else
				{
					oppTeam->IncreaseBonusPoints(10 * MultiB);
					oppTeam->IncreaseScore(10 * MultiB);
				}

				for( i =0 ; i < 5; i++)
				{
					TheStarBank[i]->Reset();
				}
				count  = 0;
			}
		}
	}

	bool CStarBank::GetHit(CTeamMember* thePlayer, int yPos)
	{

		if(yPos < 0)
		{
			yPos *= -1;
		}

		if(yPos < LEFT_STARBANK_SY + 32)
		{
			if(left)
			{
				if(		(thePlayer->DirectionOfPlay == DOWN_MAP)
					&&	(TheStarBank[0]->OnOff == false) )
				{
					return false;
				}
				else
				{
					TheStarBank[0]->GetHit(thePlayer);
				}
			}
			else
			{
				if(		(thePlayer->DirectionOfPlay == UP_MAP)
					&&	(TheStarBank[0]->OnOff == false) )
				{
					return false;
				}
				else
				{
					TheStarBank[0]->GetHit(thePlayer);
				}
			}
				
		}
		else
		{
			if(yPos < LEFT_STARBANK_SY + (32 * 2))
			{
				if(left)
				{
					if(		(thePlayer->DirectionOfPlay == DOWN_MAP)
						&&	(TheStarBank[1]->OnOff == false) )
					{
						return false;
					}
					else
					{
						TheStarBank[1]->GetHit(thePlayer);
					}
				}
				else
				{
					if(		(thePlayer->DirectionOfPlay == UP_MAP)
						&&	(TheStarBank[1]->OnOff == false) )
					{
						return false;
					}
					else
					{
						TheStarBank[1]->GetHit(thePlayer);
					}
				}
			}
			else
			{
				if(yPos < LEFT_STARBANK_SY + (32 * 3))
				{
					if(left)
					{
						if(		(thePlayer->DirectionOfPlay == DOWN_MAP)
							&&	(TheStarBank[2]->OnOff == false) )
						{
							return false;
						}
						else
						{
							TheStarBank[2]->GetHit(thePlayer);
						}
					}
					else
					{
						if(		(thePlayer->DirectionOfPlay == UP_MAP)
							&&	(TheStarBank[2]->OnOff == false) )
						{
							return false;
						}
						else
						{
							TheStarBank[2]->GetHit(thePlayer);
						}
					}
				}
				else
				{
					if(yPos < LEFT_STARBANK_SY + (32 * 4))
					{
						if(left)
						{
							if(		(thePlayer->DirectionOfPlay == DOWN_MAP)
								&&	(TheStarBank[3]->OnOff == false) )
							{
								return false;
							}
							else
							{
								TheStarBank[3]->GetHit(thePlayer);
							}
						}
						else
						{
							if(		(thePlayer->DirectionOfPlay == UP_MAP)
								&&	(TheStarBank[3]->OnOff == false) )
							{
								return false;
							}
							else
							{
								TheStarBank[3]->GetHit(thePlayer);
							}
						}
					}
					else
					{
						if(left)
						{
							if(		(thePlayer->DirectionOfPlay == DOWN_MAP)
								&&	(TheStarBank[4]->OnOff == false) )
							{
								return false;
							}
							else
							{
								TheStarBank[4]->GetHit(thePlayer);
							}
						}
						else
						{
							if(		(thePlayer->DirectionOfPlay == UP_MAP)
								&&	(TheStarBank[4]->OnOff == false) )
							{
								return false;
							}
							else
							{
								TheStarBank[4]->GetHit(thePlayer);
							}
						}
					}
				}
			}
		}
		return true;
	}

	void	CStarBank::Reset()
	{
		for(int i = 0; i < 5; i++)
		{
			TheStarBank[i]->Reset();
		}
	}
		
};