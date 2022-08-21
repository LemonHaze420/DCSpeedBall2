#ifndef _ACTOR_TEAMMEMBER_CPP_
#define _ACTOR_TEAMMEMBER_CPP_
#include	"Actor.h"
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"GameDefines.h"
#include	"MyString.h"
//#include	"Actor_Token.cpp"

class CTeamMember : public CActor
{
protected:
	
	CString	Name;
	int Team;
	int Position;
	int Worth;
	
	int Agr;
	int Att;
	int Def;
	int Spd;
	int Thr;
	int Pow;
	int Sta;
	int Int;

	int			PictureNumber;
	
	int			CurrentStamina;
	bool		Frozen;
	int			DestX;
	int			DestY;

	
	//trying something here for speed
	float		tempSpeed;	
public:
	bool		IsControlled;
	bool		HasPossesion;
	bool		IsClosest;
	float		MoveX; //how far to go each turn on the X axis
	float		MoveY; //how far to go each turn on the Y axis;
	float		MoveSpeed; //increment on move each turn
	int			FacingDirection; //FACING
	int			DirectionOfPlay;
	bool		NotHoldBall; //can't catch the ball if you have just thrown it

	//this is so that if you are being stood on you can't stand up;
	bool		NotStand;

	//this is for current token effects
	int			CurrentTokenEffect;
	int			TokenEffectCount;
	bool		OneTime;

	CActor*		Weapon;

	
	CTeamMember::CTeamMember()
	{
		Team = BRUTAL_DELUXE;
		Worth = 0;
#ifdef BIG_CHEAT_BUILD
		Agr = 250;
		Att = 250;
		Def = 250;
		Spd = 250;
		Thr = 250;
		Pow = 250;
		Sta = 250;
		Int = 250;
		CurrentStamina = 250;
#else
		Agr = 100;
		Att = 100;
		Def = 100;
		Spd = 100;
		Thr = 100;
		Pow = 100;
		Sta = 100;
		Int = 100;
		CurrentStamina = 100;
#endif
		PictureNumber = 0;

		Frozen = false;
	
		HasPossesion = false;
		FacingDirection = N;
		IsControlled = false;
		NotHoldBall = false;

		CurrentTokenEffect = GONE;
		TokenEffectCount = 0;
		tempSpeed = 0;

		NotStand = false;
		CalcMoveSpeed();

		OneTime = false;
		Weapon = NULL;

		DestX	= 0;
		DestY	= 0;
		IsClosest = false;
		MoveX = 0;
		MoveY = 0;
		MoveSpeed = 0;
		DirectionOfPlay = DOWN_MAP;
	}

	CTeamMember::CTeamMember(int Pos)
	{
		this->Position = Pos;
		Team = BRUTAL_DELUXE;
		Worth = 0;
		Agr = 100;
		Att = 100;
		Def = 100;
		Spd = 100;
		Thr = 100;
		Pow = 100;
		Sta = 100;
		Int = 100;
		PictureNumber = Pos;
		CurrentStamina = 100;
		Frozen = false;
		
		HasPossesion = false;
		FacingDirection = N;
		IsControlled = false;
		NotHoldBall = false;

		NotStand = false;

		//asd
		CalcMoveSpeed();
		CurrentTokenEffect = GONE;
		TokenEffectCount = 0;
		OneTime = false;

		tempSpeed = 0;
		Weapon = NULL;

		DestX	= 0;
		DestY	= 0;
		IsClosest = false;
		MoveX = 0;
		MoveY = 0;
		MoveSpeed = 0;
		DirectionOfPlay = DOWN_MAP;
	}

	
	CTeamMember::~CTeamMember()
	{
	}
	
	CTeamMember::CTeamMember(	int ATeam,
								int APosition,
								int AAgr,
								int AAtt,
								int ADef,
								int ASpd,
								int AThr,
								int APow,
								int ASta,
								int AInt,
								int Diff = 0
								)
	{
		Diff = Diff * 15;
		Team = ATeam;
		Position = APosition;
		PictureNumber = APosition;
		Agr = AAgr + Diff;
		Att = AAtt + Diff;
		Def = ADef + Diff;
		Spd = ASpd + Diff;
		Thr = AThr + Diff;
		Pow = APow + Diff;
		Sta = ASta + Diff;
		Int = AInt + Diff;
		CurrentStamina = ASta;
		Frozen = false;
		
		HasPossesion = false;
		FacingDirection = S;
		IsControlled = false;
		CurrentTokenEffect = GONE;
		TokenEffectCount = 0;
		OneTime = false;

		tempSpeed = 0;
		Weapon = NULL;

		//asd
		CalcMoveSpeed();

		NotHoldBall = false;

		DestX	= 0;
		DestY	= 0;
		IsClosest = false;
		MoveX = 0;
		MoveY = 0;
		MoveSpeed = 0;
		DirectionOfPlay = DOWN_MAP;

	}
	
