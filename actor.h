#ifndef _ACTOR_H_
#define _ACTOR_H_

#include	"Object.h"
//#include	"common.h"
#include	"GameDefines.h"

#include	"SafeProgramming.h"
/****************************************
 * Actor is a base class used for all things in game.
 * It records thier state, wether they are on or off screen.
 * The frame of animation they are in and thier position.
 ****************************************/

class CActor : public CObject
{
public:
	CActor::CActor()
	{
		type			= OTHER;
		state			= NONE;
		ControlState	= NULL;
		xPos			= 0;
		yPos			= 0;
		screenXPos		= 0;
		screenYPos		= 0;
		isVisible		= true;
		aniFrame		= 0;
		DirectionTravel	= STILL;
		Height			= 0;

		WeaponStat		= 0;
		Uses			= 0;;
	};


	// ----
	CActor::~CActor()
	{

	}


	// ----
	CActor::CActor( int theType,
				   int theXPos,
				   int theYPos)
	{
		type			= theType;
		state			= NONE;
		ControlState	= NULL;
		xPos			= theXPos;
		yPos			= theYPos;
		screenXPos		= 0;
		screenYPos		= 0;
		isVisible		= true;
		aniFrame		= 0;
		DirectionTravel	= STILL;
		Height			= 0;

		WeaponStat		= 0;
		Uses			= 0;;
	};

	//******************************************
	/*         METHODS                        */
	//******************************************

	int getType(){return type;};
	void	setType(int _Type) {type = _Type;}

	DWORD	getState(){return state;};
	void	setState(DWORD theState){state = theState;};

	int		getXPos(){return (xPos >> 4);};
	int		getYpos(){return (yPos >> 4);};
	int		getYPos(){return (yPos >> 4);};

	int		getScreenXPos(){return screenXPos;};
	int		getScreenYPos(){return screenYPos;};

	int		getAniFrame(){return aniFrame;};

	//returns MOVEMENT direction
	int		GetDirection(){return DirectionTravel;};

	void	SetHeight(int theHeight){ Height = theHeight; };
	int		GetHeight(){return Height;};

	bool	GetVisible(){return isVisible;};

	int		getxPos(){return (xPos >> 4);};
	int		getyPos(){return (yPos >> 4);};

	void	setYPos(int Y){yPos = Y << 4;};
	void	setXPos(int X){xPos = X << 4;};

	void	setxPos(int X){xPos = X << 4;};
	void	setyPos(int Y){yPos = Y << 4;};
	//****************************************//
	//			VARIABLES					  //
	//****************************************//
	int	 WeaponStat;
	int  Uses;
protected:
	
	int		type;				// Look in GameDefines.h for type definitions.
	DWORD	state;				// Playing, standing, off, on..... this is a flag.in the style 0x00000000 look at game defines
	DWORD	ControlState;		// Flag to show what the comp is doing with the char or if the human is controlling
	int		xPos;
	int     yPos;
	int		screenXPos;
	int		screenYPos;
	bool	isVisible;
	int		aniFrame;
	int		DirectionTravel;	// An int form 0-7 where 0 is directly up the pitch;
	int		Height;				// Probably 0, 1, 2.


};

#endif