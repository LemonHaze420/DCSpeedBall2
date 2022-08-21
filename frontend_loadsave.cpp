#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"

#define LOAD_GRAPHIC					"loadoverlay"
#define	TEXT_BOTTOM				(SCREEN_FOOTER_POSITION - 25)
#define TEXT_START				(USABLE_LEFT + 24)
								
#define BOX_START_X				16
#define BOX_START_Y				32
#define BOX_HEIGHT				160
#define BOX_WIDTH				96
#define BOX_GAP					16

#define	LOAD_CAPTION_LEFT		48
#define	LOAD_VALUE_LEFT			(LOAD_CAPTION_LEFT + 52)

// ----
// Load screen (not to be confused with LoadingScreen - which tells the user that the game is loading).
bool	CFrontEnd::Load()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!LoadOverlay)
	{
		NEW(LoadOverlay, CGraphicsData());
		SAFELY_CONTROL(if, LoadOverlay, Init(GraphicsObjectRef, &CString(LOAD_GRAPHIC), PlatformRef), ==, false)
			return false;
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Load";
		MenuTextArray[1]	= "Back";

		// Force an immediate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;

		SelectedFace = 0; // menu 1 offset
		FaceOffset = 0; // menu 2 offset
		SelectedBodyPart = 0;

		SAFELY_CALL(SoundSystemRef, Process());
		GameDataRef->CatSaveFiles();
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
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

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));	
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	//DefaultVerticalNav(NumberOfOptions, true);

	IfRightDB					// Macros
	{
		CurrentMenuPos--;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfLeftDB
	{
		CurrentMenuPos++;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
	}

	if (CurrentMenuPos < 0)	
		CurrentMenuPos = NumberOfOptions - 1;
	if (CurrentMenuPos > (NumberOfOptions - 1))
		CurrentMenuPos = 0;

	IfUpDB					// Macros
	{
		SelectedBodyPart--;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfDownDB
	{
		SelectedBodyPart++;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
	}

	if (SelectedBodyPart < 0)	
		SelectedBodyPart =  GameDataRef->GetNumberOfSaves() - 1;
	if (SelectedBodyPart >= ((int)GameDataRef->GetNumberOfSaves()))
		SelectedBodyPart = 0;


	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = TEXT_BOTTOM;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, TEXT_BOTTOM, BackgroundGraphic, region));

			// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString("Load"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											TEXT_START + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * i), 
											TEXT_BOTTOM,
											TEXT_LEFT, 
											GameWindowRef->GetMousePoint(),
											&InRect);	

			// Process mouse
			if (InRect)
			{
				// Zoom to the control.
				if (CurrentMenuPos != i)
				{
					CurrentMenuPos = i;
				//SelectedBodyPart = i;
					SOUND_SELECT;
				}
				MouseMovedSelected = true;
			}
		}
		
		// 
		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString(">")),
										(TEXT_START - 20) + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * CurrentMenuPos), //+ (150 * i) + 15, 
										TEXT_BOTTOM,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										(TEXT_START + 80) + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * CurrentMenuPos), //+ (150 * i) - 15, 
										TEXT_BOTTOM,	// Random values, plucked from the air.
										TEXT_RIGHT);

		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = 207;
		all.bottom = 237;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(16, 31, LoadOverlay, all));

		//Titles for the cup and league boxes

		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&(CString("League")),
										TEXT_START + 5, //+ (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * i), 
										40,
										TEXT_LEFT);
		
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&(CString("Cup")),
										TEXT_START + 5 + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2))), 
										40,
										TEXT_LEFT);

		SaveInfoStruct* theSave;
		SaveInfoStruct* SelectedSave = NULL;

		if (GameDataRef->GetNumberOfSaves())
		{
			int NumberOfSaves = GameDataRef->GetNumOfLeagueSaves();

			if(NumberOfSaves > 15)
			{
				NumberOfSaves = 15;
			}

			for(i = 0 + SelectedFace; i < (NumberOfSaves + SelectedFace); i++)
			{
				bool InRect;

				if(GameDataRef->GetLeagueSaveInfo(i, &theSave))
				{
					//suceeded

					if(	i != SelectedBodyPart)
					{
						GraphicsObjectRef->DrawString(	SmallDarkFont, 
														&(theSave->FriendlyName),
														TEXT_START - 10, 
														50 + ( (i - SelectedFace) * 9) ,
														TEXT_LEFT,
														GameWindowRef->GetMousePoint(),
														&InRect);	
					}
					else
					{
						GraphicsObjectRef->DrawString(	SmallLightFont, 
														&(theSave->FriendlyName),
														TEXT_START - 10, 
														50 + ( (i - SelectedFace) * 9) ,
														TEXT_LEFT,
														GameWindowRef->GetMousePoint(),
														&InRect);	
					}

				}
				else
				{
					//failed
					/*
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(CString("Failed")),
													TEXT_START - 10, 
													50 + ( (i - SelectedFace) * 9) ,
													TEXT_LEFT,
													GameWindowRef->GetMousePoint(),
													&InRect);
													*/
					break;
				}

				if (InRect)
				{
					// Zoom to the control.
					if (SelectedBodyPart != i)
					{
						SelectedBodyPart = i;
						SOUND_SELECT;
					}	
				}
			}

			// Now the scroll bar for the left
			//bottom  = 192
			//*********************LEFT SCROLL BARR************************************
			//RECT region;
			region.top		= 32;
			region.bottom	= 112;
			region.left		= 96;
			region.right	= 116;


			bool InRect = true;
			POINT*	_Point = GameWindowRef->GetMousePoint();

			if (	(_Point->x >= 0)			// Is the mouse on the screen?
					&&	(_Point->y >= 0)

					&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
					&&	(_Point->x < region.right) 
					&&	(_Point->y > region.top) 
					&&	(_Point->y < region.bottom) 
					)
					InRect = true;	// Yes.
				else
					InRect = false;	// No, to one of the above.

			if (InRect)
			{
				if( (int)(GameDataRef->GetNumOfLeagueSaves() - 15) > 0)
				{
					// Zoom to the control.
					SelectedFace--;
					//SelectedBodyPart--;

					//if(SelectedFace < 0)
					//	SelectedFace = 0;

					//if(SelectedFace > (int)(GameDataRef->GetNumOfLeagueSaves() - 15) )
					//{
					//	SelectedFace = GameDataRef->GetNumOfLeagueSaves() - 15;

					//	if(SelectedFace < 0)
					//	SelectedFace = 0;
					//}
					//SOUND_SELECT;
				}
			}

			

			region.top		= 112;
			region.bottom	= 192;
			region.left		= 96;
			region.right	= 116;


			InRect = true;
			_Point = GameWindowRef->GetMousePoint();

			if (	(_Point->x >= 0)			// Is the mouse on the screen?
					&&	(_Point->y >= 0)

					&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
					&&	(_Point->x < region.right) 
					&&	(_Point->y > region.top) 
					&&	(_Point->y < region.bottom) 
					)
					InRect = true;	// Yes.
				else
					InRect = false;	// No, to one of the above.

			if (InRect)
			{
				if( (int)(GameDataRef->GetNumOfLeagueSaves() - 15) > 0)
				{
					// Zoom to the control.
					SelectedFace++;
					//SelectedBodyPart++;

					//SOUND_SELECT;
				}
			}

			/*
			// If on the left...
			if ((int)SelectedBodyPart < (int)GameDataRef->GetNumOfLeagueSaves())
			{
				if (SelectedBodyPart < SelectedFace)
					SelectedFace = SelectedBodyPart;

				if (SelectedBodyPart >= (SelectedFace + 15))
					SelectedFace = SelectedBodyPart - 15;
			}
			*/

			// Safe guard
			if (SelectedFace > ((int)GameDataRef->GetNumOfLeagueSaves() - 15))
				SelectedFace = max((int)0, (int)(GameDataRef->GetNumOfLeagueSaves() - 15));

			if (SelectedFace < 0)
				SelectedFace = 0;


			//**********************END LEFT SCROLL BARR*********************************************

			NumberOfSaves = GameDataRef->GetNumberOfSaves();
			if(NumberOfSaves > (int)(15 +GameDataRef->GetNumOfLeagueSaves()))
			{
				NumberOfSaves = (15 + GameDataRef->GetNumOfLeagueSaves());
			}

			for(i = (GameDataRef->GetNumOfLeagueSaves() + FaceOffset); i < (NumberOfSaves + FaceOffset); i++)
			{
				bool InRect;

				if(GameDataRef->GetSaveLCInfo(i, &theSave))
				{
					//suceeded

					if(SelectedBodyPart!= i )
					{
						GraphicsObjectRef->DrawString(	SmallDarkFont, 
														&(theSave->FriendlyName),
														(GraphicsObjectRef->GetDisplay()->cxWidth) - 25, 
														50 + (((i - FaceOffset) - GameDataRef->GetNumOfLeagueSaves()) * 9) ,
														TEXT_RIGHT,
														GameWindowRef->GetMousePoint(),
														&InRect);
					}
					else
					{
						GraphicsObjectRef->DrawString(	SmallLightFont, 
														&(theSave->FriendlyName),
														(GraphicsObjectRef->GetDisplay()->cxWidth) - 25, 
														50 + (((i - FaceOffset) - GameDataRef->GetNumOfLeagueSaves()) * 9) ,
														TEXT_RIGHT,
														GameWindowRef->GetMousePoint(),
														&InRect);
					}

				}
				else
				{
					//failed
					/*
					GraphicsObjectRef->DrawString(	SmallDarkFont, 
													&(CString("Failed")),
													(GraphicsObjectRef->GetDisplay()->cxWidth) - 25, 
													50 + ( (i- FaceOffset) * 9) ,
													TEXT_RIGHT,
													GameWindowRef->GetMousePoint(),
													&InRect);
													*/
					break;
				}

				if (InRect)
				{
					// Zoom to the control.
					if (SelectedBodyPart != i)
					{
						SelectedBodyPart = i;
						//MouseMovedSelected = true;
						SOUND_SELECT;
					}
				}
			}

			//*********************RIGHT SCROLL BAR************************************************
			//RECT region;
			region.top		= 32;
			region.bottom	= 112;
			region.left		= 122;
			region.right	= 144;

			InRect = true;
			_Point = GameWindowRef->GetMousePoint();

			if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)
				&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < region.right) 
				&&	(_Point->y > region.top) 
				&&	(_Point->y < region.bottom) 
				)
				InRect = true;	// Yes.
			else
				InRect = false;	// No, to one of the above.

			if (InRect)
			{
			//	if(((int)GameDataRef->GetNumberOfSaves() - (int)GameDataRef->GetNumOfCupSaves()) > (int)(15 + GameDataRef->GetNumOfLeagueSaves()))
				{
					// Zoom to the control.
					FaceOffset--;
					//SelectedBodyPart--;

	//				SOUND_SELECT;
				}
			}

			

			region.top		= 112;
			region.bottom	= 192;
			region.left		= 122;
			region.right	= 144;


			InRect = true;
			_Point = GameWindowRef->GetMousePoint();

			if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)
				&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < region.right) 
				&&	(_Point->y > region.top) 
				&&	(_Point->y < region.bottom) 
				)
				InRect = true;	// Yes.
			else
				InRect = false;	// No, to one of the above.

			if (InRect)
			{
				//if(((int)GameDataRef->GetNumberOfSaves() - (int)GameDataRef->GetNumOfCupSaves()) > (int)(15 + GameDataRef->GetNumOfLeagueSaves()))
				{
					// Zoom to the control.
					FaceOffset++;
					//SelectedBodyPart ++;

				//	SOUND_SELECT;
				}
			}

			/*
			if ((int)SelectedBodyPart >= (int)GameDataRef->GetNumOfLeagueSaves())
			{
				if ((int)SelectedBodyPart < (int)(FaceOffset + GameDataRef->GetNumOfLeagueSaves()))
					FaceOffset = SelectedBodyPart - GameDataRef->GetNumOfLeagueSaves();

				if ((int)SelectedBodyPart >= (int)(FaceOffset + GameDataRef->GetNumOfLeagueSaves() + 15))
					FaceOffset = SelectedBodyPart - GameDataRef->GetNumOfLeagueSaves() - 15;
			}
			*/

			// Safe guards
			if (FaceOffset > ((int)GameDataRef->GetNumOfCupSaves() - 15))
				FaceOffset = max((int)0, (int)(GameDataRef->GetNumOfCupSaves() - 15));

			if (FaceOffset < 0)
				FaceOffset = 0;

			//*********************END RIGHT SCROLL BAR***********************************************
		}

		SelectedSave = NULL;
		//if (SelectedBodyPart < (int)GameDataRef->GetNumOfLeagueSaves())
		//{
			GameDataRef->GetSaveLCInfo(SelectedBodyPart, &SelectedSave);
		//}
		//else
		//{
		//	GameDataRef->GetSaveLCInfo(SelectedBodyPart, &SelectedSave);
		//}

		// Draw captions reguardless.
		TempString = "File";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										LOAD_CAPTION_LEFT, 
										214,
										TEXT_LEFT);
		TempString = "Date";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										LOAD_CAPTION_LEFT, 
										214 + 8,
										TEXT_LEFT);
		TempString = "Type";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										LOAD_CAPTION_LEFT, 
										214 + 16,
										TEXT_LEFT);
		TempString = "Round";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										LOAD_CAPTION_LEFT, 
										214 + 24,
										TEXT_LEFT);
		TempString = "Balance";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										LOAD_CAPTION_LEFT, 
										214 + 32,
										TEXT_LEFT);


		if(SelectedSave != NULL)
		{
			// Name
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&SelectedSave->FriendlyName,
											LOAD_VALUE_LEFT, 
											214,
											TEXT_LEFT);

			
			// Date
			SYSTEMTIME ShowDate;
			memset((void*)&ShowDate, 0, sizeof(SYSTEMTIME));
			ShowDate.wYear	= SelectedSave->Date >> 11;
			ShowDate.wMonth	= (SelectedSave->Date >> 6) & 0x1F;
			ShowDate.wDay	= SelectedSave->Date & 0x1F;

			TCHAR	DateBuffer[24];
			DateBuffer[0] = 0;
			GetDateFormat(	NULL, 
							DATE_SHORTDATE | LOCALE_NOUSEROVERRIDE,	// Default short date (only thing that will fit!).
							&ShowDate,
							NULL,
							DateBuffer,
							14);

			TCHAR*	TempChar;