	//returns plaer name
	CString*	CTeamMember::getName()
	{
		return &Name;	
	}
	
	//Returns Players current worth
	int		CTeamMember::getWorth ()
	{
		return Worth;	
	}
	
	//Returns players position
	int		CTeamMember::getPosition ()
	{
		return Position;	
	}
	
	//Returns the team they are playing for. 0 or 1
	int		CTeamMember::getTeam ()
	{
		return Team;	
	}
	
	//returns Aggresssion rating
	int		CTeamMember::getAgr ()
	{
		return Agr;	
	}
	
	//Returns PLayers attack power
	int		CTeamMember::getAtt ()
	{
		return Att;	
	}
	
	//returns players defence strength
	int		CTeamMember::getDef ()
	{
		return Def;	
	}
	
	//Returns PLayers current speed
	int		CTeamMember::getSpd ()
	{
		return Spd;	
	}
	
	//Returns PLayers throw strength
	int		CTeamMember::getThr ()
	{
		return Thr;	
	}
	
	//Returns PLayers power
	int		CTeamMember::getPow ()
	{
		return Pow;	
	}
	
	//Returns PLayers stamina rating
	int		CTeamMember::getSta ()
	{
		return Sta;	
	}
	
	//Returns PLayers intelligence
	int		CTeamMember::getInt ()
	{
		return Int;	
	}
	
	//Returns the players current stamina
	int		CTeamMember::getCurrentStamina ()
	{
		return CurrentStamina;	
	}
	
	bool	CTeamMember::getFrozen()
	{
		return Frozen;
	}

	//returns FACING direction
	int		CTeamMember::GetDirectionFacing()
	{
		return FacingDirection;
	}
	//how many pixels to move each turn
	float	CTeamMember::GetMoveSpeed()
	{
		return MoveSpeed;	
	}

	bool	CTeamMember::GetPossesion()
	{
		return HasPossesion;
	}

	void	CTeamMember::SetPossesion(bool x)
	{
		HasPossesion = x;
	}
	
	/* Set all players stats */
	//*********************************************************************
	//sets plaer name
	void	CTeamMember::setName (CString* theName)
	{
		Name = theName;
	}
	
	//sets Players current worth
	void	CTeamMember::setWorth (int theWorth)
	{
		Worth = theWorth;	
	}
	
	void	CTeamMember::CalcValue()
	{
		// Calculate the player's worth
		Worth = 0;
		Worth += ((max(0, Int - 100) / 10) * INT_COST);
		Worth += ((max(0, Agr - 100) / 10) * AGR_COST);
		Worth += ((max(0, Att - 100) / 10) * ATT_COST);
		Worth += ((max(0, Thr - 100) / 10) * THR_COST);
		Worth += ((max(0, Pow - 100) / 10) * POW_COST);
		Worth += ((max(0, Def - 100) / 10) * DEF_COST);
		Worth += ((max(0, Sta - 100) / 10) * STA_COST);
		Worth += ((max(0, Spd - 100) / 10) * SPD_COST);
	}

	//sets players position
	void	CTeamMember::setPosition (int thePos)
	{
		Position = thePos;	
		
	}
	
	void	CTeamMember::setPicture(int thePict)
	{
		PictureNumber = thePict;
	}
		
	//sets the team they are playing for. 0 or 1
	void	CTeamMember::setTeam (int theTeam)
	{
		Team = theTeam;	
	}
	
	//sets Aggresssion rating
	void	CTeamMember::setAgr (int theAgr)
	{
		Agr = theAgr;	
	}
	
	//sets PLayers attack power
	void	CTeamMember::setAtt (int theAtt)
	{
		Att = theAtt;	
	}
	
