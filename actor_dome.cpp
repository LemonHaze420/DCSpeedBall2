#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"GameDefines.h"
#include	"Actor_Ball.cpp"

#define	DOME_WIDTH	16

/***********************************************************
 *	This class provdes a basis for using Domes
 *	in the game
 */

class CDome : public CActor
{
protected:

	int DomeReboundStrength;
	int LitTimer;
public:
	CDome::CDome(int x, int y)
	{
		setxPos(x);
		setyPos(y);
	}

	CDome::~CDome()
	{
	}
		
	void	CDome::OnHit(CBall* theBall, int multiplier)
	{
		if(theBall->LastHeldBy->DirectionOfPlay == UP_MAP)
		{
			theBall->SetBallBurning(1 + ((multiplier - 1) * 2), DOWN_MAP);//these valuse shall change
		}
		else
		{
			theBall->SetBallBurning(1 + ((multiplier - 1) * 2), UP_MAP);
		}
		this->setState(LIT);
		LitTimer = 5;
		
		//is the ball in the left 3rd?
		if(theBall->getXPos() < (getxPos() - (DOME_WIDTH / 2)))
		{
			//is the ball in the bottom 3rd?
			if(theBall->getYpos() < (getyPos() - (DOME_WIDTH / 2)))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, SW );
				return;
			}

			//is it in the top 3rd
			if(theBall->getYpos() > ((getyPos() + DOME_WIDTH) - (DOME_WIDTH / 2)))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, NW);
				return;
			}

			//no then it must have hit in the middle
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, W);
			return;
		}

		//no.. so is it in the right 3rd?
		if(theBall->getXPos() > (getxPos() ))//+ DOME_WIDTH / 2) )
		{
			//is the ball in the bottom 3rd?
			if(theBall->getYpos() < (getyPos() - (DOME_WIDTH / 2)))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, SE );
				return;
			}

			//is it in the top 3rd
			if(theBall->getYpos() > ((getyPos() + DOME_WIDTH) - (DOME_WIDTH / 2)))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, NE);
				return;
			}

			//no then it must have hit in the middle
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, E);
			return;
		}

		//no so it must be in the middle
		//is the ball in the bottom 3rd?
		if(theBall->getYpos() < (getyPos() - (DOME_WIDTH / 2)))
		{
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, S);
			return;
		}

		//is it in the top 3rd
		if(theBall->getYpos() > (getyPos() - (DOME_WIDTH / 2)))
		{
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, N);
			return;
		}
	}

	void	CDome::Process()
	{
		switch(state)
		{
		case UNLIT:
			break;
		case LIT:
			LitTimer--;
			if(LitTimer == 0)
				this->setState(UNLIT);
			break;
		default:
			//ummmmmm not really relevant
			break;
		}
	}
};