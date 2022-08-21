#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"



#define	MAIN_MENU_SCREEN_GRAPHIC		"menubg"
#define PITCH_GRAPHIC					"pitchhelp"
#define	USABLE_LEFT						13
#define	USABLE_TOP						27
#define	USABLE_WIDTH					214
#define	USABLE_HEIGHT					268

/******************************************/
//This method is put in by Anthony....
//But it still might work.
/*******************************************/

bool	CFrontEnd::HelpMenu()
{
	
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 8;	// Remember to update this for each method!!!

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Introduction";
		MenuTextArray[1]	= "Background";
		MenuTextArray[2]	= "Controls";
		MenuTextArray[3]	= "Tokens";
		MenuTextArray[4]	= "Game Modes";
		MenuTextArray[5]	= "Screens";
		MenuTextArray[6]	= "The Pitch";
		MenuTextArray[7]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));
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
		region.top = MENU_TOP;
		region.right = 240;
		region.bottom = MENU_TOP + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, MENU_TOP, StoryScreenBackground, region));


		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											MENU_TOP + i * MENU_ITEM_SPACING,
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
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + CurrentMenuPos * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:		// Hmmm, poor form
			SOUND_SELECT;
			SetState(EFES_Help_Overview);
			break;

		case 1:
			SOUND_SELECT;
			SetState(EFES_Help_Story);
			break;

		case 2:
			SOUND_SELECT;
			SetState(EFES_Help_Controls);
			break;

		case 3:
			SOUND_SELECT;
			SetState(EFES_Help_Items);
			break;

		case 4:
			SOUND_SELECT;
			SetState(EFES_Help_Game_Modes);
			break;

		case 5:
			SOUND_SELECT;
			SetState(EFES_Help_Screens);
			break;

		
		case 6:
			SOUND_SELECT;
			SetState(EFES_Help_Pitch);
			break;

		case 7:
			SOUND_SELECT;
			SetState(EFES_Main);
			break;

		default:
			NODEFAULT;
			break;
		}
	}
	

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//****************************************************************************************************
bool	CFrontEnd::HelpMenuOverview()
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

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
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
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.MAIN_MENU_SCREEN_GRAPHIC
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		// Draw caption at top
		/*
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString("Overview"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);
										*/

		//****************************************
		// This does the text!!!
		//****************************************

		FILE *text;
		TempString = PlatformRef->GetPath(EP_Text);
		// Generate file name here
		TempString += "intro.ctd";// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												15, 
												USABLE_TOP);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);
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
		region.top = SCREEN_FOOTER_POSITION;
		region.right = 240;
		region.bottom = SCREEN_FOOTER_POSITION + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, SCREEN_FOOTER_POSITION, StoryScreenBackground, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											(SCREEN_FOOTER_POSITION) + i * MENU_ITEM_SPACING,
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
		
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										(SCREEN_FOOTER_POSITION),// + NumberOfOptions * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										(SCREEN_FOOTER_POSITION),// + NumberOfOptions * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
		
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

//****************************************************************************************************
bool	CFrontEnd::HelpMenuStory()
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

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
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
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		//****************************************
		// This does the text!!!
		//****************************************

		FILE *text;

		TempString = PlatformRef->GetPath(EP_Text);
		// Generate file name here
		TempString += "story.ctd";// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												15, 
												USABLE_TOP);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);
	
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
		region.top = SCREEN_FOOTER_POSITION;
		region.right = 240;
		region.bottom = SCREEN_FOOTER_POSITION + MENU_ITEM_SPACING * (NumberOfOptions);
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, SCREEN_FOOTER_POSITION, StoryScreenBackground, region));

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
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

//****************************************************************************************************
bool	CFrontEnd::HelpMenuControls()
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

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
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
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		FILE *text;

		TempString = PlatformRef->GetPath(EP_Text);
		// Generate file name here
		TempString += "controls.ctd";// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												5, 
												10);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);
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
		region.top = SCREEN_FOOTER_POSITION;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, SCREEN_FOOTER_POSITION, StoryScreenBackground, region));

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
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}

//****************************************************************************************************
#define ITEM_SPACE	8
bool	CFrontEnd::HelpMenuItems()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int			i;
	bool		MouseMovedSelected	= false;
