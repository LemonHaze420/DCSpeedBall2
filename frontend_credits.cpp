#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"

// ----
// Shows some credits.
bool	CFrontEnd::Credits()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
			return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 1;	// Remember to update this for each method!!!
	int		NumberOfCredits		= 38;

	if (!CreditsGraphic)
	{
		NEW(CreditsGraphic, CGraphicsData());
		SAFELY_CONTROL(if, CreditsGraphic, Init(GraphicsObjectRef, &CString(CREDITS_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;

		SelectedFace = 1465; //selected face is used as the scrolling thing on the credits

		/*
		* Now I want to put all of the people into the credits.....
		*/

		NumStringArray[37]	= "Broadsword Interactive";
		NumStringArray[36]	= " ";
		NumStringArray[35]	= "Management Team";
		NumStringArray[34]	= "John Jones-Steele";
		NumStringArray[33]	= "David Rowe";
		NumStringArray[32]	= " ";
		NumStringArray[31]	= "Programming Team";
		NumStringArray[30]	= "Andrew Evans";
		NumStringArray[29]	= "Anthony Dunn";
		NumStringArray[28]	= " ";
		NumStringArray[27]	= "Graphics Team";
		NumStringArray[26]	= "Philip Curtis";
		NumStringArray[25]	= "Sam Denney";
		NumStringArray[24]	= " ";
		NumStringArray[23]	= "Sound Technician";
		NumStringArray[22]	= "Steve Rose";
		NumStringArray[21]	= " ";
		NumStringArray[20]	= "Music";
		NumStringArray[19]	= "Anthony Dunn";
		NumStringArray[18]	= " ";
		NumStringArray[17]  = "Quality Assurance";
		NumStringArray[16]	= "Ed Bartlett";
		NumStringArray[15]	= " ";
		NumStringArray[14]	= "Copyright 1990-2001";
		NumStringArray[13]  = "The Bitmap Brothers";
		NumStringArray[12]	= "www.bitmap-brothers.co.uk";
		NumStringArray[11]	= " ";
		NumStringArray[10]	= " ";
		NumStringArray[9]	= "This product utilises ZLib";
		NumStringArray[8]	= "written by Mark Adler";
		NumStringArray[7]	= "and Jean-loup Gailly.";
		NumStringArray[6]   = " ";
		NumStringArray[5]	= " ";
		NumStringArray[4]	= "Look out for more";
		NumStringArray[3]	= "Exciting games from";
		NumStringArray[2]	= "Broadsword Interactive";
		NumStringArray[1]	= " ";
		NumStringArray[0]	= "www.broadsword.co.uk";
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(0, 0, 0));

		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = 240;
		all.bottom = 160;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, CreditsGraphic, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{

		RECT region;
		region.left = 0;
		region.top = 144;
		region.right = 240;
		region.bottom = 320;
		GraphicsObjectRef->DrawRect(&region, 0, 0, 0);

/*	// No logos yet.
		region.left = 0;
		region.top = 0;
		region.right = 64;
		region.bottom = 43;	
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(4, 256 - 12 - 56, InfiniteVenturesLogoGraphic, region));

		// Draw bitmap brothers logo.
		region.left = 0;
		region.top = 0;
		region.right = 57;
		region.bottom = 56;	
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(4, 256 - 12, BitmapBrothersLogoGraphic, region));
*/

#define	CREDITS_TOP		135
#define CREDITS_BOTTOM	(300)
		//counter for the height of the text
		SelectedFace--;

		for(i = NumberOfCredits; i--;)
		{
			int YDraw = CREDITS_TOP - (15 * i) + (SelectedFace / 2);

			if(YDraw < CREDITS_TOP)
				continue;

			if(YDraw > CREDITS_BOTTOM)
				continue;

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(NumStringArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth / 2,
											YDraw,	// Random values, plucked from the air.
											TEXT_CENTRED);
		}


		region.left = 0;
		region.top = 300;
		region.right = 240;
		region.bottom = 320;

		GraphicsObjectRef->DrawRect(&region, 0, 0, 0);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION + i * MENU_ITEM_SPACING,
											TEXT_CENTRED, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										SCREEN_FOOTER_POSITION + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										SCREEN_FOOTER_POSITION + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);

		//Ok draw the credits............
		//this is not gonna be easy...........
		//ummmmm.m................
		//first clear a rectangl

	}

	//just draw the bottom of the graghic again
	RECT all;
	all.left = 0;
	all.top = CREDITS_TOP;
	all.right = 240;
	all.bottom = 160;

	SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, CREDITS_TOP, CreditsGraphic, all));

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			SetState(EFES_Main);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	if(SelectedFace < 0)
	{
		SetState(EFES_Main);
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}