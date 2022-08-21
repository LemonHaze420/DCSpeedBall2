#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"Gamedefines.h"

class CExtra : public CActor
{
protected:

	int ExtraType;

	int xPos;
	int yPos;

	int DestX;
	int DestY;

	int MoveX;
	int MoveY;

	int MoveSpeed;

	int FacingDirection;
	int DirectionTravel;

	bool once;

public:
	bool There;

	bool Thinga;
	CExtra::CExtra()
	{
		xPos = 0;
		yPos = 0;

		DestX = 0;
		DestY = 0;

		MoveX = 4;
		MoveY = 4;

		MoveSpeed = 4;

		DirectionTravel = STILL;

		state = FLOORED;

		There = false;
		once = false;
	}

	CExtra::~CExtra()
	{
	}

	void	CExtra::Init(int xDestination,
						 int yDestination,
						 bool ThingA)
	{
		once = false;

		DestX = xDestination;
		DestY = yDestination;

		if(DestX >= 0)
		{
			if(ThingA)
			{
				setxPos(xDestination - 254);
				setyPos(yDestination);
			}
			else
			{
				setxPos(xDestination - 230);
				setyPos(yDestination);
			}

		}
		else
		{
			if(ThingA)
			{
				setxPos(xDestination + 230);
				setyPos(yDestination);
			}
			else
			{
				setxPos(xDestination + 254);
				setyPos(yDestination);
			}
		}

		if(ThingA)
			DestX -= 24;
		else
			DestX += 24;

		Thinga = ThingA;
	}

	void	CExtra::Move()
	{
	//yPos+=MoveY;
	//	xPos+=MoveX;

		if(		(DirectionTravel == NE)
			||	(DirectionTravel == E)
			||	(DirectionTravel == SE))
		{
			if(	(MoveX + getxPos()) > DestX)
			{
				setxPos(DestX);
			}
			else
			{
				setxPos(getxPos() + MoveX);
			}
		}
		else
		{
			if(		(DirectionTravel == NW)
				||	(DirectionTravel == W)
				||	(DirectionTravel == SW))
			{
				if( (MoveX + getxPos()) > DestX)
				{
					setxPos( getxPos() + MoveX );
				}
				else
				{
					setxPos(DestX);
				}		
			}
		}

		if(		(DirectionTravel == NW)
			||	(DirectionTravel == NE)
			||	(DirectionTravel == N))
		{
			if(	(MoveY + getyPos()) > DestY)
			{
				setyPos(DestY);
			}
			else
			{
				setyPos(getYPos() + MoveY);
			}
		}
		else
		{
			if(		(DirectionTravel == SW)
				||	(DirectionTravel == SE)
				||	(DirectionTravel == S))
			{
				if( (MoveY + getyPos()) > DestY)
				{
					setyPos(getYPos() + MoveY);
				}
				else
				{
					setyPos(DestY);
				}		
			}
		}
	}

	void	CExtra::SetDestination(int x, int y)
	{
		//if(DestX == getXPos())
			DestX = x;

		//if(DestY == getYPos())
			DestY = y;
	}

	int		CExtra::WorkOutDirection(int startX, int startY, int xDest, int yDest)
	{
	
		if(((startY + PLAYER_WIDTH) > yDest) && ((startY - PLAYER_WIDTH) < yDest))
		{
			if(startX < xDest)
			{
				state = RUNNING;
				return E;
			}

			if(startX > xDest)
			{
				state = RUNNING;
				return W;
			}
		}

		if(((startX + PLAYER_WIDTH) > xDest) && ((startX - PLAYER_WIDTH) < xDest))
		{
			if(startY < yDest)
			{
				state = RUNNING;
				return N;
			}

			if(startY > yDest)
			{
				state = RUNNING;
				return S;
			}
		}

		if(startY < yDest)
		{
			if(startX < xDest)
			{
				state = RUNNING;
				return NE;
			}

			if(startX > xDest)
			{
				state = RUNNING;
				return NW;
			}
		}

		if(startY > yDest)
		{
			if(startX < xDest)
			{
				state = RUNNING;
				return SE;
			}

			if(startX > xDest)
			{
				state = RUNNING;
				return SW;
			}
		}
		return STILL;
	}

	void	CExtra::SetXY(int X, int Y)
	{
		setxPos(X);
		setyPos(Y);
		DestX = (X);
		DestY = (Y);
	}

	void	CExtra::SetDirection(int Direction)
	{
		if( Direction != STILL)
		{
			FacingDirection = Direction;
		}
		DirectionTravel = Direction;
	}

	int		CExtra::GetDirectionFacing()
	{
		return FacingDirection;
	}

	bool	CExtra::Process(CExtra* thing)
	{
		//find out which direction they should be going
		int dir = WorkOutDirection(getxPos(), getyPos(), DestX, DestY);
		//Turn them to face that direction
		SetDirection(dir);

		aniFrame++;

		if(		(thing->There)
			&&	(There) )
		{
			GoOffScreen();
			setState(SLIDING);
		}

		switch(state)
		{
		case RUNNING:
			aniFrame = 0;
			if(		(DestX == getXPos())
				&&	(DestY = getYPos() ) )
			{
				if(Thinga)
					SetDirection(E);
				else
					SetDirection(W);
				There = true;
			}
			break;
		case SLIDING:
			break;
		default:
			return false;
		}

		SetMoveXAndMoveY(dir);
		//then move them
		Move();
		return true;
	}

	void	CExtra::GoOffScreen()
	{
		if( once == true)
			return;

		if(Thinga)
		{
			if((getxPos() + 24) >= 0)
			{
				DestX -= 5000;
				DestY = getYPos();
			}
			else
			{
				DestX += 5000;
				DestY = getYPos();
			}
		}
		else
		{
			if((getxPos() - 24) >= 0)
			{
				DestX -= 5000;
				DestY = getYPos();
			}
			else
			{
				DestX += 5000;
				DestY = getYPos();
			}
		}
		once = true;
	}

	void	CExtra::SetMoveXAndMoveY(int direction)
	{
		switch(direction)
		{
		case N:
			MoveX = 0;
			MoveY = MoveSpeed;
			break;
		case NE:
			MoveX = MoveSpeed;
			MoveY = MoveSpeed;
			break;
		case E:
			MoveX = MoveSpeed;
			MoveY = 0;
			break;
		case SE:
			MoveX = MoveSpeed;
			MoveY = -MoveSpeed;
			break;
		case S:
			MoveX = 0;
			MoveY = -MoveSpeed;
			break;
		case SW:
			MoveX = -MoveSpeed;
			MoveY = -MoveSpeed;
			break;
		case W:
			MoveX = -MoveSpeed;
			MoveY = 0;
			break;
		case NW:
			MoveX = -MoveSpeed;
			MoveY = MoveSpeed;
			break;
		default:
			MoveX = 0;
			MoveY = 0;
		}

		if(		(MoveX == 0)
			&&	(MoveY == 0))
		{
			setState(NONE);
		}
		else
		{
		}
		
	}
};