#ifdef _UNICODE
			TempChar = wcsrchr(DateBuffer, '/');
#else
			TempChar = strrchr(DateBuffer, '/');
#endif
			while (TempChar)
			{
				*TempChar = '-';
#ifdef _UNICODE
				TempChar = wcschr(DateBuffer, '/');
#else
				TempChar = strchr(DateBuffer, '/');
#endif
			}
				
			TempString = DateBuffer;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											LOAD_VALUE_LEFT, 
											214 + 8,
											TEXT_LEFT);


			// GameType
			switch (SelectedSave->GameType)
			{
			case LEAGUE:
				TempString = "League";
				break;
			case LEAGUE_TEAM_MANAGER:
				TempString = "League-Manager";
				break;
			case CUP:
				TempString = "Cup";
				break;
			default:
				TempString = "N/A";
				break;
			}
			if (TempString.GetStringLength() > 0)
			{
				GraphicsObjectRef->DrawString(	SmallLightFont, 
												&TempString,
												LOAD_VALUE_LEFT, 
												214 + 16,
												TEXT_LEFT);
			}

			// Round
			sprintf(convert, 
					"%d", 
					SelectedSave->Round);
			TempString = convert;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											LOAD_VALUE_LEFT, 
											214 + 24,
											TEXT_LEFT);

			// Cash
			sprintf(convert, 
					"%d", 
					SelectedSave->Cash);
			TempString = convert;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											LOAD_VALUE_LEFT, 
											214 + 32,
											TEXT_LEFT);
		}
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (CurrentMenuPos)
		{
		case 0:	// Load game
			SOUND_SELECT;
		
			if (GameDataRef->GetNumberOfSaves())
			{
				if(SelectedBodyPart < (int)GameDataRef->GetNumOfLeagueSaves())
				{
					if(!GameDataRef->LoadLeagueGame(SelectedBodyPart))
					{
						SetState(EFES_Load_Failed);
						break;
					}
					else
					{
						switch (GameDataRef->GetGameType())
						{
						case LEAGUE:
							SetState(EFES_Single_League_New_Load_Successful);
							break;
 
						case LEAGUE_TEAM_MANAGER:
							SetState(EFES_Single_League_TeamManager_Load_Successful);
							break;
 
						case CUP:
							SetState(EFES_Single_Cup_New_Load_Successful);
							break;
 
						default:      
							SetState(EFES_Load_Failed);
							break;       
						}
					}
				}
				else
				{
					if(!GameDataRef->LoadCupGame(SelectedBodyPart - (int)GameDataRef->GetNumOfLeagueSaves()))
					{
						SetState(EFES_Load_Failed);
						break;
					}
					else
					{
						switch (GameDataRef->GetGameType())
						{
						case LEAGUE:
							SetState(EFES_Single_League_New_Load_Successful);
							break;
 
						case LEAGUE_TEAM_MANAGER:
							SetState(EFES_Single_League_TeamManager_Load_Successful);
							break;
 
						case CUP:
							SetState(EFES_Single_Cup_New_Load_Successful);
							break;
 
						default:      
							SetState(EFES_Load_Failed);
							break;       
						}
					}
				}
			}
			break;

		case 1:	// Cancel.
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

// ----
// Save the game
bool	CFrontEnd::Save()
{
	// Graphical crap here!
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	bool    AllSelectChanged	= false;
	int		NumberOfOptions		= 2;	// Remember to update this for each method!!!

	if (!LoadOverlay)
	{
		NEW(LoadOverlay, CGraphicsData());
		SAFELY_CONTROL(if, LoadOverlay, Init(GraphicsObjectRef, &CString(LOAD_GRAPHIC), PlatformRef), ==, false)
			return false;
	}

	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	if (!DialogInitialised)
	{
		MenuTextArray[0]	= "Save";
		MenuTextArray[1]	= "Back";

		// Force an immedicate redraw (in fact first draw) of the background.
		RedrawCountDown		= 0;

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;

		SelectedFace = 0;
		FaceOffset = 0;

		SAFELY_CALL(SoundSystemRef, Process());
		GameDataRef->CatSaveFiles();
		AllSelect = -1;

		TempStringArray[0] = "";
	}

	if (!RedrawCountDown)
	{
		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*
		// Load splash screen.
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
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

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, all));

	
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;
	// ----
	// Process
	//DefaultVerticalNav(NumberOfOptions, true);

	IfRightDB					// Macros
	{
		CurrentMenuPos--;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound));
	}

	IfLeftDB
	{
		CurrentMenuPos++;
		SAFELY_CALL(SoundSystemRef, PlaySample(MovementSound))
	}

	if (CurrentMenuPos < 0)	
		CurrentMenuPos = NumberOfOptions - 1;
	if (CurrentMenuPos > (NumberOfOptions - 1))
		CurrentMenuPos = 0;

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		RECT region;
		region.left = 0;
		region.top = TEXT_BOTTOM;
		region.right = 240;
		region.bottom = 320;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, TEXT_BOTTOM, BackgroundGraphic, region));

			// Draw menu
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString("Save"),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);
/**************************LEFT_SCROLL_BAR********************************************************/

			//RECT region;
		region.top		= 32;
		region.bottom	= 112;
		region.left		= 96;
		region.right	= 116;


		bool InRect = true;
		POINT*	_Point = GameWindowRef->GetMousePoint();

		if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)

				&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < region.right) 
				&&	(_Point->y > region.top) 
				&&	(_Point->y < region.bottom) 
				)
				InRect = true;	// Yes.
			else
				InRect = false;	// No, to one of the above.

		if (InRect)
		{
			if( (int)(GameDataRef->GetNumberOfSaves() - 15) > 0)
			{
				// Zoom to the control.
				SelectedFace--;

				if(SelectedFace < 0)
					SelectedFace = 0;

				if(SelectedFace > (int)(GameDataRef->GetNumberOfSaves() - 15) )
				{
					SelectedFace = GameDataRef->GetNumberOfSaves() - 15;

					if(SelectedFace < 0)
					SelectedFace = 0;
				}

				//SOUND_SELECT;
			}
		}

		

		region.top		= 112;
		region.bottom	= 192;
		region.left		= 96;
		region.right	= 116;


		InRect = true;
		_Point = GameWindowRef->GetMousePoint();

		if (	(_Point->x >= 0)			// Is the mouse on the screen?
				&&	(_Point->y >= 0)

				&&	(_Point->x > region.left)		// Yes, so, is the mouse in the rectangle?
				&&	(_Point->x < region.right) 
				&&	(_Point->y > region.top) 
				&&	(_Point->y < region.bottom) 
				)
				InRect = true;	// Yes.
			else
				InRect = false;	// No, to one of the above.

		if (InRect)
		{
			if( (int)(GameDataRef->GetNumberOfSaves() - 15) > 0)
			{
				// Zoom to the control.
				SelectedFace++;

				if(SelectedFace > ((int)GameDataRef->GetNumberOfSaves() - 15))
					SelectedFace = (GameDataRef->GetNumberOfSaves() - 15);

				//SOUND_SELECT;
			}
		}

		//**********************END LEFT SCROLL BARR*********************************************

		// Draw buttons
		for (i = NumberOfOptions; i--;)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	MainMenuFont, 
											&(MenuTextArray[i]),
											TEXT_START + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * i), 
											TEXT_BOTTOM,
											TEXT_LEFT, 
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
										(TEXT_START - 20) + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * CurrentMenuPos), //+ (150 * i) + 15, 
										TEXT_BOTTOM,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	MainMenuFont, 
										&(CString("<")),
										(TEXT_START + 80) + (((GraphicsObjectRef->GetDisplay()->cxWidth / 2) - 10) * CurrentMenuPos), //+ (150 * i) - 15, 
										TEXT_BOTTOM,	// Random values, plucked from the air.
										TEXT_RIGHT);

		RECT all;
		all.left = 0;
		all.top = 0;
		all.right = 120 - 16;
		all.bottom = 200 - 31;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(16, 31, LoadOverlay, all));

		//Then put in all the save games that already exist
