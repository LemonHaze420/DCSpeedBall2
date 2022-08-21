#include	"FrontEnd.h"
#include	"SafeProgramming.h"

#include	"Object.h"
#include	<windows.h>

#include	"GameData.h"
#include	"GameWindow.h"
#include	"Misc.h"

// ----
// The Stats screen

#define	TEXT_MENU_TITLE_STATISTICS		"Statistics"

#define	TEXT_SHOT_AT_GOAL				"Shots at goal"
#define	TEXT_GOALS_SCORED				"Goals scored"
#define	TEXT_GOALS_SAVED				"Goals saved"
#define	TEXT_TIME_IN_POSSESION			"Time in possesion"
#define	TEXT_TIME_IN_OPPENENTS_HALF		"Time in oppenents half"
#define	TEXT_SUCCESSFUL_TACKLES			"Successful tackles"
#define	TEXT_SUBSTITUTIONS				"Substitutions"
#define	TEXT_BONUS_POINTS_SCORED		"Bonus points scored"

#define FIXTURE_BORDER					USABLE_COMMON_BORDER
#define	STATS_TEAM_A_NAME_X_POS			(USABLE_LEFT + (USABLE_WIDTH >> 1))
#define	STATS_TEAM_A_NAME_Y_POS			(USABLE_TOP + FIXTURE_BORDER)
#define	STATS_TEAM_B_NAME_X_POS			STATS_TEAM_A_NAME_X_POS
#define	STATS_TEAM_B_NAME_Y_POS			(USABLE_TOP + USABLE_HEIGHT - (SmallLightFont->GetLineHeight() * 13) - FIXTURE_BORDER)

#define	STATS_TEAM_A_ATTRIBUTE_X_POS	(USABLE_LEFT + FIXTURE_BORDER)
#define	STATS_TEAM_A_ATTRIBUTE_Y_POS	(STATS_TEAM_A_NAME_Y_POS + SmallLightFont->GetLineHeight() * (i + 2))
#define	STATS_TEAM_B_ATTRIBUTE_X_POS	(USABLE_LEFT + FIXTURE_BORDER)
#define	STATS_TEAM_B_ATTRIBUTE_Y_POS	(STATS_TEAM_B_NAME_Y_POS + SmallLightFont->GetLineHeight() * (i + 2))
#define	STATS_TEAM_A_FIELD_X_POS		(USABLE_LEFT + USABLE_WIDTH - FIXTURE_BORDER)
#define	STATS_TEAM_A_FIELD_Y_POS		STATS_TEAM_A_ATTRIBUTE_Y_POS
#define	STATS_TEAM_B_FIELD_X_POS		(USABLE_LEFT + USABLE_WIDTH - FIXTURE_BORDER)
#define	STATS_TEAM_B_FIELD_Y_POS		STATS_TEAM_B_ATTRIBUTE_Y_POS