//	int			NumberOfOptions		= 1;	// Remember to update this for each method!!!
	int			itemOptions = 21;
	CString*		Items[21];
	int			SpriteNum;

	for (i = itemOptions; i--;)
		Items[i] = NULL;

	Items[0] = new CString("FREEZE TEAM");
	Items[1] = new CString("REDUCE TEAM");	
	Items[2] = new CString("INCREASE TEAM");
	Items[3] = new CString("MANIC");	
	Items[4] = new CString("SLOW TEAM");
	Items[5] = new CString("GRAB BALL");
	Items[6] = new CString("TRANSPORT");
	Items[7] = new CString("GOAL DOOR");
	Items[8] = new CString("SHIELD");
	Items[9] = new CString("FULL ENERGY");
	Items[10] = new CString("ZAP TEAM");	
	Items[11] = new CString("COINS");	
	Items[12] = new CString("SHOULDER");	
	Items[13] = new CString("CHESTPLATE");	
	Items[14] = new CString("BOOTS");		
	Items[15] = new CString("ARMPLATE");		
	Items[16] = new CString("GLOVES");		
	Items[17] = new CString("BOTTLE");	
	Items[18] = new CString("HELMET");		
	Items[19] = new CString("BITMAP SHADES");
	Items[20] = new CString("Back");

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
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
		SpriteNum = 0;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	IfLeftDB					// Macros
	{
		CurrentMenuPos -= 10;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfRightDB					// Macros
	{
		CurrentMenuPos += 10;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfUpDB					// Macros
	{
		if(CurrentMenuPos != 10)
			CurrentMenuPos--;
		else
			CurrentMenuPos = 20;

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfDownDB
	{
		if(CurrentMenuPos != 9)
			CurrentMenuPos++;
		else
			CurrentMenuPos = 20;

		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
	}

	if (CurrentMenuPos < 0)	
		CurrentMenuPos = itemOptions - 1;
	if (CurrentMenuPos > (itemOptions - 1))
		CurrentMenuPos = 0;


	char	convert[24];

	// Render the item words
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		//refresh background behind the icxon + text
		region.left = 0;
		region.top = 0;
		region.right = 240;
		region.bottom = 200;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, region));

		//refresh background behind exit label
		region.left = 0;
		region.top = 280;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 280, StoryScreenBackground, region));

		// Draw buttons
		for (i = itemOptions; i--;)
		{
			bool InRect = false;
			int YPlacing = 110 + i * ITEM_SPACE;
			int XPlacing = (GraphicsObjectRef->GetDisplay()->cxWidth >> 1) + 10;
			if( i < 10)
			{
				XPlacing = 20;
				YPlacing = 110 + (i + 10) * ITEM_SPACE;
			}

			if(i != 20)
			{
				if(CurrentMenuPos == i)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													Items[i],
													XPlacing, 
													YPlacing,
													TEXT_LEFT, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													Items[i],
													XPlacing, 
													YPlacing,
													TEXT_LEFT, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
			}
			else
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												Items[i],
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												SCREEN_FOOTER_POSITION,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}

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
	}


	if(CurrentMenuPos == 20)
	{
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										SCREEN_FOOTER_POSITION,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										SCREEN_FOOTER_POSITION,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		//and then do a switch on the other menu... ha ha ha cunning...
		switch (CurrentMenuPos)
		{
		case 20:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;
		default:
			SOUND_SELECT;
			break;
		}
	}


	SpriteNum = 0;
	switch (CurrentMenuPos)
	{
	case 0:
		SpriteNum = 0;
		break;
	case 1:
		SpriteNum = 2;
		break;
	case 2:
		SpriteNum = 3;
		break;
	case 3:
		SpriteNum = 4;
		break;
	case 4:
		SpriteNum = 5;
		break;
	case 5:
		SpriteNum = 6;
		break;
	case 6:
		SpriteNum = 7;
		break;
	case 7:
		SpriteNum = 8;
		break;
	case 8:
		SpriteNum = 9;
		break;
	case 9:
		SpriteNum = 10;
		break;
	case 10:
		SpriteNum = 11;
		break;
	case 11:
		SpriteNum = 20;
		break;
	case 12:
		SpriteNum = 13;
		break;
	case 13:
		SpriteNum = 16;
		break;
	case 14:
		SpriteNum = 14;
		break;
	case 15:
		SpriteNum = 15;
		break;
	case 16:
		SpriteNum = 12;
		break;
	case 17:
		SpriteNum = 17;
		break;
	case 18:
		SpriteNum = 18;
		break;
	case 19:
		SpriteNum = 19;
		break;
	case 20:
		break;
	default:
		NODEFAULT;
		break;
	}


	// Now draw the token

