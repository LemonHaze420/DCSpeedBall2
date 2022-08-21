#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"

#define	TEXT_MENU_TITLE_CUP_TABLE	"Fixture List"
#define	TEXT_PLAY					"Play"
#define	CUPTABLE_TEAMA_X			(USABLE_LEFT + USABLE_COMMON_BORDER)
#define	CUPTABLE_TEAMB_X			(USABLE_LEFT + USABLE_WIDTH - USABLE_COMMON_BORDER)
#define	CUPTABLE_TEAM_Y				(((GraphicsObjectRef->GetDisplay()->cyHeight - NumRows * SmallLightFont->GetLineHeight()) >> 1) + (i * SmallLightFont->GetLineHeight()))
// ----
bool	CFrontEnd::CupTable()
{
	RECT	Region;
	int		i;

	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef))
		return false;

	if (		(!DialogInitialised)
		||		(!RedrawCountDown) )
	{
		SAFELY_CALL(GraphicsObjectRef, StartDraw());

		// Clear screen
		SAFELY_CALL(GraphicsObjectRef, ClearScreen(255, 255, 255));
/*	
		// Load screen
		SAFELY_DELETE(BackgroundGraphic);
		NEW(BackgroundGraphic, CGraphicsData());
		SAFELY_CONTROL(if, BackgroundGraphic, Init(GraphicsObjectRef, &CString(MAIN_MENU_SCREEN_GRAPHIC), PlatformRef), ==, false)
		{
			SAFELY_CALL(GraphicsObjectRef, EndDraw());
			return false;
		}
*/
		// Draw background image
		Region.left = 0;
		Region.top = 0;
		Region.right = GraphicsObjectRef->GetDisplay()->cxWidth;
		Region.bottom = GraphicsObjectRef->GetDisplay()->cyHeight;
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(0, 0, BackgroundGraphic, Region));

		// Draw background colour
		Region.top		= USABLE_TOP;
		Region.bottom	= Region.top + USABLE_HEIGHT;
		Region.left		= USABLE_LEFT;
		Region.right	= Region.left + USABLE_WIDTH;
		SAFELY_CALL(GraphicsObjectRef, DrawRect(&Region, 39, 64, 64));

		// Screen title
		GraphicsObjectRef->DrawString(	TopSmallFont, 
										&CString(TEXT_MENU_TITLE_CUP_TABLE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);

		int NumRows = 0;

		// Loop through all the possible meetings
		for (i = 0; i < MAX_CUP_FIXTURES; i++)
		{
			IntCouple*	ic = GameDataRef->GetCupDraw(GameDataRef->GetCupRound(), i);
			int TeamIntA = ic->A;
			int TeamIntB = ic->B;

			// Quit if any of the team values are out of bounds (in later rounds some should be -1, indicating no fixture).
			if (	(TeamIntA < 0)
				||	(TeamIntA >= 16)
				||	(TeamIntB < 0)
				||	(TeamIntB >= 16))
				break;

			NumRows++;
		}

		for (i = 0; i < NumRows; i++)
		{
			// Ret team data
			IntCouple*	ic = GameDataRef->GetCupDraw(GameDataRef->GetCupRound(), i);
			CTeam	*TeamA = GameDataRef->GetTeamByIndex(ic->A);
			CTeam	*TeamB = GameDataRef->GetTeamByIndex(ic->B);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											TeamA->GetShortTeamName(),
											CUPTABLE_TEAMA_X,
											CUPTABLE_TEAM_Y);

			TempString = TEXT_PLAY;
			GraphicsObjectRef->DrawString(	SmallLightFont, 
											&TempString,
											GraphicsObjectRef->GetDisplay()->cxWidth >> 1,
											CUPTABLE_TEAM_Y,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	SmallLightFont, 
											TeamB->GetShortTeamName(),
											CUPTABLE_TEAMB_X,
											CUPTABLE_TEAM_Y,
											TEXT_RIGHT);
		}

		SAFELY_CALL(GraphicsObjectRef, EndDraw());

		DialogInitialised = true;
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	if (	(ControllerRef->IsButtonPressedDB(Select))
		||	(ControllerRef->IsButtonPressedDB(MouseSelect)))
	{
		switch (FrontEndState)
		{
		case EFES_Single_Cup_New_Tables:
		case EFES_Single_Cup_New_Tables_Then_Test_Win: // ???
			SetState(EFES_Single_Cup_New_Manager);
			break;
		default:
			return false;
		}
	}

	return true;
}