	//sets players defence strength
	void	CTeamMember::setDef (int theDef)
	{
		Def = theDef;	
	}
	
	//sets Players current speed
	void	CTeamMember::setSpd (int theSpd)
	{
		Spd = theSpd;
		CalcMoveSpeed();
	}
	
	//sets PLayers throw strength
	void	CTeamMember::setThr (int theThr)
	{
		Thr = theThr;	
	}
	
	//sets PLayers power
	void	CTeamMember::setPow (int thePow)
	{
		Pow = thePow;	
	}
	
	//sets PLayers stamina rating
	void	CTeamMember::setSta (int theSta)
	{
		Sta = theSta;
		CurrentStamina = Sta;
	}
	
	//sets PLayers intelligence
	void	CTeamMember::setInt (int theInt)
	{
		Int = theInt;	
	}
	
	//sets the players current stamina
	void	CTeamMember::setCurrentStamina (int theCurrSta)
	{
		CurrentStamina = theCurrSta;	
	}
	//sets wether a player is currently frozen
	void	CTeamMember::setFrozen(bool isFrozen)
	{
		Frozen = isFrozen;
	}

	void	CTeamMember::SetDirection(int Direction)
	{
		if( Direction != STILL)
		{
			FacingDirection = Direction;
			DirectionTravel = Direction;
		}
		else
		{
			DirectionTravel = Direction;

			if(getPosition() == GOAL)
			{
				if(DirectionOfPlay == UP_MAP)
				{
					if(		(FacingDirection == S)
						||	(FacingDirection == SW)
						||	(FacingDirection == SE) 
						||	(FacingDirection == E)
						||	(FacingDirection == W) )
					{
						FacingDirection = N;
					}
				}
				else
				{
					if(		(FacingDirection == N)
						||	(FacingDirection == NW)
						||	(FacingDirection == NE) 
						||	(FacingDirection == E)
						||	(FacingDirection == W) )
					{
						FacingDirection = S;
					}	
				}
			}
		}
	}
	
	//***********************************************************************
	/* SETS AND GETS FINISHED */
	//***********************************************************************
	
	int		CTeamMember::GetAverageStats()
	{
		return ((Agr + Att + Def + Spd + Thr + Pow + Sta + Int) >> 3);
	}

	void	CTeamMember::CalcMoveSpeed()
	{
		//gonna try something
		MoveSpeed = getSpd();

		//Speed  = 4.5 + (( (Speed - 100) / 15) / 2 );
		MoveSpeed  = 4.5 + (( (MoveSpeed - 100) / 20) / 3 ); //was /2 at the end
	}

	//***********************************************************************
	/*PLAYER ACTIONS AND STUFF*/
	//***********************************************************************

	void	CTeamMember::setState(DWORD theState, bool forceInterupt = false)
	{
		if(		(this->state == NONE) 
			||	(this->state == RUNNING)
			||	(this->state == CATCHING)
			||	(forceInterupt == true))
		{
			aniFrame = 0;
			state = theState;
			NotHoldBall = false;
		}
	}

	void	CTeamMember::SetXY(int X, int Y)
	{
		setxPos(X);
		setyPos(Y);
		DestX = (X);
		DestY = (Y);
	}

	void	CTeamMember::TakeDamage(int damage)
	{
		CurrentStamina -= damage;
	}

	void	CTeamMember::Slide(int Direction)
	{
		
		SetDirection(Direction);
		this->setState(SLIDING);
	}

	void	CTeamMember::Jump(int Direction)
	{
		SetDirection(Direction);
		this->setState(JUMPING);
	}

	void	CTeamMember::FallOver(int Direction)
	{
		SetDirection(Direction);
		
		this->setState(FALLING, true);
		//DropBall();
		if(Weapon != NULL)
		{
			Weapon->setXPos(getXPos());
			Weapon->setYPos(getYPos());
			Weapon->setState(NONE);
			Weapon->Uses = (Weapon->Uses - 1);
			switch(Weapon->getType())
			{
				case ATTACKWEAPON:
					setAtt(Weapon->WeaponStat);
					break;
				case DEFENCEWEAPON:
					setDef(Weapon->WeaponStat);
					break;
				case SPEEDWEAPON:
					setSpd(Weapon->WeaponStat);
					break;
				case THROWWEAPON:
					setThr(Weapon->WeaponStat);
					break;
				case POWERWEAPON:
					setPow(Weapon->WeaponStat);
					break;
				case STAMINAWEAPON:
					setSta(Weapon->WeaponStat);
					break;
				case IQWEAPON:		
					setInt(Weapon->WeaponStat);
					break;
				case AGGRWEAPON:
					setAgr(Weapon->WeaponStat);
					break;
				default:
					break;
			}
			if(Weapon->Uses <= 0)
			{
				Weapon->setState(GONE);
			}

			Weapon = NULL;
		}
		this->SetHeight(0);
	}

