#ifndef		_ACTOR_TOKEN_CPP_
#define		_ACTOR_TOKEN_CPP_
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"actor.h"
#include	"Gamedefines.h"
//#include	"Actor_TeamMember.cpp"
#include	"Team.cpp"
#include	"Actor_Ball.cpp"

class CToken : public CActor
{
protected:

	int count;
	int TokenType;
public:
	bool IsActive;
	int	 GoalProtecting; //0 = bottom; 1 = top
	

public:
	CToken::CToken()
	{

		TokenType = GONE;
		IsActive = false;
		type = TokenType;
	}

	CToken::CToken(int theType, int theXpos, int theYpos)
	{
		count = 100;
		TokenType = theType;
		setxPos(theXpos);
		setyPos(theYpos);
		IsActive = true;
		type = TokenType;
		state = NONE;
		GoalProtecting = -1;
		WeaponStat = 0;
		Uses = 0;

		switch(theType)
		{
			case ATTACKWEAPON:
			case DEFENCEWEAPON:
			case SPEEDWEAPON:		
			case THROWWEAPON:		
			case POWERWEAPON:		
			case STAMINAWEAPON:	
			case IQWEAPON:		
			case AGGRWEAPON:
				WeaponStat = 250;
				Uses = 2;
				break;
			default:
				WeaponStat = 0;
				break;
		}
	}

	CToken::~CToken()
	{
	}

	// ----
	int	GetTokenType() {return TokenType;}

	void	CToken::OnPickUp(CTeam* theTeam, CTeam* oppTeam, int GameHalf, CBall* TheBall = NULL, bool IsCup = false)
	{
		int tempStat;
		int i = 0;

		if(theTeam->GetControlledTeamMember()->Weapon != NULL)
		{
			switch(TokenType)
			{
				case ATTACKWEAPON:
				case DEFENCEWEAPON:
				case SPEEDWEAPON:		
				case THROWWEAPON:		
				case POWERWEAPON:		
				case STAMINAWEAPON:	
				case IQWEAPON:		
				case AGGRWEAPON:
					theTeam->GetControlledTeamMember()->DropWeapon();
					break;
				default:
					break;
			}
		}

		switch(TokenType)
		{
			//aniFrame++; Unreachable
		
		case FREEZETEAM:
			
			for( i = 0; i < 9; i++)
			{
				oppTeam->GetTeamMember(i)->TokenEffect(FREEZETEAM, 100);
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case REDUCE:
			for( i = 0; i < 9; i++)
			{
				oppTeam->GetTeamMember(i)->TokenEffect(REDUCE, 100);
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case INCREASE:	
			for( i = 0; i < 9; i++)
			{
				theTeam->GetTeamMember(i)->TokenEffect(INCREASE, 100);	
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case MANIC:
			for( i = 0; i < 9; i++)
			{
				oppTeam->GetTeamMember(i)->TokenEffect(MANIC, 100);	
			}
			for( i = 0; i < 9; i++)
			{
				theTeam->GetTeamMember(i)->TokenEffect(MANIC, 100);	
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case SLOWDOWN:
			for( i = 0; i < 9; i++)
			{
				oppTeam->GetTeamMember(i)->TokenEffect(SLOWDOWN, 100);	
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case GRABBALL:

			TheBall->HoldOrLetGoOfBall();
			TheBall->HoldOrLetGoOfBall(theTeam->GetControlledTeamMember());
		
			aniFrame = 0;
			setState(GONE);
			break;
		case TRANSPORT:

			TheBall->HoldOrLetGoOfBall();
			TheBall->HoldOrLetGoOfBall(theTeam->GetTeamMember(CFWD));
		
			aniFrame = 0;
			setState(GONE);
			break;
		case GOALDOOR:
			aniFrame = 0;

			if(theTeam->GetTeamMember(1)->DirectionOfPlay == UP_MAP)
				GoalProtecting = 0;
			else
				GoalProtecting = 1;

			setState(PICKED_UP);
			break;
		case SHIELDTEAM:
			for( i = 0; i < 9; i++)
			{
				theTeam->GetTeamMember(i)->TokenEffect(SHIELDTEAM, 100);	
			}
			aniFrame = 0;
			setState(PICKED_UP);
			break;
		case FULLENERGY:
			theTeam->GetControlledTeamMember()->setCurrentStamina(theTeam->GetControlledTeamMember()->getSta());
			aniFrame = 0;
			setState(GONE);
			break;
		case ZAPTEAM:
			for(i = 0; i < 9; i++)
			{
				oppTeam->GetTeamMember(i)->FallOver(oppTeam->GetTeamMember(i)->GetDirection());				
			}
			aniFrame = 0;
			setState(GONE);
			break;
		case COIN:
			if(IsCup == false)
				theTeam->CreditAccount(100);
			else
				theTeam->CreditAccount(200);

			setState(GONE);
			break;
		case ATTACKWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getAtt();
			theTeam->GetControlledTeamMember()->setAtt(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case DEFENCEWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getDef();
			theTeam->GetControlledTeamMember()->setDef(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case SPEEDWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getSpd();
			theTeam->GetControlledTeamMember()->setSpd(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case THROWWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getThr();
			theTeam->GetControlledTeamMember()->setThr(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case POWERWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getPow();
			theTeam->GetControlledTeamMember()->setPow(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case STAMINAWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getSta();
			theTeam->GetControlledTeamMember()->setSta(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case IQWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getInt();
			theTeam->GetControlledTeamMember()->setInt(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		case AGGRWEAPON:
			tempStat = theTeam->GetControlledTeamMember()->getAgr();
			theTeam->GetControlledTeamMember()->setAgr(WeaponStat);
			WeaponStat = tempStat;
			theTeam->GetControlledTeamMember()->Weapon = this;
			setState(WEAPON_GOT);
			break;
		default:
			setState(GONE);
			break;
		}

		
		if(		(state == PICKED_UP)
			||	(state == WEAPON_GOT) )
		{
			setYPos(1000);
			if(GameHalf == FIRST_HALF)
			{
				if(theTeam->GetControlledTeamMember()->DirectionOfPlay == UP_MAP)
				{
					setXPos(-1000);
				}
				else
				{
					setXPos(1000);
				}
			}
			else
			{
				if(theTeam->GetControlledTeamMember()->DirectionOfPlay == UP_MAP)
				{
					setXPos(1000);
				}
				else
				{
					setXPos(-1000);
				}
			}
		}
	}

	void	CToken::Process()
	{
		
		if(TokenType == COIN)
		{
			aniFrame++;
			if(aniFrame > 11)
			{
				aniFrame = 0;
			}
		}

		if(getState() == PICKED_UP)
		{
			count--;
			if(count <= 0)
				setState(GONE);
		}
	}
};
#endif
