#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"

#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"

// ----
bool	CFrontEnd::CupResults()
{
	char	convert[24];
	int		TeamAScore;
	int		TeamBScore;

	if (	(!GraphicsObjectRef) 
		||	(!ControllerRef) 
		||	(!GameDataRef))
		return false;

	if (		(!DialogInitialised)
		||		(!RedrawCountDown) )
	{
		// If this far then draw the screen.
		if (GameDataRef->GetCupLeg() > 0)
		{
			GraphicsObjectRef->StartDraw();
			GraphicsObjectRef->ClearScreen(0, 0, 0);

			// CupScore hasn't been processed yet! - thus must do sum here.
			TeamAScore = GameDataRef->GetActiveTeamByIndex(0)->Score + GameDataRef->GetActiveTeamByIndex(0)->CupScore;
			TeamBScore = GameDataRef->GetActiveTeamByIndex(1)->Score + GameDataRef->GetActiveTeamByIndex(1)->CupScore;

			// If you've won overall...
			if (TeamAScore >= TeamBScore)
			{
				RECT Region;
				Region.top		= WIN_TOP;
				Region.left		= WIN_LEFT;
				Region.bottom	= Region.top + WIN_HEIGHT;
				Region.right	= Region.left + WIN_WIDTH;

				CGraphicsData*	TempGraphic	= NULL;

				NEW(TempGraphic, CGraphicsData());
				SAFELY_CONTROL(if, TempGraphic, Init(GraphicsObjectRef, &CString(WIN_MATCH_SCREEN), PlatformRef), ==, true)
				{
					GraphicsObjectRef->DrawSprite(	(GraphicsObjectRef->GetDisplay()->cxWidth - WIN_WIDTH) >> 1,
													0,
													TempGraphic,
													Region);
				}
	//Adding text here for the win

#define TEXT_CR_TOP	(200)	
			
				if (TeamAScore == TeamBScore)
				{
					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("Game drawn"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_CR_TOP ,
													TEXT_CENTRED);
				}
				else
				{

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("A total score of"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_CR_TOP ,
													TEXT_CENTRED);

					sprintf(convert, "%03d", TeamAScore);
					TempString = " ";
					TempString += convert;
					TempString += " To ";
					sprintf(convert, "%03d ", TeamBScore);
					TempString += convert;

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&TempString,
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_CR_TOP + 20,
													TEXT_CENTRED);

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("A win for"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_CR_TOP + 40 ,
													TEXT_CENTRED);

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													GameDataRef->GetActiveTeamByIndex(0)->GetTeamName(),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_CR_TOP + 60,
													TEXT_CENTRED);

				}

				SAFELY_DELETE(TempGraphic);
			}
			else
			{
				RECT Region;
				Region.top		= LOSE_TOP;
				Region.left		= LOSE_LEFT;
				Region.bottom	= Region.top + LOSE_HEIGHT;
				Region.right	= Region.left + LOSE_WIDTH;

				CGraphicsData*	TempGraphic	= NULL;

				NEW(TempGraphic, CGraphicsData());
				SAFELY_CONTROL(if, TempGraphic, Init(GraphicsObjectRef, &CString(LOSE_MATCH_SCREEN), PlatformRef), ==, true)
				{
					GraphicsObjectRef->DrawSprite(	(GraphicsObjectRef->GetDisplay()->cxWidth - LOSE_WIDTH) >> 1,
													0,
													TempGraphic,
													Region);
				}

				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&CString("A total score of"),
												WIN_LEFT + (WIN_WIDTH/2),
												TEXT_CR_TOP,
												TEXT_CENTRED);

				sprintf(convert, "%03d", TeamAScore);
				TempString = convert;
				TempString += " to ";
				sprintf(convert, "%03d ", TeamBScore);
				TempString += convert;

				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												WIN_LEFT + (WIN_WIDTH/2),
												TEXT_CR_TOP + 20,
												TEXT_CENTRED);

				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&CString("A loss for"),
												WIN_LEFT + (WIN_WIDTH/2),
												TEXT_CR_TOP + 40 ,
												TEXT_CENTRED);

				GraphicsObjectRef->DrawString(	ThePopUpFont,
												GameDataRef->GetActiveTeamByIndex(0)->GetTeamName(),
												WIN_LEFT + (WIN_WIDTH/2),
												TEXT_CR_TOP + 60,
												TEXT_CENTRED);
				SAFELY_DELETE(TempGraphic);
			}

			GraphicsObjectRef->EndDraw();
		}
		

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// If button press, or time out. 
	if (	(GameDataRef->GetCupLeg() == 0)
		||	(ControllerRef->IsButtonPressedDB(Select)) 
		||	(ControllerRef->IsButtonPressedDB(MouseSelect))
		||	(GetTimeDifference(&TimeNow, &DialogStartTime) > 4.0f))
	{
		// Process the stars
		GameDataRef->ProcessStarPlayers();

		// Update cup standings, and choose new opponent before going to tables.
		GameDataRef->UpdateCupState();
		SetState(EFES_Single_Cup_New_RoundN);
		return true;
	}

	return true;
}