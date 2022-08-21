#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"GameDefines.h"
#include	"Actor_Ball.cpp"
#include	"Team.cpp"

/***********************************************************
 *	This class provdes a basis for using shrooms
 *	in the game
 */

class CShroom : public CActor
{
protected:

	int ShroomPoints;
	int ShroomReboundStrength;
	int LitTimer;


public:
	CShroom::CShroom(int thexPos, int theypos)
	{
		ShroomPoints = 2;
		ShroomReboundStrength = 150;
		setxPos(thexPos);
		setyPos(theypos);
		LitTimer = 0;
	}

	CShroom::~CShroom()
	{
	}
		
//this is done in a very odd way so bear with me

	void	CShroom::OnHit(CBall* theBall, CTeam* theTeam, float multiplier)
	{
		//firstly add the points to the teams score
		theTeam->IncreaseBonusPoints(2 * multiplier);
		theTeam->IncreaseScore(2 * multiplier);
		
		this->setState(LIT);
		LitTimer = 2;
		
		//is the ball in the left 3rd?
		if(theBall->getXPos() < (getxPos()) - (SHROOM_WIDTH / 1.5))
		{
			//is the ball in the bottom 3rd?
			if(theBall->getYpos() < (getyPos()) - (SHROOM_WIDTH / 1.5))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, SW);
				return;
			}

			//is it in the top 3rd
			if(theBall->getYpos() > (getyPos()) - (SHROOM_WIDTH / 1.5))
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
		if(theBall->getXPos() > (getxPos()) + (SHROOM_WIDTH / 1.5))
		{
			//is the ball in the bottom 3rd?
			if(theBall->getYpos() < (getyPos()) - (SHROOM_WIDTH / 1.5))
			{
				//so reflect it in that direction
				theBall->GetThrown(150, false, true, SE);
				return;
			}

			//is it in the top 3rd
			if(theBall->getYpos() > (getyPos()) - (SHROOM_WIDTH / 1.5))
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
		if(theBall->getYpos() < (getyPos()) - (SHROOM_WIDTH / 1.5))
		{
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, S);
			return;
		}

		//is it in the top 3rd
		if(theBall->getYpos() > (getyPos())- (SHROOM_WIDTH / 1.5))
		{
			//so reflect it in that direction
			theBall->GetThrown(150, false, true, N);
			return;
		}

		//no then it must have hit in the middle
		//so reflect it in that direction
		theBall->GetThrown(150, false, true, UP);
		return;
	}

	void	CShroom::Process()
	{
		switch(state)
		{
		case UNLIT:
			break;
		case LIT:
			LitTimer--;
			if(LitTimer <= 0)
				this->setState(UNLIT);
			break;
		default:
			//ummmmmm not really relevant
			break;
		}
	}
};