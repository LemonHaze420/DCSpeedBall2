/*
#include	"SafeProgramming.h"
#include	"Actor.h"

// ----
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
};


// ----
int		CActor::getType()
{
	return type;
}


// ----
DWORD	CActor::getState()
{
	return state;
}


// ----
void	CActor::setState(DWORD theState)
{
	state = theState;
}


// ----
int			CActor::getXPos()
{
	return xPos >> 4; 
}

int			CActor::getxPos()
{
	return xPos >> 4; 
}


// ----
int			CActor::getYpos()
{
	return yPos >> 4;
}

int			CActor::getyPos()
{
	return yPos >> 4;
}



void		CActor::setYPos(int Y)
{
	yPos  = Y << 4;
}		

void		CActor::setXPos(int X)
{
	xPos  = X << 4;
}	

void		CActor::setxPos(int X)
{
	xPos  = X << 4;
}	

void		CActor::setyPos(int Y)
{
	yPos  = Y << 4;
}			


// ----
int		CActor::getScreenXPos()
{
	return screenXPos;
}


// ----
int		CActor::getScreenYPos()
{
	return screenYPos;
}


// ----
int		CActor::getAniFrame()
{
	return aniFrame;
}

*/