	void	CTeamMember::GetHurt()
	{
		
		this->setState(FLOORED, true);
		this->SetHeight(0);
		//DropBall();
		DestX = getxPos();
		DestY = getyPos();
	}

	void	CTeamMember::Punch(int Direction)
	{
		SetDirection(Direction);
		this->setState(PUNCHING);
		DestX = getxPos();
		DestY = getyPos();
	}

	void	CTeamMember::Dance()
	{
		SetDirection(STILL);
		this->setState(DANCING, true);
		DestX = getxPos();
		DestY = getyPos();
	}

	void	CTeamMember::KneeSlide()
	{
		if(DirectionOfPlay == UP_MAP)
		{
			SetDirection(S);
			SetDestination(getxPos(), getyPos()-300);
		}
		else
		{
			SetDirection(N);
			SetDestination(getxPos(), getyPos()+300);
		}

		this->setState(KNEE_SLIDE, true);
	}

	bool	CTeamMember::Process()
	{
		//process any current token effects
		if(CurrentTokenEffect != GONE)
		{
			if(		(TokenEffectCount != 0)
				&&	(OneTime == false )	  )
			{
				switch(CurrentTokenEffect)
				{
				case FREEZETEAM:
					tempSpeed = Spd;
					Spd = -170;
					OneTime = true;
					CalcMoveSpeed();
					break;
				case REDUCE:
					Agr -= 50;
					Att -= 50;
					Def -= 50;
					Spd -= 50;
					Thr -= 50;
					Pow -= 50;
					Sta -= 50;
					Int -= 50;
					if(Weapon != NULL)
					{
						Weapon->WeaponStat -= 50;
					}
					OneTime = true;
					break;
				case INCREASE:
					Agr += 50;
					Att += 50;
					Def += 50;
					Spd += 50;
					Thr += 50;
					Pow += 50;
					Sta += 50;
					Int += 50;
					if(Weapon != NULL)
					{
						Weapon->WeaponStat += 50;
					}
					OneTime = true;
					break;
				case MANIC:
					Agr += 150;
					Att += 150;
					Def += 150;
					Spd += 150;
					Thr += 150;
					Pow += 150;
					Sta += 150;
					Int += 150;
					if(Weapon != NULL)
					{
						Weapon->WeaponStat += 150;
					}
					OneTime = true;
					break;
				case SLOWDOWN:
					Spd -= 50;
					if(Weapon != NULL)
					{
						if(Weapon->getType() == SPEEDWEAPON)
							Weapon->WeaponStat -= 50;
					}
					OneTime = true;
					break;
				case SHIELDTEAM:
					Def += 10000;
					if(Weapon != NULL)
					{
						if(Weapon->getType() == DEFENCEWEAPON)
							Weapon->WeaponStat += 10000;
					}
					OneTime = true;
					break;
				default:
					//OutputDebugString(TEXT("SOMETHINGS GONE WRONG!!!!!!!!!!!!!!!!!!!!!!!!"));
					CurrentTokenEffect = GONE;
					break;
				}
				TokenEffectCount--;
			}
			else
			{
				TokenEffectCount--;
				if(TokenEffectCount == 0)
				{
					switch(CurrentTokenEffect)
					{
					case FREEZETEAM:
						Spd = tempSpeed;
						CalcMoveSpeed();
						break;
					case REDUCE:
						Agr += 50;
						Att += 50;
						Def += 50;
						Spd += 50;
						Thr += 50;
						Pow += 50;
						Sta += 50;
						Int += 50;
						if(Weapon != NULL)
						{
							Weapon->WeaponStat += 50;
						}
						break;
					case INCREASE:
						Agr -= 50;
						Att -= 50;
						Def -= 50;
						Spd -= 50;
						Thr -= 50;
						Pow -= 50;
						Sta -= 50;
						Int -= 50;
						if(Weapon != NULL)
						{
							Weapon->WeaponStat -= 50;
						}
						break;
					case MANIC:
						Agr -= 150;
						Att -= 150;
						Def -= 150;
						Spd -= 150;
						Thr -= 150;
						Pow -= 150;
						Sta -= 150;
						Int -= 150;
						if(Weapon != NULL)
						{
							Weapon->WeaponStat -= 150;
						}
						break;
					case SLOWDOWN:
						Spd += 50;
						if(Weapon != NULL)
						{
							if(Weapon->getType() == SPEEDWEAPON)
								Weapon->WeaponStat += 50;
						}
						break;
					case SHIELDTEAM:
						Def -= 10000;
						if(Weapon != NULL)
						{
							if(Weapon->getType() == DEFENCEWEAPON)
								Weapon->WeaponStat -= 10000;
						}
						break;
					default:
					//	OutputDebugString(TEXT("SOMETHINGS GONE WRONG!!!!!!!!!!!!!!!!!!!!!!!!"));
						CurrentTokenEffect = GONE;
						break;
					}
					CurrentTokenEffect = GONE;
					TokenEffectCount = 0;
					OneTime = false;
				}
			}
		}

	//*/*/*/*/*/*/*/*/*/*/*/*/*/*/
		int dir = STILL;
		//work out which direction they should run in
		//as long as they can change direction
		if(		(state == RUNNING)
			||	(state == NONE)	)
		{
			dir = WorkOutDirection(getxPos(), getyPos(), DestX, DestY);
			//Turn them to face that direction
			SetDirection(dir);
		}
		else
		{
			dir = GetDirection();
			SetDestFromDirection(dir);
		}

		

		if((state == THROWING))
		{
			dir = STILL;
		}
		
		//then work out how far they can move
		//CalcMoveSpeed();
		//now process other states for them
		//firstly increment the animation frame by one;
		
		aniFrame++;

		//then find which state it is... if you are past the number of frames for that state
		//then reset the frames and, in some cases, reset the state
		switch(state)
		{
		case NONE:
		{
			aniFrame = 0;
			//NotHoldBall = false;
			break;
		}
		case RUNNING:
		{
			if(aniFrame >= RUNNING_FRAMES)
			{
				aniFrame = 0;
			}
			break;
		}
		case FLOORED:
		{
			if(		(aniFrame >= (17 - (Sta /20)) )
				&&	(CurrentStamina > 0) )
			{
				NotHoldBall = false;
				aniFrame = 0;
				MoveSpeed = 0;
				state = NONE;
			}
			else
			{
				NotHoldBall = true;
				//DropBall();
			}

			break;
		}
		case SLIDING:
		{
			if(aniFrame >= SLIDING_FRAMES)
			{
				NotHoldBall = false;
				aniFrame = 0;
				state = NONE;
			}

			break;
		}
		case JUMPING:
		{
			if(aniFrame <= 2)
			{
				SetHeight(1);
			}

			if( (aniFrame >= 1) && (aniFrame <= 7))
			{
				SetHeight(2);
			}

			if( (aniFrame == 7) ) 
			{
				SetHeight(1);
			}

			if(aniFrame > JUMPING_FRAMES)
			{
				SetHeight(0);
				aniFrame = 0;
				state = NONE;
			}

			break;
		}
		case PUNCHING:
		{
			if(aniFrame >= PUNCHING_FRAMES)
			{
				aniFrame = 0;
				state = NONE;
			}
			break;
		}
		case FALLING:
			if(aniFrame > FALLING_FRAMES)
			{
				aniFrame = 0;
				NotHoldBall = false;
				state = FLOORED;
			}
			break;
		case THROWING:
			if(aniFrame >= THROWING_FRAMES)
			{
				aniFrame = 0;
				NotHoldBall = false;
				state = NONE;
			}
			else
			{
				NotHoldBall = true;
			}
			break;
		case DANCING:
			if(aniFrame >= DANCING_FRAMES)
			{
				aniFrame = 0;
			}
			break;
		case KNEE_SLIDE:
			if(aniFrame >= KNEESLIDE_FRAMES)
			{
				aniFrame = 0;
				if(DirectionOfPlay == UP_MAP)
					SetDestination(0,-48);
				else
					SetDestination(0,48);
				state = NONE;
			}
			break;
		case DIVING:
			if(aniFrame >= DIVING_FRAMES)
			{
				NotHoldBall = false;
				aniFrame = 0;
				state = NONE;
			}
			break;
		case CATCHING:
			if(aniFrame > 4)
			{
				aniFrame = 0;
				state = NONE;
			}
			break;
		default:
			aniFrame = 0;
			state = NONE;
			break;
		}

		//then set how far on the x and y they should move this turn;
		SetMoveXAndMoveY(dir);
		//then move them
		Move();

		return true;
	}