#define TOKEN_FRAME_WIDTH	32
#define TOKEN_FRAME_HEIGHT	32
#define	TOKEN_WIDTH_SHIFT	5
#define	TOKEN_HEIGHT_SHIFT	5
#define Y_TOKEN_PITCH		40

	RECT			Region;
	int				RenderX, RenderY;
	unsigned int	TokenFrame;

		
	RenderX		= (GraphicsObjectRef->GetDisplay()->cxWidth >> 1) - 16;
	RenderY		= 25;
	TokenFrame	= SpriteNum;

	// Use
	
	Region.top		= (TokenFrame / Y_TOKEN_PITCH) << TOKEN_HEIGHT_SHIFT;
	Region.bottom	= Region.top + TOKEN_FRAME_HEIGHT;
	Region.left		= ((TokenFrame % Y_TOKEN_PITCH) << TOKEN_WIDTH_SHIFT);
	Region.right	= (Region.left + TOKEN_FRAME_WIDTH);
	GraphicsObjectRef->DrawSprite(	RenderX,
									RenderY,
									ThePickupGraphic, 
									Region);
	
	
	//draws the text

	FILE *text;

	TempString = PlatformRef->GetPath(EP_Text);
	sprintf(convert, "token%d.ctd", SpriteNum);
	TempString += convert;// Put your file name here!
	text = _wfopen(TempString.GetString(), TEXT("rb")); // No longer translated!

	CString theText;

	if(text != NULL)
	{
		//fseek( text, 0L, SEEK_SET ); // No need
		
		if (theText.LoadEncodedString(text))
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(theText),
											25, 
											70);
		}
		fclose(text);
	}
	else
	{
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&(CString("Data file not found.....")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
										TEXT_CENTRED);

		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&(CString("Please re-install game.")),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
										TEXT_CENTRED);
	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	for (i = itemOptions; i--;)
	{
		SAFELY_DELETE_C2248(Items[i]);
	}

	return true;
}

//****************************************************************************************************
bool	CFrontEnd::HelpMenuGameModes()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 4;	// Remember to update this for each method!!!
	char convert[24];

	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "KnockOut";
		MenuTextArray[1]	= "Cup";
		MenuTextArray[2]	= "League";
		MenuTextArray[3]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		//****************************************
		// This does the text!!!
		//****************************************

		FILE *text;

		if(CurrentMenuPos > 2)
		{
			CurrentMenuPos = 0;
		}


		TempString = PlatformRef->GetPath(EP_Text);
		sprintf(convert, "modes%d.ctd", CurrentMenuPos + 1);
		TempString += convert;// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												15, 
												USABLE_TOP);
			}
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = 240;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 240, StoryScreenBackground, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			if( i == 3)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												(SCREEN_FOOTER_POSITION - 50) + i * 16,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}
			else
			{
				if(CurrentMenuPos == i)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
													(SCREEN_FOOTER_POSITION - 50) + i * 12,
													TEXT_CENTRED, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1,
													(SCREEN_FOOTER_POSITION - 50) + i * 12,
													TEXT_CENTRED,
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
			}

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					DialogInitialised = false; // This may need to be moved outside the 'if'.
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		if(CurrentMenuPos == 3)
		{
			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString(">")),
											MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION - 50) + CurrentMenuPos * 16,	// Random values, plucked from the air.
											TEXT_LEFT);

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString("<")),
											GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION - 50) + CurrentMenuPos * 16,	// Random values, plucked from the air.
											TEXT_RIGHT);
		}
	}
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			DialogInitialised = false;
			break;
		case 1:
			SOUND_SELECT;
			DialogInitialised = false;
			break;
		case 2:
			SOUND_SELECT;
			DialogInitialised = false;
			break;
		case 3:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//**************************************************************************************************