#define KEYBOARD_START_X	32
#define KEYBOARD_START_Y	206
#define KEY_SIZE			16

		//now draw my funky keyboard
		all.left = 0;
		all.top = 0;
		all.right = 176;
		all.bottom = 64;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(KEYBOARD_START_X, KEYBOARD_START_Y, KeyboardGraphic, all));

#define NUM_OF_KEYS		44


		for( int n = 0; n < NUM_OF_KEYS; n++)
		{
			POINT*		_Point;
			bool		_PointIsInDrawRegion = true;

			RECT key;
			key.left	= KEYBOARD_START_X + ( (n%11) * KEY_SIZE);
			key.right	= key.left + KEY_SIZE;
			key.top		= KEYBOARD_START_Y + ((n / 11) * KEY_SIZE);
			key.bottom	= key.top + KEY_SIZE;

			_Point =  GameWindowRef->GetMousePoint();

			if (	(_Point)
			&&	(_PointIsInDrawRegion)
			)
			{
				if (	(_Point->x >= 0)			// Is the mouse on the screen?
					&&	(_Point->y >= 0)

					&&	(_Point->x > key.left)		// Yes, so, is the mouse in the rectangle?
					&&	(_Point->x < key.right) 
					&&	(_Point->y > key.top) 
					&&	(_Point->y < key.bottom) 
					)
					_PointIsInDrawRegion = true;	// Yes.
				else
					_PointIsInDrawRegion = false;	// No, to one of the above.
			}

			if(_PointIsInDrawRegion)
			{
				SOUND_SELECT;
				AllSelect = n;
				AllSelectChanged = true;
			}
		}

	}

	CString temp;
	/*this is the test to see if a button on the keypad has been pressed*/
	IfSelectOrMouseSelectDB(AllSelectChanged)
	{
		switch(AllSelect)
		{
		case KEY_1:
			temp = "1";
			break;
		case KEY_2:
			temp = "2";
			break;
		case KEY_3:
			temp = "3";
			break;
		case KEY_4:
			temp = "4";
			break;
		case KEY_5:
			temp = "5";
			break;
		case KEY_6:
			temp = "6";
			break;
		case KEY_7:
			temp = "7";
			break;
		case KEY_8:
			temp = "8";
			break;
		case KEY_9:
			temp = "9";
			break;
		case KEY_0:
			temp = "0";
			break;
		case KEY_Q:
			temp = "Q";
			break;
		case KEY_W:
			temp = "W";
			break;	
		case KEY_E:
			temp = "E";
			break;
		case KEY_R:
			temp = "R";
			break;
		case KEY_T:
			temp = "T";
			break;
		case KEY_Y:
			temp = "Y";
			break;
		case KEY_U:
			temp = "U";
			break;
		case KEY_I:
			temp = "I";
			break;
		case KEY_O:
			temp = "O";
			break;
		case KEY_P:
			temp = "P";
			break;
		case KEY_A:
			temp = "A";
			break;
		case KEY_S:
			temp = "S";
			break;
		case KEY_D:
			temp = "D";
			break;
		case KEY_F:
			temp = "F";
			break;
		case KEY_G:
			temp = "G";
			break;
		case KEY_H:
			temp = "H";
			break;
		case KEY_J:
			temp = "J";
			break;		
		case KEY_K:
			temp = "K";
			break;
		case KEY_L:
			temp = "L";
			break;			
		case KEY_Z:
			temp = "Z";
			break;
		case KEY_X:
			temp = "X";
			break;
		case KEY_C:
			temp = "C";
			break;
		case KEY_V:
			temp = "V";
			break;
		case KEY_B:
			temp = "B";
			break;
		case KEY_N:
			temp = "N";
			break;
		case KEY_M:
			temp = "M";
			break;
		case KEY_RETURN_1:
		case KEY_RETURN_2:
		case KEY_RETURN_3:
			break;
		case KEY_DELETE_1:
		case KEY_DELETE_2:
			GameDataRef->GetSaveNamePointer()->DeleteLastChar();
			break;
		case KEY_BLANK_1:	
		case KEY_BLANK_2:
		case KEY_BLANK_3:
		default:
			break;
		}

		if(GameDataRef->GetSaveNamePointer()->GetStringLength() < 8 )
			*(GameDataRef->GetSaveNamePointer()) += temp;
	}

	RECT all;
	all.left = 120;
	all.top = 160;
	all.right = GraphicsObjectRef->GetDisplay()->cxWidth;
	all.bottom = 160 + 10;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(120, 160, BackgroundGraphic, all));
	//the text that is inputed....
	GraphicsObjectRef->DrawString(	SmallLightFont, 
									&(CString("Enter save name")),
									173, //+ (150 * i) + 15, 
									140,	// Random values, plucked from the air.
									TEXT_CENTRED);

	GraphicsObjectRef->DrawString(	SmallLightFont, 
									GameDataRef->GetSaveNamePointer(),
									173, //+ (150 * i) + 15, 
									160,	// Random values, plucked from the air.
									TEXT_CENTRED);

	//draw the existing saves
	int NumberOfSaves = GameDataRef->GetNumberOfSaves();
	if(NumberOfSaves > 15)
	{
		NumberOfSaves = 15;
	}

		SaveInfoStruct* theSave;
		//SaveInfoStruct* SelectedSave = NULL;
		//CString		SaveName;

		for(i = 0 + SelectedFace; i < NumberOfSaves + SelectedFace; i++)
		{
			bool InRect;

			if(GameDataRef->GetSaveLCInfo(i, &theSave))
			{
				//suceeded
				

				
				GraphicsObjectRef->DrawString(	SmallDarkFont, 
												&(theSave->FriendlyName),
												TEXT_START - 10, 
												50 + ( (i - SelectedFace) * 9) ,
												TEXT_LEFT,
												GameWindowRef->GetMousePoint(),
												&InRect);	
				
			}
			else
			{
				//failed
				GraphicsObjectRef->DrawString(	SmallDarkFont, 
												&(CString("Failed")),
												TEXT_START - 10, 
												50 + ( (i- SelectedFace) * 9) ,
												TEXT_LEFT,
												GameWindowRef->GetMousePoint(),
												&InRect);	
			}

			if (InRect)
			{
				// Zoom to the control.
				*(GameDataRef->GetSaveNamePointer()) = theSave->FriendlyName;
				//MouseMovedSelected = true;
				SOUND_SELECT;
			}
			
		}

	
	// /* This test will have to change to reflect the user pressing 'save'
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
	// */
		switch (CurrentMenuPos)
		{
		case 0:	// Save game
			SOUND_SELECT;

			switch (FrontEndState)
			{
			case EFES_Single_League_New_Save:
				if(!GameDataRef->SaveGame(GameDataRef->GetSaveNamePointer()))
				{
					SetState(EFES_Single_League_New_Save_Failed);
				}
				else
				{
					SetState(EFES_Single_League_New_Save_Successful);
				}
				break;
			case EFES_Single_League_TeamManager_Save:
				if(!GameDataRef->SaveGame(GameDataRef->GetSaveNamePointer()))
				{
					SetState(EFES_Single_League_TeamManager_Save_Failed);
				}
				else
				{
					SetState(EFES_Single_League_TeamManager_Save_Successful);
				}
				break;

			case EFES_Single_Cup_New_Save:
				if(!GameDataRef->SaveGame(GameDataRef->GetSaveNamePointer()))
				{
					SetState(EFES_Single_Cup_New_Save_Failed);
				}
				else
				{
					SetState(EFES_Single_Cup_New_Save_Successful);
				}
				break;

			default:
				NODEFAULT;
				break;
			}
			break;

		case 1:	// Cancel.
			SOUND_SELECT;
			switch (FrontEndState)
			{
			case EFES_Single_League_New_Save:
				SetState(EFES_Single_League_New_Manager);
				break;
			case EFES_Single_League_TeamManager_Save:
				SetState(EFES_Single_League_TeamManager_Manager);
				break;
			case EFES_Single_Cup_New_Save:
				SetState(EFES_Single_Cup_New_Manager);
				break;
			default:
				NODEFAULT;
				break;
			}
			break;

		default:
			NODEFAULT;
			break;
		}
	

		// Right - put the save code here. (The mode specified above will be stored and used upon loading).

	}

	SAFELY_CALL(GraphicsObjectRef, EndDraw());
	return true;
}