#ifndef		_ACTOR_BALL_CPP
#define		_ACTOR_BALL_CPP
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"actor.h"
#include	"GameDefines.h"
#include	"Actor_TeamMember.cpp"


/***********************************************************
 *	This class provdes a basis for using Balls
 *	in the game
 */

class CBall : public CActor
{
protected:

	int			TeamHeldBy;
	int			Speed;
	bool		IsBeingHeld;		//if the ball is being held then we don't display with it,
								//it just travels with the player.
	
		

	//Throw varibles, to get passed in to getThrown after the animation has finished
	int			startX;
	int			startY; 
	int			Direction; 
	int			theSpeed; 
	int			MoveSpeed;
	bool		HiLo; 
	bool		human;
	bool		Object;
	int			ThrowCount;
	int			AfterTouchDirection;
	bool		HasBounced;
	
public:

	int				DestX; //final destination of the ball
	int				DestY; //final destination of the ball
	int				MoveX; //distance to move across the x axis each turn
	int				MoveY; //distance to move across the y axis each turn
	double		TimeInTeamOneHalf; //these should be doubles just in case the game is running at a quick
	double		TimeInTeamTwoHalf; //frame rate and the game turns go past the int limit

	int				WhichTeamToBurn; //if the ball is burning.. which team to zap;
	CTeamMember*	heldBy;			//either who is currently holding the ball  
									//
									//if the ball is not
	CTeamMember*	LastHeldBy;	//held then it shows who last had the ball

	int				AccelCount;	//If the ball has just been thrown it accelerates for a bit
								//this tells it how long to do it for	

	int ChuteFrame;				//This is for updating the height when the ball is in the chute;
	int	BurnCount;


	CBall::CBall()
	{
		Height		= 0;
		setxPos(CENTRE_PITCHX);
		setyPos(CENTRE_PITCHY);
		screenXPos	= 0;//CENTRE_SCREENX;
		screenYPos	= 0;//CENTRE_SCREENY;
		isVisible	= false;


		ChuteFrame = 3;

		TeamHeldBy  =	NEITHER;

		TimeInTeamOneHalf = 0;
		TimeInTeamTwoHalf = 0;
		BurnCount = 0;
		WhichTeamToBurn = NEITHER;
		Speed = 0;
		startX = 0;
		startY = 0; 
		Direction = N; 
		AfterTouchDirection = STILL;
	
		HiLo = false; 
		human = false;
		Object = false;
		ThrowCount = 0;


		IsBeingHeld = false;

		AccelCount = 0;
		theSpeed = 0; 
		MoveSpeed = 0;

		heldBy = NULL;
		LastHeldBy = NULL;

		DestX = 0;
		DestY = 0;
		MoveX = 0;
		MoveY = 0;
	
		AccelCount = 0;

		HasBounced = false;
	}