bool	CFrontEnd::HelpMenuScreens()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 5;	// Remember to update this for each method!!!
	char convert[24];
	//SelectedFace using selected face because its nice and free

	int		NumPages			= 2;
	CString Pages[2];
	Pages[0] = "page 1";
	Pages[1] = "Page 2";
		
	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "The Gym Screen";
		MenuTextArray[1]	= "The Manager Screen";
		MenuTextArray[2]	= "The Trading Screen";
		MenuTextArray[3]	= "The Keypad";
		MenuTextArray[4]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
		SelectedFace = 0;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		//****************************************
		// This does the text!!!
		//****************************************

		FILE *text;

		if(CurrentMenuPos > 3)
		{
			CurrentMenuPos = 0;
		}

		TempString = PlatformRef->GetPath(EP_Text);
		sprintf(convert, "screen%d-%d.ctd", CurrentMenuPos + 1, SelectedFace + 1);
		// Generate file name here
		TempString += convert;// Put your file name here!
		text = _wfopen(TempString.GetString(), TEXT("rb"));

		CString theText;

		if(text != NULL)
		{
			fseek( text, 0L, SEEK_SET );
			
			theText.LoadEncodedString(text);

			//if(		(CurrentMenuPos != 3)
			//	||	(SelectedFace != 1) )
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												5, 
												USABLE_TOP);
			}
			/*
			else
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(theText),
												5, 
												USABLE_TOP + 2);
			}
			*/
		}
		else
		{
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Data file not found.....")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&(CString("Please re-install game.")),
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
											SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
											TEXT_CENTRED);
		}

		fclose(text);