	void	CTeamMember::SetMoveXAndMoveY(int direction)
	{
		
		
		this->CalcMoveSpeed();

		if(CurrentTokenEffect != FREEZETEAM)
		{
			if(this->HasPossesion == true)
			{
				MoveSpeed -= 0.5;
			}
			
			if(this->state == SLIDING)
			{
				MoveSpeed += 0.5;
			}

			if(state == FALLING)
			{
				MoveSpeed -= 1;
			}

			if(state == DIVING)
			{
				MoveSpeed += 2.5;//0.5;
			}

			if(state == FLOORED)
			{
				MoveSpeed = 0;	
			}
		}


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

	bool	CTeamMember::Throw(int direction)
	{
		if(		(HasPossesion == true)
			&&	(state != JUMPING) 
			&&	(state != SLIDING)   )
		{
			aniFrame = 0;
			this->SetDirection(direction);
			this->setState(THROWING);
			SetDestination(getxPos(), getyPos());
			return true;
		}
		return false;
	}

	void	CTeamMember::ProcessTokenEffects()
	{
	}

	int		CTeamMember::WorkOutDirection(int startX, int startY, int xDest, int yDest)
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

	void	CTeamMember::SetDestination(int x, int y)
	{
		//if(DestX == getXPos())
			DestX = x;

		//if(DestY == getYPos())
			DestY = y;

		if(getPosition() == GOAL)
		{
			if(DirectionOfPlay == UP_MAP)
			{
				if(DestY > -400)
				{
					DestY = -400;
				}
			}
			else
			{
				if(DestY < 400)
				{
					DestY = 400;
				}
			}
		}
	}

	void	CTeamMember::Move()
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
//**********************************************************************************************
	void	CTeamMember::DropBall()
	{
		NotHoldBall = true;
		SetPossesion(false);
	}

	void	CTeamMember::PickUpBall(bool still = true)
	{
		SetPossesion(true);
		NotHoldBall = false;

		if(		((MoveX == 0)
			&&	 (MoveY == 0))
			&&	(still == false))
		{
			setState(CATCHING);
		}
	}

	//goalie specific
	void	CTeamMember::Dive(int direction)
	{
		SetDirection(direction);
		this->setState(DIVING);
	}

	void	CTeamMember::TokenEffect(int whichToken, int time)
	{
		if(CurrentTokenEffect == GONE)
		{
			CurrentTokenEffect = whichToken;
			TokenEffectCount = time;
		}
	}

	// ----
	int		CTeamMember::GetPictureNumber()
	{
		return PictureNumber;
	}


	// ----
	void		CTeamMember::SetPictureNumber(int _PictureNumber)
	{
		PictureNumber = _PictureNumber;
	}

	void	CTeamMember::SetDestFromDirection(int Direction)
	{
		switch(Direction)
		{
		case N:
			SetDestination(getxPos(), getyPos() + 30);
			break;
		case NE:
			SetDestination(getxPos() + 30, getyPos() + 30);
			break;
		case E:
			SetDestination(getxPos() + 30, getyPos());
			break;
		case SE:
			SetDestination(getxPos() + 30, getyPos() - 30);
			break;
		case S:
			SetDestination(getxPos(), getyPos() - 30);
			break;
		case SW:
			SetDestination(getxPos() - 30, getyPos() - 30);
			break;
		case W:
			SetDestination(getxPos() - 30, getyPos());
			break;
		case NW:
			SetDestination(getxPos() - 30, getyPos() + 30);
			break;
		default:
			SetDestination(getxPos(), getyPos());
		}
	}

	void	CTeamMember::ReadyPlayer()
	{
		IsControlled = false;
		HasPossesion = false;
		IsClosest = false;
		MoveX = 0; //how far to go each turn on the X axis
		MoveY = 0; //how far to go each turn on the Y axis;

		NotHoldBall = false; //can't catch the ball if you have just thrown it

		NotStand = false;

		switch(CurrentTokenEffect)
		{
		case FREEZETEAM:
			Spd = tempSpeed;
			CalcMoveSpeed();
			break;
		case REDUCE:
			Agr += 50;
			Att += 50;
			Def += 50;
			Spd += 50;
			Thr += 50;
			Pow += 50;
			Sta += 50;
			Int += 50;
			if(Weapon != NULL)
			{
				Weapon->WeaponStat += 50;
			}
			break;
		case INCREASE:
			Agr -= 50;
			Att -= 50;
			Def -= 50;
			Spd -= 50;
			Thr -= 50;
			Pow -= 50;
			Sta -= 50;
			Int -= 50;
			if(Weapon != NULL)
			{
				Weapon->WeaponStat -= 50;
			}
			break;
		case MANIC:
			Agr -= 150;
			Att -= 150;
			Def -= 150;
			Spd -= 150;
			Thr -= 150;
			Pow -= 150;
			Sta -= 150;
			Int -= 150;
			if(Weapon != NULL)
			{
				Weapon->WeaponStat -= 150;
			}
			break;
		case SLOWDOWN:
			Spd += 50;
			if(Weapon != NULL)
			{
				if(Weapon->getType() == SPEEDWEAPON)
					Weapon->WeaponStat += 50;
			}
			break;
		case SHIELDTEAM:
			Def -= 10000;
			if(Weapon != NULL)
			{
				if(Weapon->getType() == DEFENCEWEAPON)
					Weapon->WeaponStat -= 10000;
			}
			break;
		default:
		//	OutputDebugString(TEXT("SOMETHINGS GONE WRONG!!!!!!!!!!!!!!!!!!!!!!!!"));
			CurrentTokenEffect = GONE;
			break;
		}
		CurrentTokenEffect = GONE;
		TokenEffectCount = 0;
		OneTime = false;

		if(Weapon != NULL)
		{
			switch(Weapon->getType())
			{
				case ATTACKWEAPON:
					setAtt(Weapon->WeaponStat);
					break;
				case DEFENCEWEAPON:
					setDef(Weapon->WeaponStat);
					break;
				case SPEEDWEAPON:
					setSpd(Weapon->WeaponStat);
					break;
				case THROWWEAPON:
					setThr(Weapon->WeaponStat);
					break;
				case POWERWEAPON:
					setPow(Weapon->WeaponStat);
					break;
				case STAMINAWEAPON:
					setSta(Weapon->WeaponStat);
					break;
				case IQWEAPON:		
					setInt(Weapon->WeaponStat);
					break;
				case AGGRWEAPON:
					setAgr(Weapon->WeaponStat);
					break;
				default:
					break;
			}

			if(Weapon != NULL)
			{
				Weapon->~CActor();
				Weapon = NULL;
			}
		}
		//Code here to reverse token effects
	}

	void	CTeamMember::DropWeapon()
	{
		if(Weapon != NULL)
		{
			Weapon->setXPos(getXPos());
			Weapon->setYPos(getYPos());
			Weapon->setState(NONE);
			Weapon->Uses = (Weapon->Uses - 1);
			switch(Weapon->getType())
			{
				case ATTACKWEAPON:
					setAtt(Weapon->WeaponStat);
					break;
				case DEFENCEWEAPON:
					setDef(Weapon->WeaponStat);
					break;
				case SPEEDWEAPON:
					setSpd(Weapon->WeaponStat);
					break;
				case THROWWEAPON:
					setThr(Weapon->WeaponStat);
					break;
				case POWERWEAPON:
					setPow(Weapon->WeaponStat);
					break;
				case STAMINAWEAPON:
					setSta(Weapon->WeaponStat);
					break;
				case IQWEAPON:		
					setInt(Weapon->WeaponStat);
					break;
				case AGGRWEAPON:
					setAgr(Weapon->WeaponStat);
					break;
				default:
					break;
			}
			if(Weapon->Uses <= 0)
			{
				Weapon->setState(GONE);
			}

			Weapon = NULL;
		}
	}
};
#endif