	CBall::~CBall()
	{
	}
//********************************************************************************************************
	void	CBall::DoBounce()
	{
		if( (getyPos() >= PITCH_TRY + 16) || (getyPos() <= PITCH_BLY - 16) )
		{
			switch(DirectionTravel)
			{
			case N:
				DirectionTravel = S;
/*
				if(AfterTouchDirection == E)
					AfterTouchDirection = W;
				else
					if(AfterTouchDirection == W)
						AfterTouchDirection = E;
						*/
				break;
			case NE:
				DirectionTravel = SE;
				break;
			case E:
				DirectionTravel = E;
				if(AfterTouchDirection == N)
					AfterTouchDirection = S;
				else
					if(AfterTouchDirection == S)
						AfterTouchDirection = N;
				break;
			case SE:
				DirectionTravel = NE;
				break;
			case S:
				DirectionTravel = N;
				/*
				if(AfterTouchDirection == E)
					AfterTouchDirection = W;
				else
					if(AfterTouchDirection == W)
						AfterTouchDirection = E;
						*/
				break;
			case SW:
				DirectionTravel = NW;
				break;
			case W:
				DirectionTravel = W;
				if(AfterTouchDirection == N)
					AfterTouchDirection = S;
				else
					if(AfterTouchDirection == S)
						AfterTouchDirection = N;
				break;
			case NW:
				DirectionTravel = SW;
				break;
			default:
				break;
			}
		}
		else
		{
			switch(DirectionTravel)
			{
			case N:
				DirectionTravel = N;
				if(AfterTouchDirection == E)
					AfterTouchDirection = W;
				else
					if(AfterTouchDirection == W)
						AfterTouchDirection = E;
				break;
			case NE:
				DirectionTravel = NW;
				break;
			case E:
				DirectionTravel = W;
/*
				if(AfterTouchDirection == N)
					AfterTouchDirection = S;
				else
					if(AfterTouchDirection == S)
						AfterTouchDirection = N;
						*/
				break;
			case SE:
				DirectionTravel = SW;
				break;
			case S:
				DirectionTravel = S;
				if(AfterTouchDirection == E)
					AfterTouchDirection = W;
				else
					if(AfterTouchDirection == W)
						AfterTouchDirection = E;
				break;
			case SW:
				DirectionTravel = SE;
				break;
			case W:
				DirectionTravel = E;
/*
				if(AfterTouchDirection == N)
					AfterTouchDirection = S;
				else
					if(AfterTouchDirection == S)
						AfterTouchDirection = N;
						*/
				break;
			case NW:
				DirectionTravel = NE;
				break;
			default:
				break;
			}
		}
	}
//********************************************************************************************************
	void	CBall::SetMoveXAndMoveY(int direction)
	{
		switch(direction)
		{
		case N:
			MoveX = 0;
			if(AfterTouchDirection != STILL)
			{
				if(AfterTouchDirection == E)
					MoveX += (MoveSpeed / 2);

				if(AfterTouchDirection == W)
					MoveX -= (MoveSpeed / 2);
			}
			MoveY = MoveSpeed;
			break;
		case NE:
			MoveX = MoveSpeed;
			MoveY = MoveSpeed;
			break;
		case E:
			MoveX = MoveSpeed;
			MoveY = 0;
			if(AfterTouchDirection != STILL)
			{
				if(AfterTouchDirection == N)
					MoveY += (MoveSpeed / 2);

				if(AfterTouchDirection == S)
					MoveY -= (MoveSpeed / 2);
			}
			break;
		case SE:
			MoveX = MoveSpeed;
			MoveY = -MoveSpeed;
			break;
		case S:
			MoveX = 0;
			if(AfterTouchDirection != STILL)
			{
				if(AfterTouchDirection == E)
					MoveX += (MoveSpeed / 2);

				if(AfterTouchDirection == W)
					MoveX -= (MoveSpeed / 2);
			}
			MoveY = -MoveSpeed;
			break;
		case SW:
			MoveX = -MoveSpeed;
			MoveY = -MoveSpeed;
			break;
		case W:
			MoveX = -MoveSpeed;
			MoveY = 0;
			if(AfterTouchDirection != STILL)
			{
				if(AfterTouchDirection == N)
					MoveY += (MoveSpeed / 2);

				if(AfterTouchDirection == S)
					MoveY -= (MoveSpeed / 2);
			}
			break;
		case NW:
			MoveX = -MoveSpeed;
			MoveY = MoveSpeed;
			break;
		default:
			MoveX = 0;
			MoveY = 0;
		}
		
	}
//********************************************************************************************************
	void	CBall::SetVisible(bool visible)
	{
		isVisible = visible;
	}
//********************************************************************************************************
	void	CBall::SetAfterTouch(int Dir)
	{
		AfterTouchDirection = Dir;
	}
//********************************************************************************************************
	void	CBall::GoTo(int X, int Y)
	{
		DestX = X;
		DestY = Y;
	}
//********************************************************************************************************
	void	CBall::SetXY(int X, int Y)
	{
		setXPos(X);
		setYPos(Y);
		//AfterTouchDirection = STILL;
	}
//********************************************************************************************************
//********************************************************************************************************
	//how many pixels to move each turn
	int		CBall::GetSpeed()
	{
		return Speed;
	}
//********************************************************************************************************
	void	CBall::SetSpeed(int theSpeed)
	{
		Speed = theSpeed;
	}
//*******************************************************************************************************
	bool	CBall::IsBallHeld()
	{
		return IsBeingHeld;
	}
//********************************************************************************************************
	void	CBall::HoldOrLetGoOfBall(CTeamMember* thePlayer = NULL)
	{
		if(thePlayer == NULL)
		{
			IsBeingHeld = false;
			if(heldBy != NULL)
			{
				heldBy->DropBall();
				LastHeldBy = heldBy;
			}

			heldBy = NULL;
			SetHeight(0);
			setState(LOW);
			//OutputDebugString(TEXT("put Down!!"));
		}
		else
		{
			HasBounced = false;
			IsBeingHeld = true;
			this->heldBy = thePlayer;
			heldBy->HasPossesion = true;
			heldBy->NotHoldBall = false;
			setState(HELD);
			heldBy->setState(NONE);
			SetHeight(thePlayer->GetHeight());
			AfterTouchDirection = STILL;
			if(Speed < 1)
			{
				heldBy->PickUpBall();
			}
			else
			{
				heldBy->PickUpBall(false);
			}
			//OutputDebugString(TEXT("Pick UP!!!"));
		}
	}
//********************************************************************************************************
	void	CBall::SetDirection(int direction)
	{
		DirectionTravel = direction;
	}
//********************************************************************************************************
	void	CBall::setState(DWORD theState)
	{
			state = theState;
	}
//********************************************************************************************************
	void	CBall::PrepareThrow(int theDirection, int AtheSpeed, bool AHiLo, bool Ahuman, bool AObject)
	{
		if(heldBy == NULL)
			return;
		if(AObject != true)
		{
			heldBy->SetDirection(theDirection);
			if(!heldBy->Throw(heldBy->GetDirectionFacing()))
				return;
		}
		Object		=	AObject;
		theSpeed	=	AtheSpeed;
		HiLo		=	AHiLo;
		human		=	Ahuman;
		ThrowCount	=	0;//THROWING_FRAMES - 6;
		AfterTouchDirection = STILL;
		setState(PREPARE_THROW);
	}
//********************************************************************************************************
	//Yes this function needs that many varibles
	void	CBall::GetThrown(int theSpeed, bool HiLo, bool Object, int Direction)
	{
		if(Object == false)
		{
			SetXY(heldBy->getXPos(), heldBy->getYPos());
			HoldOrLetGoOfBall();
		}
		else
		{
			SetXY( getxPos() , getyPos());
			HasBounced = true;
		}

		SetDirection(Direction);

		SetSpeed(theSpeed);
		if(HiLo)
		{
			setState(HIGH);
			AccelCount = 20;
			
		}
		else
		{
			setState(LOW);
			AccelCount = 10;
		}

	
	}
//********************************************************************************************************
	//team one half lets you know whos half the ball is in
	//isButtonPressed is for the ball to tell it wether to go hi or lo
	void	CBall::Process(int TeamOneHalf, bool isButtonPressed, int DirectionPressed)
	{
		DWORD currentAction = this->getState();
		int frame;
		frame = this->getAniFrame();

		if(getyPos() < 0)
		{
			if(TeamOneHalf == 0)
			{
				TimeInTeamOneHalf++;
			}
			else
			{
				TimeInTeamTwoHalf++;
			}
		}
		else
		{
			if(TeamOneHalf == 0)
			{
				TimeInTeamTwoHalf++;
			}
			else
			{
				TimeInTeamOneHalf++;
			}
		}

		//If the ball is being held we don't want to waste time processing this
		//Just draw the ball in the players hand gadammit

		if(IsBeingHeld == true)
		{
				setxPos(heldBy->getXPos());
				setyPos(heldBy->getYPos());
				Height = heldBy->GetHeight();
		}
		else
		{
			if(state != IN_CHUTE)
			{
				//*************
				//First Process Movement
				//*************
				int dir = GetDirection();
				//then work out how far they can move
				CalcMoveSpeed();
				//then set how far on the x and y they should move this turn;
				SetMoveXAndMoveY(dir);
				//then move them
				Move();
				
			}
		}
		//*************
		//Then state tags
		//************

		switch(currentAction)
		{
		case HELD:
			SetDirection(STILL);
			SetSpeed(0);
			AfterTouchDirection = STILL;
			break;
		case STOPPED:
			SetDirection(STILL);
			SetSpeed(0);
			SetHeight(0);
			BurnCount = 0;
			AfterTouchDirection = STILL;
			break;
		case ONFLOOR:
			SetSpeed(Speed - 4);
			if(Speed <= 0)
				setState(STOPPED);
			
			break;
		case LOW:
			AccelCount--;

			if(AccelCount > 7)
			{
				if(		(LastHeldBy != NULL)
					&&	(LastHeldBy->getTeam() == BRUTAL_DELUXE))
				{
					if(DirectionPressed != STILL)
					{
						AfterTouchDirection = DirectionPressed;
					}
				}
			}
			
			if(AccelCount == 6)
			{
				if(		(LastHeldBy != NULL)
					&&	(LastHeldBy->getTeam() == BRUTAL_DELUXE)
					&&	(HasBounced == false) )
				{
					if(isButtonPressed)
					{
						setState(HIGH);
						AccelCount = 17;
						break;
					}
				}
				SetHeight(1);
				isVisible = true;
				break;
			}

			if(AccelCount > 4)
			{
				SetHeight(1);
				isVisible = true;
				break;
			}

			if (	(AccelCount > 2) &&
					(AccelCount < 4))
			{
				SetHeight(1);
				break;
			}
			
			if(	(AccelCount < 2)
			&&	(AccelCount > 0))
			{
				SetHeight(1);
				break;
			}

			if(AccelCount < 0)
			{
				SetHeight(0);
				setState(ONFLOOR);
			}	
			break;
		case HIGH:
			AccelCount--;
			if(AccelCount > 18)
			{
				SetHeight(1);
				isVisible = true;
				break;
			}

			if (	(AccelCount > 3) &&
					(AccelCount < 18))
			{
				SetHeight(2);
				break;
			}
			
			if(	(AccelCount < 3)
			&&	(AccelCount > 0))
			{
				SetHeight(1);
				break;
			}

			if(AccelCount <= 0)
			{
				setState(ONFLOOR);
				//setState(STOPPED);
			}
			break;
		case LAUNCHING:
			AccelCount--;

			if(AccelCount >= 40)
			{	
				isVisible = false;
				SetHeight(0);
				break;
			}

			if(		(AccelCount > 35)
				&&	(AccelCount	< 40) )
			{
				SetHeight(1);
				isVisible = true;
				break;
			}

			if (	(AccelCount > 10) &&
					(AccelCount <= 35))
			{
				SetHeight(2);
				break;
			}
			
			if(	(AccelCount <= 10)
			&&	(AccelCount > 0))
			{
				SetHeight(1);
				break;
			}

			if(AccelCount < 0)
			{

				SetHeight(0);
				setState(STOPPED);
			}
			break;
		case PREPARE_THROW:
			ThrowCount--;
			if(ThrowCount < 0)
			{
				if(heldBy == NULL)
				{
					setState(LOW);
					break;
				}

				if(human == false)
				{
					if(heldBy != NULL)
					{
						//heldBy->DropBall();
						GetThrown(theSpeed, HiLo, Object, heldBy->GetDirectionFacing());
					}
				}
				else
				{/*
					if(isButtonPressed)
					{
						if(heldBy != NULL)
						{
							//heldBy->DropBall();
							GetThrown(theSpeed, true, Object, heldBy->GetDirectionFacing());
						}

					}
					else
					*/
					if(heldBy != NULL)
					{
						//heldBy->DropBall();
						GetThrown(theSpeed, false, Object, heldBy->GetDirectionFacing());
					}
				}
			}
			break;
		case IN_CHUTE:
			AccelCount--;
			if(AccelCount <= 0)
			{
				AccelCount = 0;
			}
			else
			{
				this->IsBeingHeld = false;
				this->theSpeed = 0;
				this->Speed = 0;
				AfterTouchDirection = STILL;
			}
			AfterTouchDirection = STILL;
		default:
			break;
			//not too sure as yet...
		}
	}
//********************************************************************************************************
	void	CBall::SetBallBurning(int TimesToBurn, int theTeamToBurn)
	{
		BurnCount = TimesToBurn;
		WhichTeamToBurn = theTeamToBurn;

	}
//********************************************************************************************************
	//will only be called on ball pickup
	bool	CBall::IsBallBurning()
	{
		if(BurnCount > 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
//********************************************************************************************************
	void	CBall::LaunchIt()
	{
		Height		= 0;
		setxPos(CENTRE_PITCHX);
		setyPos(CENTRE_PITCHY);
		DirectionTravel = STILL;
		
		
		TeamHeldBy  =	NEITHER;
		BurnCount = 0;
		WhichTeamToBurn = NEITHER;
		Speed = 0;


		AccelCount = 50;
		AfterTouchDirection = STILL;

		this->IsBeingHeld = false;
		this->theSpeed = 0;
		this->Speed = 0;
		
		if(heldBy != NULL)
		{
			heldBy->SetPossesion(false);
			heldBy->IsClosest = false;
			heldBy->IsControlled = false;
			this->heldBy = NULL;
		}
		

		state = LAUNCHING;
	}
//********************************************************************************************************
	void	CBall::Move()
	{
		setxPos(getXPos()+MoveX);
		setyPos(getYPos()+MoveY);
	}
//***************************************************************************************************
	void	CBall::CalcMoveSpeed()
	{
		int speed;
		speed = Speed;
		speed = (7.5 + ((speed - 100) / 10) /4); //10 was 15

		if(		(state != LAUNCHING)
			||	(state != STOPPED) )
		{
			MoveSpeed = speed;
		}
		else
		{
			Speed = 0;
			MoveSpeed = 0;
		}
		
	}


//*****************************************************************************************************
	void	CBall::doChute()
	{
		if(state != IN_CHUTE)
		{
			AccelCount = CHUTE_FRAMES - 10;
		}

		setState(IN_CHUTE);
	}
};
#endif