#define	KEY_PITCH					10
#define	KEY_WIDTH					32
#define	KEY_HEIGHT					32
#define	X_KEY_SPACING				23
#define X_KEY_LEFT                  (30-23)
#define X_KEY_RIGHT					(30)
#define	Y_KEY_SPACING				20
#define Y_KEY_START					(SCREEN_FOOTER_POSITION - 240)
		//Now if we have the key descriptions selected I want to draw some pretty pics of em.
		if(CurrentMenuPos == 3)
		{
			RECT Region;

			if(SelectedFace == 0)
			{
				//trade key
				Region.top		= (TRA_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((TRA_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START,
												KeysGraphic, 
												Region);
				
				Region.top		= (SAV_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((SAV_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + Y_KEY_SPACING,
												KeysGraphic, 
												Region);

				Region.top		= (TAB_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((TAB_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + (Y_KEY_SPACING * 2),
												KeysGraphic, 
												Region);

				Region.top		= (GYM_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((GYM_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 3),
												KeysGraphic, 
												Region);

				Region.top		= (STA_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((STA_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + (Y_KEY_SPACING * 4),
												KeysGraphic, 
												Region);

				Region.top		= (SUB_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((SUB_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 5),
												KeysGraphic, 
												Region);

				Region.top		= (FIX_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((FIX_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 6),
												KeysGraphic, 
												Region);

				Region.top		= (PLY_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((PLY_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 7),
												KeysGraphic, 
												Region);
				
			}
			else
			{
				Region.top		= (ALL_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((ALL_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START,
												KeysGraphic, 
												Region);
				
				Region.top		= (I_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((I_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + Y_KEY_SPACING,
												KeysGraphic, 
												Region);

				Region.top		= (G_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((G_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + (Y_KEY_SPACING * 2),
												KeysGraphic, 
												Region);

				Region.top		= (T_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((T_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 3),
												KeysGraphic, 
												Region);

				Region.top		= (UP_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((UP_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + (Y_KEY_SPACING * 4),
												KeysGraphic, 
												Region);

				Region.top		= (BUY_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((BUY_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 5),
												KeysGraphic, 
												Region);

				Region.top		= (FIX_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((FIX_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_LEFT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 6),
												KeysGraphic, 
												Region);

				Region.top		= (ESC_KEY / KEY_PITCH) << TOKEN_HEIGHT_SHIFT;
				Region.bottom	= Region.top + KEY_HEIGHT;
				Region.left		= ((ESC_KEY % KEY_PITCH) << TOKEN_WIDTH_SHIFT);
				Region.right	= (Region.left + KEY_WIDTH);
				GraphicsObjectRef->DrawSprite(	X_KEY_RIGHT,
												Y_KEY_START + ((Y_KEY_SPACING ) * 7),
												KeysGraphic, 
												Region);

			}
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

	//left and right go between pages!

	IfRightDB					// Macros
	{
		SelectedFace = 1;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfLeftDB
	{
		SelectedFace = 0;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
	}

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = 250;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 250, StoryScreenBackground, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			if( i == 4)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												(SCREEN_FOOTER_POSITION),
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}
			else
			{
				if(CurrentMenuPos == i)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
													(SCREEN_FOOTER_POSITION - 50) + i * 12,
													TEXT_CENTRED, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1,
													(SCREEN_FOOTER_POSITION - 50) + i * 12,
													TEXT_CENTRED,
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
			}

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				CurrentMenuPos = i;
				SelectedFace = 0;
				MouseMovedSelected = true;
				SOUND_SELECT;
				DialogInitialised = false;
			}
		}

		for (int p = NumPages; p--;)
		{
			bool InRect;

		
				if(SelectedFace == p)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&(Pages[p]),
													((GraphicsObjectRef->GetDisplay()->cxWidth >> 1) - 25) + p * 50, 
													(SCREEN_FOOTER_POSITION - 70), //+ p * 12,
													TEXT_CENTRED, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(Pages[p]),
													((GraphicsObjectRef->GetDisplay()->cxWidth >> 1) - 25) + p * 50,
													(SCREEN_FOOTER_POSITION - 70), //+ p * 12,
													TEXT_CENTRED,
													GameWindowRef->GetMousePoint(),
													&InRect);
		

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				SelectedFace = p;
				MouseMovedSelected = true;
				SOUND_SELECT;
				//DialogInitialised = false;
			}
		}
		}
		
		// 
		if(CurrentMenuPos == 4)
		{
			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString(">")),
											MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION),	// Random values, plucked from the air.
											TEXT_LEFT);

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString("<")),
											GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION),	// Random values, plucked from the air.
											TEXT_RIGHT);
		}
	}
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			//SelectedFace = 0;
			RedrawCountDown = 0;
			break;
		case 1:
			SOUND_SELECT;
			//SelectedFace = 0;
			RedrawCountDown = 0;
			break;
		case 2:
			SOUND_SELECT;
			//SelectedFace = 0;
			RedrawCountDown = 0;
			break;
		case 3:
			SOUND_SELECT;
			//SelectedFace = 0;
			RedrawCountDown = 0;
			break;
		case 4:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}

		switch(SelectedFace)
		{
		case 0:
			SOUND_SELECT;
			RedrawCountDown = 0;
			break;
		case 1:
			SOUND_SELECT;
			RedrawCountDown = 0;
			break;
		default:
			NODEFAULT;
			break;
		}
	}

	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
//*********************************************
//********************************************************************************************************
bool	CFrontEnd::HelpMenuPitch()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 3;	// Remember to update this for each method!!!
//	char convert[24];
		
	// Load background
	if (!StoryScreenBackground)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(StoryScreenBackground, CGraphicsData());
		SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

	if (!ThePitchGraphic)
	{
		//SAFELY_DELETE(StoryScreenBackground);
		NEW(ThePitchGraphic, CGraphicsData());
		SAFELY_CONTROL(if, ThePitchGraphic, Init(GraphicsObjectRef, &CString(PITCH_GRAPHIC), PlatformRef), ==, false)
		{
			return false;
		}
	}

//	SAFELY_DELETE(ThePitchGraphic);
//	NEW(ThePitchGraphic, CGraphicsData());
//	SAFELY_CONTROL(if, ThePitchGraphic, Init(GraphicsObjectRef, &CString(PITCH_GRAPHIC), PlatformRef), ==, false)
//		return false;

//	SAFELY_DELETE(StoryScreenBackground);
//	NEW(StoryScreenBackground, CGraphicsData());
//	SAFELY_CONTROL(if, StoryScreenBackground, Init(GraphicsObjectRef, &CString(STORY_SCREEN_GRAPHIC), PlatformRef), ==, false)
//		return false;

	SAFELY_CALL(GraphicsObjectRef, StartDraw());
	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Pitch Breakdown";
		MenuTextArray[1]	= "Pitch bonus objects";
		MenuTextArray[2]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));

		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		all.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, StoryScreenBackground, all));

		//****************************************
		// This does the text!!!
		//****************************************

		if(CurrentMenuPos > 2)
		{
			CurrentMenuPos = 0;
		}
       
		
		if(CurrentMenuPos == 0)
		{

			RECT Region;

			Region.top		= 0;
			Region.bottom	= 255;
			Region.left		= 0;
			Region.right	= 225;
			GraphicsObjectRef->DrawSprite(	10,
											0,
											ThePitchGraphic, 
											Region);
		}
		else
		{
			FILE *text;

			//Firstly render all of those nice spangly graphics and stuff
			RECT Region;

			Region.top		= 0;
			Region.bottom	= 64;
			Region.left		= 0;
			Region.right	= 64;
			GraphicsObjectRef->DrawSprite(	10,
											20,
											TheBonusGraphic, 
											Region);
			Region.top		= 0;
			Region.bottom	= 32;
			Region.left		= 64;
			Region.right	= 96;
			GraphicsObjectRef->DrawSprite(	10,
											90,
											TheBonusGraphic, 
											Region);
			Region.top		= 0;
			Region.bottom	= 32;
			Region.left		= 96;
			Region.right	= 128;
			GraphicsObjectRef->DrawSprite(	10,
											130,
											TheBonusGraphic, 
											Region);
			Region.top		= 32;
			Region.bottom	= 64;
			Region.left		= 64;
			Region.right	= 96;
			GraphicsObjectRef->DrawSprite(	10,
											170,
											TheBonusGraphic, 
											Region);
			Region.top		= 32;
			Region.bottom	= 64;
			Region.left		= 96;
			Region.right	= 128;
			GraphicsObjectRef->DrawSprite(	10,
											210,
											TheBonusGraphic, 
											Region);
			CString theText;
			TempString = PlatformRef->GetPath(EP_Text);
			// Generate file name here
			TempString += "bonus.ctd";// Put your file name here!
			text = _wfopen(TempString.GetString(), TEXT("rb"));

			if(text != NULL)
			{
				fseek( text, 0L, SEEK_SET );
				
				theText.LoadEncodedString(text);
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&(theText),
													25, 
													USABLE_TOP - 20);
				}
			}
			else
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(CString("Data file not found.....")),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												SCREEN_FOOTER_POSITION - 200 * MENU_ITEM_SPACING,
												TEXT_CENTRED);

				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&(CString("Please re-install game.")),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												SCREEN_FOOTER_POSITION - 180 * MENU_ITEM_SPACING,
												TEXT_CENTRED);
			}

			fclose(text);
		}
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	DefaultVerticalNav(NumberOfOptions, true);

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = 250;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 250, StoryScreenBackground, region));

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			if( i == 2)
			{
				GraphicsObjectRef->DrawString(	MainMenuFont, 
												&(MenuTextArray[i]),
												GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
												(SCREEN_FOOTER_POSITION - 30) + i * 12,
												TEXT_CENTRED, 
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}
			else
			{
				if(CurrentMenuPos == i)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
													(SCREEN_FOOTER_POSITION - 30) + i * 12,
													TEXT_CENTRED, 
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(MenuTextArray[i]),
													GraphicsObjectRef->GetDisplay()->cxWidth >> 1,
													(SCREEN_FOOTER_POSITION - 30) + i * 12,
													TEXT_CENTRED,
													GameWindowRef->GetMousePoint(),
													&InRect);
				}
			}

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
					DialogInitialised = false;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		if(CurrentMenuPos == 2)
		{
			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString(">")),
											MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION - 30) + CurrentMenuPos * 12,	// Random values, plucked from the air.
											TEXT_LEFT);

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(CString("<")),
											GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
											(SCREEN_FOOTER_POSITION - 30) + CurrentMenuPos * 12,	// Random values, plucked from the air.
											TEXT_RIGHT);
		}
	}
	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:
			SOUND_SELECT;
			DialogInitialised = false;
			break;
		case 1:
			SOUND_SELECT;
			DialogInitialised = false;
			break;
		case 2:
			SOUND_SELECT;
			SetState(EFES_Help);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	
	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}