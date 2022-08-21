#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"GameDefines.h"
#include	"Actor_TeamMember.cpp"
#include	"Team.cpp"

/***********************************************************
 *	This class provdes a basis for using Stars
 *	in the game
 */

class CStar : public CActor
{
public:

	bool	OnOff;
	int		TeamControlling;

	bool	wasLitLastTurn;

	int		BlueTeamNumber;


	CStar::CStar(int YCent, int Blue)
	{
		OnOff = false;
		TeamControlling = NONE;
		setyPos(YCent);
		wasLitLastTurn = false;
		BlueTeamNumber = Blue;
	}

	CStar::~CStar()
	{
	}

	void CStar::GetHit(CTeamMember* thePlayer)
	{
		if(thePlayer->getTeam() == BlueTeamNumber)
		{
			if(OnOff == false)
			{
				OnOff = true;
				TeamControlling = BLUE;
			}
			else
			{
				////****
				if(TeamControlling != BLUE)
				{
					OnOff = false;
					TeamControlling = NONE;
				}
			}
		}
		else
		{
			if(OnOff == false)
			{
				OnOff = true;
				TeamControlling = RED;
			}
			else
			{
				if(TeamControlling == BLUE)
				{
					OnOff = false;
					TeamControlling = NONE;
				}
			}
			
		}
	}

	void CStar::Reset()
	{
		OnOff = false;
		wasLitLastTurn = false;
		TeamControlling = NONE;		
	}
		
};