bool	CFrontEnd::Stats()
{
	RECT	Region;
	char	convert[24];
	int		i, j;
	int		value;

	if (	(!GraphicsObjectRef) 
		||	(!GameWindowRef)
		||	(!ControllerRef))
		return false;

	if (		(!DialogInitialised)
		||		(!RedrawCountDown)	)
	{
		SAFELY_CALL(GraphicsObjectRef, StartDraw());
		RedrawCountDown = 0;
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
										&CString(TEXT_MENU_TITLE_STATISTICS),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_CAPTION_POSITION,
										TEXT_CENTRED);

		// Screen footer
		GraphicsObjectRef->DrawString(	BottomSmallFont, 
										&CString(TEXT_PRESS_FIRE),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										SCREEN_FOOTER_POSITION,
										TEXT_CENTRED);

		
		// ----
		// Draw all the other information.
		CTeam	*TeamA	= GameDataRef->GetTeamFromDivision(GET_HUMAN_TEAM, SELECT_FROM_HUMAN_PLAYERS_DIVISION);
		CTeam	*TeamB	= GameDataRef->GetLastTeamPlayed();

		// Fail if one of the teams is invalid.
		if ((!TeamA) || (!TeamB))
		{
			GraphicsObjectRef->EndDraw();
			return false;
		}

		// -- 
		for (j = 0; j < 2; j++)
		{
			for (i = 0; i < 8; i++)
			{
				switch (i)
				{
				case 0:
					// Shots at goal
					value = (j==0)?TeamA->ShotsAtGoal:TeamB->ShotsAtGoal;
					TempStringArray[0] = TEXT_SHOT_AT_GOAL;
					break;

				case 1:
					// Goals scored
					value = (j==0)?TeamA->GoalsScored:TeamB->GoalsScored;
					TempStringArray[0] = TEXT_GOALS_SCORED;
					break;

				case 2:
					// Goals saved
					value = (j==0)?TeamA->GoalsSaved:TeamB->GoalsSaved;
					TempStringArray[0] = TEXT_GOALS_SAVED;
					break;

				case 3:
					// Time In Possesion
					value = (j==0)?TeamA->TimeInPossesion:TeamB->TimeInPossesion;
					TempStringArray[0] = TEXT_TIME_IN_POSSESION;
					break;

				case 4:
					// Time In Oppenents Half
					value = TeamA->TimeInOppenentsHalf + TeamB->TimeInOppenentsHalf;
					if (value > 0)
					{
						value = (TeamA->TimeInOppenentsHalf * GAME_PER_SIDE_DURATION * 2) / value;
						if (j)
							value = (GAME_PER_SIDE_DURATION * 2) - value;
					}
					else
						value = GAME_PER_SIDE_DURATION;
					TempStringArray[0] = TEXT_TIME_IN_OPPENENTS_HALF;
					break;

				case 5:
					// Succesful (not my fault) Tackles
					value = (j==0)?TeamA->SuccesfulTackles:TeamB->SuccesfulTackles;
					TempStringArray[0] = TEXT_SUCCESSFUL_TACKLES;
					break;

				case 6:
					// Substitutions
					value = (j==0)?TeamA->Substitutions:TeamB->Substitutions;
					TempStringArray[0] = TEXT_SUBSTITUTIONS;
					break;

				case 7:
					// Bonus Points Scored
					value = (j==0)?TeamA->BonusPointsScored:TeamB->BonusPointsScored;
					TempStringArray[0] = TEXT_BONUS_POINTS_SCORED;
					break;

				default:
					SAFELY_CALL(GraphicsObjectRef, EndDraw());
					return false;
				}

				sprintf(convert, "%03d", value);
				TempStringArray[1] = convert;
				if (j == 0)
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[0],
													STATS_TEAM_A_ATTRIBUTE_X_POS, 
													STATS_TEAM_A_ATTRIBUTE_Y_POS,
													TEXT_LEFT);

					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[1],
													STATS_TEAM_A_FIELD_X_POS, 
													STATS_TEAM_A_FIELD_Y_POS,
													TEXT_RIGHT);
				}
				else
				{
					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[0],
													STATS_TEAM_B_ATTRIBUTE_X_POS, 
													STATS_TEAM_B_ATTRIBUTE_Y_POS,
													TEXT_LEFT);

					GraphicsObjectRef->DrawString(	SmallLightFont, 
													&TempStringArray[1],
													STATS_TEAM_B_FIELD_X_POS, 
													STATS_TEAM_B_FIELD_Y_POS,
													TEXT_RIGHT);
				}
			}
		}

		// Centre bar
		i = 13;
		Region.top		= STATS_TEAM_A_ATTRIBUTE_Y_POS;
		Region.bottom	= STATS_TEAM_B_NAME_Y_POS - SmallLightFont->GetLineHeight() * 2;
		Region.left		= STATS_TEAM_A_ATTRIBUTE_X_POS;
		Region.right	= STATS_TEAM_B_FIELD_X_POS;
		GraphicsObjectRef->DrawRect(&Region, 96, 128, 128);

		TempString = "V's";
		GraphicsObjectRef->DrawString(	SmallLightFont, 
										&TempString,
										STATS_TEAM_A_NAME_X_POS ,
										(Region.top + ((Region.bottom - Region.top - SmallLightFont->GetLineHeight()) >> 1)),
										TEXT_CENTRED);

		GraphicsObjectRef->DrawString(	SmallLightFont, 
										TeamA->GetTeamName(),
										STATS_TEAM_A_NAME_X_POS, 
										STATS_TEAM_A_NAME_Y_POS,
										TEXT_CENTRED);


		GraphicsObjectRef->DrawString(	SmallLightFont, 
										TeamB->GetTeamName(),
										STATS_TEAM_B_NAME_X_POS, 
										STATS_TEAM_B_NAME_Y_POS,
										TEXT_CENTRED);


		SAFELY_CALL(GraphicsObjectRef, EndDraw());

		// Initialisation done.
		DialogInitialised	= true;
		DialogStartTime		= TimeNow;
	}

	RedrawCountDown--;
	if (RedrawCountDown < -1)
		RedrawCountDown = -1;

	// ----
	// Check for select button / mouse tap
	if (	(ControllerRef->IsButtonPressedDB(Select))
		||	(ControllerRef->IsButtonPressedDB(MouseSelect)))
	{
		// Set state depending on how we got here.
		switch (FrontEndState)
		{
		case EFES_Single_League_New_Statistics:
			SetState(EFES_Single_League_New_Manager);
			break;

		case EFES_Single_League_TeamManager_Statistics:	
			SetState(EFES_Single_League_TeamManager_Manager);
			break;

		case EFES_Single_Cup_New_Statistics:
			SetState(EFES_Single_Cup_New_Manager);
			break;

		default:
			NODEFAULT;
			break;
		}
	}

	return true;
}
