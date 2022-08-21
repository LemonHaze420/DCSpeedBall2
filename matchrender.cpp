#include	"Match.h"
#include	"CompilerEnvironment.h"
#include	"SafeProgramming.h"
#include	"Object.h"
#include	"Misc.h"
#include	"Actor.h"

#include	"graphicsclasses.h"
#include	"GameDefines.h"

//**********************************

#define INFOBAR_HEIGHT				26
#define INFOBAR_PITCH_CLIP_HEIGHT	16
#define INFOBAR_WIDTH				240

// ----
// Draw the pitch and then the pickups + players + ball, etc.
bool	CMatch::Draw()
{
	if (GraphicsObjectRef)
	{
		// Do we need to show a replay?
		switch (MatchState)
		{
		case DoingGoalSideA:
		case DoingGoalSideB:
		case DoingHomeGoalSideA:
		case DoingHomeGoalSideB:
			if (!( (GameDataRef->GetGameType() == LEAGUE_TEAM_MANAGER) && (ControllerRef->IsButtonPressed(DebugQuickExit)) ))
			{
				if (ReplayTime > 0) 
				{
					ReplayTime--;
					return DrawReplay();
				}
			}
			else
			{
				ReplayTime = 0;
			}
			break;
		case MatchOver:
#ifndef DEMO_BUILD
			if (GameDataRef) 
			{
				GraphicsObjectRef->StartDraw();
				GraphicsObjectRef->ClearScreen(0, 0, 0);

				if (	(GameDataRef->GetActiveTeamByIndex(0)->Score) >=
						(GameDataRef->GetActiveTeamByIndex(1)->Score))
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

#define TEXT_TOP	(200)	
					CString	TempString;
					char	convert[24];

					if(	(GameDataRef->GetActiveTeamByIndex(0)->Score) ==
						(GameDataRef->GetActiveTeamByIndex(1)->Score))
					{
						GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("Game drawn"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_TOP ,
													TEXT_CENTRED);
					}
					else
					{

						GraphicsObjectRef->DrawString(	ThePopUpFont,
														&CString("A score of"),
														WIN_LEFT + (WIN_WIDTH/2),
														TEXT_TOP ,
														TEXT_CENTRED);

						sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
						TempString = " ";
						TempString += convert;
						TempString += " to ";
						sprintf(convert, "%03d ", GameDataRef->GetActiveTeamByIndex(1)->Score);
						TempString += convert;

						GraphicsObjectRef->DrawString(	ThePopUpFont,
														&TempString,
														WIN_LEFT + (WIN_WIDTH/2),
														TEXT_TOP + 20,
														TEXT_CENTRED);

						GraphicsObjectRef->DrawString(	ThePopUpFont,
														&CString("A win for"),
														WIN_LEFT + (WIN_WIDTH/2),
														TEXT_TOP + 40 ,
														TEXT_CENTRED);

						GraphicsObjectRef->DrawString(	ThePopUpFont,
														GameDataRef->GetActiveTeamByIndex(0)->GetTeamName(),
														WIN_LEFT + (WIN_WIDTH/2),
														TEXT_TOP + 60,
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

					CString	TempString;
					char	convert[24];

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("A Score of"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_TOP ,
													TEXT_CENTRED);

					sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
					TempString = " ";
					TempString += convert;
					TempString += " to ";
					sprintf(convert, "%03d ", GameDataRef->GetActiveTeamByIndex(1)->Score);
					TempString += convert;


					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&TempString,
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_TOP + 20,
													TEXT_CENTRED);

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													&CString("A Loss For"),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_TOP + 40 ,
													TEXT_CENTRED);

					GraphicsObjectRef->DrawString(	ThePopUpFont,
													GameDataRef->GetActiveTeamByIndex(0)->GetTeamName(),
													WIN_LEFT + (WIN_WIDTH/2),
													TEXT_TOP + 60,
													TEXT_CENTRED);
					SAFELY_DELETE(TempGraphic);
				}

				GraphicsObjectRef->EndDraw();
			}

#else
			GraphicsObjectRef->StartDraw();
			GraphicsObjectRef->ClearScreen(0, 0, 0);
			GraphicsObjectRef->EndDraw();
#endif
			return true;
		default:
			ReplayTime = REPLAY_BUFFER_LENGTH * 2;
			break;
		}

#ifdef NOT_FINAL_BUILD
		if ((TheBall) && (!ControllerRef->IsButtonPressed(Backup)))
#else
		if (TheBall)
#endif
		{
			int	DestX;
			DestX = max(0, int(TILE_WIDTH * PITCH_TILE_WIDTH	- GraphicsObjectRef->GetDisplay()->cxWidth) / 2		+ TheBall->getXPos());
			DestX = min(int(TILE_WIDTH * PITCH_TILE_WIDTH	- GraphicsObjectRef->GetDisplay()->cxWidth), DestX);

			int DestY;
			DestY = max(0, int(TILE_HEIGHT * PITCH_TILE_HEIGHT	- (GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_PITCH_CLIP_HEIGHT)) / 2	- TheBall->getYPos());
			DestY = min(int(TILE_HEIGHT * PITCH_TILE_HEIGHT	- (GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_PITCH_CLIP_HEIGHT)), DestY);
			
#define	CAMERA_SNAP	8
			if (	((CameraX - DestX) < int(CAMERA_SNAP))
				&&	((CameraX - DestX) > int(-CAMERA_SNAP)))
				CameraX += ((DestX - CameraX) >> 1);
			else 
			if (	((CameraX - DestX) < int( 3 * CAMERA_SNAP))
				&&	((CameraX - DestX) > int(-3 * CAMERA_SNAP)))
				CameraX += ((DestX - CameraX) >> 2);
			else
				CameraX += ((DestX - CameraX) >> 3);
			
			if (	((CameraY - DestY) < int(CAMERA_SNAP))
				&&	((CameraY - DestY) > int(-CAMERA_SNAP)))
				CameraY += ((DestY - CameraY) >> 1);
			else
			if (	((CameraY - DestY) < int( 3 * CAMERA_SNAP))
				&&	((CameraY - DestY) > int(-3 * CAMERA_SNAP)))
				CameraY += ((DestY - CameraY) >> 2);
			else
				CameraY += ((DestY - CameraY) >> 3);
		}
#ifdef NOT_FINAL_BUILD
		else
		{

			IfUp					// Macros
				CameraY-=5;
			
			IfDown
				CameraY+=5;
			
			IfLeft
				CameraX-=5;

			IfRight
				CameraX+=5;
		}
#endif

		// If the game is paused then we pause the replay buffer too.
		if (MatchState != Paused)
		{
			// Increment index
			ReplayBufferIndex++;
			if (ReplayBufferIndex >= REPLAY_BUFFER_LENGTH)
				ReplayBufferIndex = 0;

			ReplayBufferPlaybackIndex = ReplayBufferIndex + 1;
			if (ReplayBufferPlaybackIndex >= REPLAY_BUFFER_LENGTH)
				ReplayBufferPlaybackIndex = 0;

			// Store camera pos.
			ReplayBuffer[ReplayBufferIndex].CameraX = CameraX;
			ReplayBuffer[ReplayBufferIndex].CameraY = CameraY;
		}


		SAFELY_CALL(GraphicsObjectRef, StartDraw());

		bool ret = true;
		if (!( (GameDataRef->GetGameType() == LEAGUE_TEAM_MANAGER) && (ControllerRef->IsButtonPressed(DebugQuickExit)) ))
		{
			ret &= DrawPitch();
			ret &= DrawTokens();
			ret &= DrawDPad();
			ret &= DrawPlayers();
			ret &= DrawBall();
			ret &= DrawExtras();
			ret &= DrawPopUp();
		}
		ret &= DrawInfoBar();

		if(MatchState == Paused)
		{
			this->GamePause();
		}

		
#ifndef HIDE_FPS
#if defined(NOT_FINAL_BUILD) || defined(SHOW_FPS)
		char convert[32];
		if (FrameTime != 0.0f)
		{
			
#ifdef DEBUG
			sprintf(convert, "DEBUG %f fps", 1.0f / FrameTime);
#else
			sprintf(convert, "RELEASE %f fps", 1.0f / FrameTime);
#endif
			
			if (GraphicsObjectRef)
				GraphicsObjectRef->DrawString(	ThePopUpFont, 
												&CString(convert),
												240, 
												0, 
												TEXT_RIGHT);
			
												
		}

#endif
#endif
		
		SAFELY_CALL(GraphicsObjectRef, EndDraw());
		return ret;
	}
	return false;
}




//int XPos;// = (TILE_WIDTH * PITCH_TILE_WIDTH / 2) - 120;
//int YPos;// = (TILE_HEIGHT * PITCH_TILE_HEIGHT / 2) - 160;



// ----
#define ADDON_WIDTH						32
#define ADDON_HEIGHT					32
#define	ADDON_WIDTH_SHIFT				5
#define	ADDON_HEIGHT_SHIFT				5
#define Y_ADDON_PITCH					10

#define CHUTE_WIDTH						16
#define CHUTE_HEIGHT					64
#define CHUTE_ADDON_PITCH				4
#define	CHUTE_WIDTH_SHIFT				4
#define	CHUTE_HEIGHT_SHIFT				6

#define	SHROOM_IMAGE_INDEX_OFFSET		0
#define	DOME_IMAGE_INDEX_OFFSET			1
#define	BLUE_STAR_IMAGE_INDEX_OFFSET	2
#define	RIGHT_DOME_RENDER_Y_CENTRE		368
#define	LEFT_DOME_RENDER_Y_CENTRE		-368
#define	LEFT_CHUTE_RENDER_X				-320
#define	LEFT_CHUTE_RENDER_Y				0
#define	RIGHT_CHUTE_RENDER_X			(320 - 16)
#define	RIGHT_CHUTE_RENDER_Y			64


bool	CMatch::DrawPitch()
{
	if (!GraphicsObjectRef)
		return false;

	RECT Region;
	int XStart, XRenderPos, YRenderPos;

	XRenderPos = -(CameraX % TILE_WIDTH);
	YRenderPos = -(CameraY % TILE_HEIGHT);

	// Calculate X, Y screen coords here.
	XStart = XRenderPos;

	int LowerHeight, UpperHeight, LowerWidth, UpperWidth;
	LowerWidth = (CameraX / TILE_WIDTH);
	LowerHeight = (CameraY / TILE_HEIGHT);

	if (LowerWidth < 0)
		LowerWidth = 0;
	if (LowerWidth > PITCH_TILE_WIDTH)
		LowerWidth = PITCH_TILE_WIDTH;
	if (LowerHeight < 0)
		LowerHeight = 0;
	if (LowerHeight > PITCH_TILE_HEIGHT)
		LowerHeight = PITCH_TILE_HEIGHT;

	UpperWidth = LowerWidth + (GraphicsObjectRef->GetDisplay()->cxWidth / TILE_WIDTH) + 1;
	UpperHeight = LowerHeight + ((GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_PITCH_CLIP_HEIGHT) / TILE_HEIGHT) + 1;

	if (UpperWidth > PITCH_TILE_WIDTH)
		UpperWidth = PITCH_TILE_WIDTH;
	if (UpperHeight > PITCH_TILE_HEIGHT)
		UpperHeight = PITCH_TILE_HEIGHT;

	int	TileID, CachedTileID, Flipper;
	CachedTileID = -1;
	for (int y = LowerHeight; y < UpperHeight; y++)
	{
		for (int x = LowerWidth; x < UpperWidth; x++)
		{
			TileID = (PitchMap[y][x]);
			Flipper = TileID & 0xF000;	// Top four bits store flipping info,
			TileID &= 0x0FFF;			// Bottom 12 contain tile number.

			if (TileID != CachedTileID) // This is a worth will time saving at the moment. Check when complete.
			{
				// Calculate co-ordinates for the given tile.
				Region.top = (TileID / Y_TILE_PITCH) << TILE_HEIGHT_SHIFT;
				Region.bottom = Region.top + TILE_HEIGHT;
				Region.left = (TileID % Y_TILE_PITCH) << TILE_WIDTH_SHIFT;
				Region.right = Region.left + TILE_WIDTH;

				CachedTileID	= TileID;
			}
			
			SAFELY_CALL(GraphicsObjectRef, DrawNonTransSprite(XRenderPos, YRenderPos, ThePitchGraphic, Region, Flipper));


			XRenderPos += TILE_WIDTH;
		}

		YRenderPos += TILE_HEIGHT;
		XRenderPos = XStart;
	}


	// ----
	// Map add ons

	// Centre launcher
	if (	(TheBall->getState() == LAUNCHING)
		&&	(TheBall->AccelCount >= 30)
		&&	(TheBall->AccelCount <= 50) )
	{
		
		int tile = 0;
		
		if (TheBall->AccelCount >= 40)
		{
			tile = 49 - TheBall->AccelCount;
			PLAY_BALL_LAUNCH;
		}
		else 
		{
			if (TheBall->AccelCount >= 30)
				tile = TheBall->AccelCount - 30;
		}

		Region.top		= ((tile / Y_ADDON_PITCH) << ADDON_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + ADDON_HEIGHT);
		Region.left		= ((tile % Y_ADDON_PITCH) << ADDON_WIDTH_SHIFT);
		Region.right	= (Region.left + ADDON_WIDTH);

		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-(ADDON_WIDTH >> 1)),
										CMatch::PitchYToScreenY(ADDON_HEIGHT >> 1),
										TheLauncherGraphic,
										Region);
	}

	// Clear replay buffer.
	ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern = 0;

	/*
	TOP_SHROOM_BIT_MASK		
	BOTTOM_SHROOM_BIT_MASK	
	LEFT_DOME_BIT_MASK		
	RIGHT_DOME_BIT_MASK		
	LEFT_CHUTE_BIT_MASK		
	RIGHT_CHUTE_BIT_MASK	
	*/

	// Chute code...
	if (LeftChute)
	{
		int ChuteState = LeftChute->GetFlag();

		// Store for later...
		ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern |= (ChuteState << 4);

		if (ChuteState > 0)
		{
			ChuteState--;

			RECT Region;
			Region.top		= ((ChuteState / CHUTE_ADDON_PITCH) << CHUTE_HEIGHT_SHIFT);
			Region.bottom	= (Region.top + CHUTE_HEIGHT);
			Region.left		= ((ChuteState % CHUTE_ADDON_PITCH) << CHUTE_WIDTH_SHIFT);
			Region.right	= (Region.left + CHUTE_WIDTH);

			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(LEFT_CHUTE_RENDER_X),
											CMatch::PitchYToScreenY(LEFT_CHUTE_RENDER_Y), 
											TheChuteGraphic, 
											Region);

			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(RIGHT_CHUTE_RENDER_X),
											CMatch::PitchYToScreenY(RIGHT_CHUTE_RENDER_Y), 
											TheChuteGraphic, 
											Region,
											SPRITE_ROTATE_180);
		}
	}

	// ----
	// Top shroom
	if (	(TopShroom)
		&&	(TopShroom->getState() == LIT)
		//&&	(CameraY < (CMatch::PitchYToScreenY(TSHROOM_Y) + ADDON_HEIGHT))
		)
	{
		// Store state.
		ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern |= TOP_SHROOM_BIT_MASK;

		// Top shroom is visible
		RECT Region;

		Region.top		= ((SHROOM_IMAGE_INDEX_OFFSET / Y_ADDON_PITCH) << ADDON_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + ADDON_HEIGHT);
		Region.left		= ((SHROOM_IMAGE_INDEX_OFFSET % Y_ADDON_PITCH) << ADDON_WIDTH_SHIFT);
		Region.right	= (Region.left + ADDON_WIDTH);

		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-(ADDON_WIDTH >> 1)),
										CMatch::PitchYToScreenY(TSHROOM_Y + (ADDON_HEIGHT >> 1)), 
										TheAddOnsGraphic, 
										Region);
	}

	// Bottom shroom
	if (	(BottomShroom)
		&&	(BottomShroom->getState() == LIT)
		//&&	(int(CameraY + GraphicsObjectRef->GetDisplay()->cyHeight) > CMatch::PitchYToScreenY(BSHROOM_Y))
		)
	{
		// Store state.
		ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern |= BOTTOM_SHROOM_BIT_MASK;

		// Bottom shroom is visible
		RECT Region;

		Region.top		= ((SHROOM_IMAGE_INDEX_OFFSET / Y_ADDON_PITCH) << ADDON_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + ADDON_HEIGHT);
		Region.left		= ((SHROOM_IMAGE_INDEX_OFFSET % Y_ADDON_PITCH) << ADDON_WIDTH_SHIFT);
		Region.right	= (Region.left + ADDON_WIDTH);

		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-(ADDON_WIDTH >> 1)),
										CMatch::PitchYToScreenY(BSHROOM_Y + (ADDON_HEIGHT >> 1)), 
										TheAddOnsGraphic, 
										Region);
	}


	// ----
	// Left wall items
	if (CameraX < ADDON_WIDTH)
	{
		RECT Region;
		
		Region.top		= 0;
		Region.bottom	= Region.top + ADDON_HEIGHT;

		// Dome / light thing
		if (	(LeftDome) 
			&&	(LeftDome->getState() == LIT))
		{
			// Store state.
			ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern |= LEFT_DOME_BIT_MASK;

			Region.left		= ADDON_WIDTH * DOME_IMAGE_INDEX_OFFSET;
			Region.right	= Region.left + ADDON_WIDTH;

			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-320),
											CMatch::PitchYToScreenY(LEFT_DOME_RENDER_Y_CENTRE + (ADDON_HEIGHT >> 1)),
											TheAddOnsGraphic,
											Region);
		}

		// Store state.
		ReplayBuffer[ReplayBufferIndex].StarBank[0].BitPattern	= 0;
		int starcount = 0;

		if (LeftBank)
		{
			for (int i = 5; i--;)
			{
				// Check that an object exists.
				if (!LeftBank->TheStarBank[i])
					return false;

				if (LeftBank->TheStarBank[i]->OnOff)
				{
					ReplayBuffer[ReplayBufferIndex].StarBank[0].Owner = LeftBank->TheStarBank[i]->TeamControlling;
					ReplayBuffer[ReplayBufferIndex].StarBank[0].BitPattern |= 1 << i;

					Region.left		= ADDON_WIDTH * (BLUE_STAR_IMAGE_INDEX_OFFSET + LeftBank->TheStarBank[i]->TeamControlling);	
					Region.right	= Region.left + ADDON_WIDTH;

					GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-320),
													CMatch::PitchYToScreenY(LEFT_STARBANK_SY + i * ADDON_HEIGHT + ADDON_HEIGHT),
													TheAddOnsGraphic,
													Region);
					starcount++;
				}

				if(starcount == 5)
				{
					PLAY_ALLSTARS;
				}
			}
		}
	}

	// RIGHT!!!
	// If the right bank is on screen.
	if ((CameraX + GraphicsObjectRef->GetDisplay()->cxWidth) > (PITCH_TILE_WIDTH * TILE_WIDTH - ADDON_WIDTH))
	{
		RECT Region;
		
		Region.top		= 0;
		Region.bottom	= Region.top + ADDON_HEIGHT;

		// Dome / light thing
		if (	(RightDome) 
			&&	(RightDome->getState() == LIT))
		{
			// Store state.
			ReplayBuffer[ReplayBufferIndex].PitchItemsBitPattern |= RIGHT_DOME_BIT_MASK;

			Region.left		= ADDON_WIDTH * DOME_IMAGE_INDEX_OFFSET;
			Region.right	= Region.left + ADDON_WIDTH;

			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(320 - ADDON_WIDTH),
											CMatch::PitchYToScreenY(RIGHT_DOME_RENDER_Y_CENTRE + (ADDON_HEIGHT >> 1)),
											TheAddOnsGraphic,
											Region, 
											SPRITE_ROTATE_180);	// Right hand stars are rotated left hand stars
		}

		int starcount = 0;
		// Star bank
		ReplayBuffer[ReplayBufferIndex].StarBank[1].BitPattern = 0;
		if (RightBank) 
		{
			for (int i = 5; i--;)
			{
				// Check that an object exists.
				if (!RightBank->TheStarBank[i])
					return false;

				if (RightBank->TheStarBank[i]->OnOff)
				{
					ReplayBuffer[ReplayBufferIndex].StarBank[1].Owner = LeftBank->TheStarBank[i]->TeamControlling;
					ReplayBuffer[ReplayBufferIndex].StarBank[1].BitPattern |= (1 << i);

					Region.left		= ADDON_WIDTH * (BLUE_STAR_IMAGE_INDEX_OFFSET + RightBank->TheStarBank[i]->TeamControlling);
					Region.right	= Region.left + ADDON_WIDTH;

					GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(320 - ADDON_WIDTH),
													CMatch::PitchYToScreenY(RIGHT_STARBANK_SY - i * ADDON_HEIGHT),
													TheAddOnsGraphic,
													Region, 
													SPRITE_ROTATE_180);	// Right hand stars are rotated left hand stars

					starcount++;
				}

				if(starcount == 5)
				{
					PLAY_ALLSTARS;
				}
			}
		}
	}


	return true;
}

// ----
#define TOKEN_FRAME_WIDTH	16
#define TOKEN_FRAME_HEIGHT	16
#define	TOKEN_WIDTH_SHIFT	4
#define	TOKEN_HEIGHT_SHIFT	4
#define Y_TOKEN_PITCH		20

bool	CMatch::DrawTokens()
{
	RECT			Region;
	int				RenderX, RenderY;
	unsigned int	TokenFrame;
	
	if (!GraphicsObjectRef)
		return false;

	// Loop token array.
	for (int i = MAX_TOKENS; i--;)
	{
		if (!TokenArray[i])	// Skip null items.
		{
			ReplayBuffer[ReplayBufferIndex].Tokens[i].Frame = -1;	// Ignore in replay
			continue;
		}

		// Collect info
		RenderX		= CMatch::PitchXToScreenX(TokenArray[i]->getXPos() - (TOKEN_FRAME_WIDTH >> 1));
		RenderY		= CMatch::PitchYToScreenY(TokenArray[i]->getYPos() + (TOKEN_FRAME_HEIGHT >> 1));
		TokenFrame	= TokenArray[i]->GetTokenType() + TokenArray[i]->getAniFrame();
	
		// Store for replay
		ReplayBuffer[ReplayBufferIndex].Tokens[i].X = RenderX;
		ReplayBuffer[ReplayBufferIndex].Tokens[i].Y = RenderY;
		ReplayBuffer[ReplayBufferIndex].Tokens[i].Frame = TokenFrame;

		if (!IsActorOnScreen((CActor*)TokenArray[i], 16))
			continue;

		// Use
		Region.top		= (TokenFrame / Y_TOKEN_PITCH) << TOKEN_HEIGHT_SHIFT;
		Region.bottom	= Region.top + TOKEN_FRAME_HEIGHT;
		Region.left		= (TokenFrame % Y_TOKEN_PITCH) << TOKEN_WIDTH_SHIFT;
		Region.right	= Region.left + TOKEN_FRAME_WIDTH;
		GraphicsObjectRef->DrawSprite(	RenderX,
										RenderY,
										ThePickupGraphic, 
										Region);
	}
	return true;
}

// ----

bool	CMatch::DrawDPad()
{
	if (	(!GameDataRef)
		||	(!GraphicsObjectRef))
		return false;

	if (	(GameDataRef->GetGameType() == DEMO)
		||	(GameDataRef->GetGameType() == LEAGUE_TEAM_MANAGER))
		return true;

	if (	(GameDataRef->GetPreferences()->StylusControl == STYLUS_CONTROL_DPAD_R)
		||	(GameDataRef->GetPreferences()->StylusControl == STYLUS_CONTROL_DPAD_L))
	{
		if (!TheDPadGraphic)
		{
			NEW(TheDPadGraphic, CGraphicsData());
			SAFELY_CONTROL(if, TheDPadGraphic, Init(GraphicsObjectRef, &CString(DPAD_IMAGE), PlatformRef), ==, false)
				return false;
		}

		RECT Region;
		Region.top		= DPAD_TOP;
		Region.left		= DPAD_LEFT;
		Region.bottom	= Region.top + DPAD_HEIGHT;
		Region.right	= Region.left + DPAD_WIDTH;

		if (GameDataRef->GetPreferences()->StylusControl == STYLUS_CONTROL_DPAD_R)
		{
			GraphicsObjectRef->DrawSprite(	DPAD_POS_X_R, 
											DPAD_POS_Y, 
											TheDPadGraphic, 
											Region);
		}
		else
		{
			GraphicsObjectRef->DrawSprite(	DPAD_POS_X_L, 
											DPAD_POS_Y, 
											TheDPadGraphic, 
											Region);
		}
	}

	return true;
}

// ----
#define PLAYER_FRAME_WIDTH	32
#define PLAYER_FRAME_HEIGHT	32
#define	PLAYER_WIDTH_SHIFT	5
#define	PLAYER_HEIGHT_SHIFT	5
#define Y_PLAYER_PITCH		10

// 'Compare' function for qsort'ing the players by their y values.
int	OrderByY (const void *elem1, const void *elem2 )
{
	CTeamMember	*A	= *(CTeamMember**)elem1;
	CTeamMember	*B	= *(CTeamMember**)elem2;

	if (	(!elem1) 
		||	(!elem2))
	{
		return NULL;
	}
	else
	{
		
		int a = A->getYPos();
		//if (	(A->getState() == FLOORED) 
		//	||	(A->getState() == FALLING))
		//	a -= 1000;

		int b = B->getYPos();
		//if (	(B->getState() == FLOORED) 
		//	||	(B->getState() == FALLING))
		//	b -= 1000;

		return ((int)(a - b));
	}
}

bool	CMatch::DrawPlayers()
{
	CTeam			*TeamRef;
	CGraphicsData	*PlayerGraphics;
	RECT			Region;
	DWORD			State;
	CTeamMember		*SortedPlayerList[18];
	int				Index = 0;

	if (!GameDataRef)
		return false;

	// Okay, need to sort the players according to y value.
	// Firstly build an array of player references.
	for (int Side = 2; Side--;)
	{
		// Get handle to player graphics.
		if (Side)
			TeamRef = GameDataRef->GetActiveTeamByIndex(1);
		else
			TeamRef = GameDataRef->GetActiveTeamByIndex(0);
		
		// For all team members.
		for (int i = 9; i--;)
		{
			SortedPlayerList[Index] = TeamRef->GetTeamMember(i);
			Index++;
		}
	}

	// Now sort the list
	qsort((void*)SortedPlayerList, 18, sizeof(CTeamMember*), OrderByY);

	for (int i = 18; i--;)
	{
		ReplayBuffer[ReplayBufferIndex].Players[i].Frame = -1;

		CTeamMember *TMRef = SortedPlayerList[i];

		if (!TMRef)
			continue;
		else
		{
			if (GameHalf == FIRST_HALF)
			{
				if (TMRef->DirectionOfPlay == DOWN_MAP)
					PlayerGraphics	= TheRedPlayerGraphic;
				else
					PlayerGraphics	= TheBluePlayerGraphic;
			}
			else
			{
				if (TMRef->DirectionOfPlay == DOWN_MAP)
					PlayerGraphics	= TheBluePlayerGraphic;
				else
					PlayerGraphics	= TheRedPlayerGraphic;
			}
		}

		//if (	((MapPosX + PLAYER_FRAME_WIDTH) < 0)
		//	||	(MapPosX > (int)GraphicsObjectRef->GetDisplay()->cxWidth)
		//	||	((MapPosY + PLAYER_FRAME_HEIGHT) < 0)
		//	||	(MapPosY > (int)GraphicsObjectRef->GetDisplay()->cyHeight))
		//	continue;
		if (!IsActorOnScreen((CActor*)TMRef, PLAYER_FRAME_WIDTH))
			continue;

		if(GameDataRef->GetGameType() == PRACTICE)
		{
			if(TMRef->getTeam() != BRUTAL_DELUXE)
				continue;
		}

		int FacingDirection	= TMRef->GetDirectionFacing();

		if (	(FacingDirection < 0) 
			||	(FacingDirection > 7))
			continue;

		int RenderX			= CMatch::PitchXToScreenX(TMRef->getXPos()) - (PLAYER_FRAME_WIDTH	>> 1);	//= TMRef->getXPos() + ((TILE_WIDTH * PITCH_TILE_WIDTH - PLAYER_FRAME_WIDTH) >> 1) - CameraX;
		int RenderY			= CMatch::PitchYToScreenY(TMRef->getYPos()) - (PLAYER_FRAME_HEIGHT	>> 1);	//= -TMRef->getYpos() + ((TILE_HEIGHT * PITCH_TILE_HEIGHT - PLAYER_FRAME_HEIGHT) >> 1) - CameraY;
		int Frame			= TMRef->getAniFrame();
		State = TMRef->getState();


		// Special case for goalie
		if (TMRef->getPosition() == GOAL)
		{
			if (TMRef->DirectionOfPlay == UP_MAP)
			{
				if (State == RUNNING)
				{
					if(		(FacingDirection != NW)
						&&	(FacingDirection != NE)
						&&	(FacingDirection != N))
					{
						State = GOALWALK;
						
						if(Frame > 2)
						{
							Frame = 1;
						}
					
						Frame = Frame + 4;
					}
				}

				if(State == NONE)
				{
					if(		(FacingDirection != NW)
						&&	(FacingDirection != NE))
					{
						State = GOALWALK;
						
						Frame = 1 + 4;
					}
				}
			}
			else
			{
				if (State == RUNNING)
				{
					if(		(FacingDirection != S)	
						&&	(FacingDirection != SW)
						&&	(FacingDirection != SE))
					{
						State = GOALWALK;
						if(Frame > 2)
						{
							Frame = 1;
						}
						//Frame = Frame - 1;
					}
				}
				
				if(State == NONE)
				{
					if(		(FacingDirection != SW)
						&&	(FacingDirection != SE))
					{
						State = GOALWALK;
						
						Frame = 0;
					}
				}

			}
		} // End special goalie case

		switch (State)
		{
		case NONE:
			Frame += FacingDirection;
			break;
		case RUNNING:
			Frame = Frame >> 1;	
			Frame += 16 + 4 * FacingDirection;
			break;
		case FLOORED:
			Frame = 94;
			break;
		case SLIDING:
			Frame = 8 + FacingDirection;
			break;
		case JUMPING:
			// 0,1			-> 0
			// 2,3,4,5,6	-> 1
			// 7			-> 0.
			if ((Frame > 1) && (Frame < 9))
				Frame = 1;
			else
				Frame = 0;
			Frame += 72 + 2 * FacingDirection;
			break;
		case PUNCHING:
			Frame = Frame >> 1;
			Frame += 47 + 3 * FacingDirection;
			break;
		case FALLING:
			Frame = Frame >> 2; // that is divide by 4
			Frame += 89;
			break;
		case THROWING:
			Frame = Frame >> 1;	
			Frame += 48 + 3 * FacingDirection;
			break;
		case DANCING:
			Frame = (TMRef->DirectionOfPlay == UP_MAP)?96:112;
			Frame += ((TMRef->getAniFrame()) % 4) == 0;
			break;
		case KNEE_SLIDE:
			Frame = (TMRef->DirectionOfPlay == UP_MAP)?98:114;
			Frame += ((TMRef->getAniFrame()) % 4) == 0;
			break;
		case DIVING:
			Frame = (TMRef->DirectionOfPlay == UP_MAP)?124:120;
			if(TMRef->GetDirection() < 5)
				Frame += 2;

			if(TMRef->getAniFrame() > 2)
				Frame++;
			break;
		case CATCHING:
			Frame = 48 + (FacingDirection * 3);
			break;
		case GOALWALK:
			//OK this is jst to test
			TMRef->NotHoldBall = false;
			Frame += 100;
			break;
		default:
			return false;
		}
		//Hack attack here.	In order to avoid any complications with taking the body off
		//pitch I am going to set the frame all the time if thingA's state is SLIDING

		if(		(ThingA->getState() == SLIDING) )
		{
			/*
			&&	(TMRef->getCurrentStamina() <= 0) )
			*/
			CTeamMember* injury	= NULL;
			CTeamMember* temp	= NULL;

			for(int i = 0; i < 9; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				
				if(temp->getCurrentStamina() < 0)
				{
					injury = temp;
				}
			}

			for(i = 0; i < 9; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				
				if(temp->getCurrentStamina() < 0)
				{
					injury = temp;
				}
			}

			if(injury != NULL)
			{
				if(injury->getTeam() == TMRef->getTeam())
				{
					if(injury->getPosition() == TMRef->getPosition() )
					{
						Frame = 95;
					}
				}
			}
		}

		// Store
		ReplayBuffer[ReplayBufferIndex].Players[i].X = RenderX;
		ReplayBuffer[ReplayBufferIndex].Players[i].Y = RenderY;
		ReplayBuffer[ReplayBufferIndex].Players[i].Frame = Frame;
		if (GameHalf == FIRST_HALF)
			ReplayBuffer[ReplayBufferIndex].Players[i].Side	= (TMRef->DirectionOfPlay == DOWN_MAP);
		else
			ReplayBuffer[ReplayBufferIndex].Players[i].Side	= (TMRef->DirectionOfPlay == UP_MAP);		

		// Use
		Region.top		= (Frame / Y_PLAYER_PITCH) << PLAYER_HEIGHT_SHIFT;
		Region.bottom	= Region.top + PLAYER_FRAME_HEIGHT;
		Region.left		= (Frame % Y_PLAYER_PITCH) << PLAYER_WIDTH_SHIFT;
		Region.right	= Region.left + PLAYER_FRAME_WIDTH;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, PlayerGraphics, Region));

		//code added to draw the markers above the players head
		if(		((TimeCount >= 5)
			&&	(TimeCount <= 10))
			||	((TimeCount >= 15)
			&&	(TimeCount <= 20)) )
		{
			if(TMRef->IsControlled)
			{
				if(TMRef->HasPossesion)
				{
					switch(TMRef->getPosition())
					{
					case CFWD:
						Frame = 0;
						break;
					case LWNG:
					case RWNG:
						Frame = 1;
						break;
					case CMID:
					case LMID:
					case RMID:
						Frame = 2;
						break;
					case LDEF:
					case RDEF:
						Frame = 3;
						break;
					case GOAL:
						Frame = 4;
						break;
					default:
						Frame = 5;
						break;
					}
				}
				else
				{
					Frame = 5;	
				}

				if (GameHalf == FIRST_HALF)
				{
					if (TMRef->DirectionOfPlay == DOWN_MAP)
						Frame += 6;
				}
				else
				{
					if (TMRef->DirectionOfPlay != DOWN_MAP)
						Frame += 6; 
				}

				Region.top		= 0;
				Region.bottom	= 16;
				Region.left		= Frame * 16;
				Region.right	= (Frame * 16) + 16;

				SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX + 8, RenderY - 18, TheMarkerGraphic, Region));
			}
		}
	}
	return true;
}

bool	CMatch::DrawExtras()
{
	RECT			Region;
	DWORD			State;
	//int				Index = 0;

	if (!GameDataRef)
		return false;

	//FIRST RENDER THING A
	CExtra *TMRef = ThingA;

	if (!TMRef)
		return true;

	int FacingDirection	= TMRef->GetDirectionFacing();

	if (	(FacingDirection < 0) 
		||	(FacingDirection > 7))
		return true;

	int RenderX			= CMatch::PitchXToScreenX(TMRef->getXPos()) - (PLAYER_FRAME_WIDTH	>> 1);	//= TMRef->getXPos() + ((TILE_WIDTH * PITCH_TILE_WIDTH - PLAYER_FRAME_WIDTH) >> 1) - CameraX;
	int RenderY			= CMatch::PitchYToScreenY(TMRef->getYPos()) - (PLAYER_FRAME_HEIGHT	>> 1);	//= -TMRef->getYpos() + ((TILE_HEIGHT * PITCH_TILE_HEIGHT - PLAYER_FRAME_HEIGHT) >> 1) - CameraY;
	int Frame			= TMRef->getAniFrame();
	State = TMRef->getState();

	if(State == SLIDING)
	{
		
	}

	switch (State)
	{
	case NONE:
		Frame = FacingDirection;
		break;
	case RUNNING:
		Frame = FacingDirection;
		break;
	case SLIDING:
		Frame = 8;
		break;
	case FLOORED:
		return true;
	default:
		return false;
	}
	// Use
	Region.top		= (Frame / Y_PLAYER_PITCH) << PLAYER_HEIGHT_SHIFT;
	Region.bottom	= Region.top + PLAYER_FRAME_HEIGHT;
	Region.left		= (Frame % Y_PLAYER_PITCH) << PLAYER_WIDTH_SHIFT;
	Region.right	= Region.left + PLAYER_FRAME_WIDTH;

	SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TheMedicGraphic, Region));

	//THEN RENDER THINGB
	TMRef = ThingB;

	if (!TMRef)
		return false;

	FacingDirection	= TMRef->GetDirectionFacing();

	if (	(FacingDirection < 0) 
		||	(FacingDirection > 7))
		return false;

	RenderX			= CMatch::PitchXToScreenX(TMRef->getXPos()) - (PLAYER_FRAME_WIDTH	>> 1);	//= TMRef->getXPos() + ((TILE_WIDTH * PITCH_TILE_WIDTH - PLAYER_FRAME_WIDTH) >> 1) - CameraX;
	RenderY			= CMatch::PitchYToScreenY(TMRef->getYPos()) - (PLAYER_FRAME_HEIGHT	>> 1);	//= -TMRef->getYpos() + ((TILE_HEIGHT * PITCH_TILE_HEIGHT - PLAYER_FRAME_HEIGHT) >> 1) - CameraY;
	Frame			= TMRef->getAniFrame();
	State = TMRef->getState();

	if(State == SLIDING)
	{
		
	}

	switch (State)
	{
	case NONE:
		Frame = FacingDirection;
		break;
	case RUNNING:
		Frame = FacingDirection;
		break;
	case SLIDING:
		Frame = 9;
		break;
	default:
		return false;
	}
	// Use
	Region.top		= (Frame / Y_PLAYER_PITCH) << PLAYER_HEIGHT_SHIFT;
	Region.bottom	= Region.top + PLAYER_FRAME_HEIGHT;
	Region.left		= (Frame % Y_PLAYER_PITCH) << PLAYER_WIDTH_SHIFT;
	Region.right	= Region.left + PLAYER_FRAME_WIDTH;

	SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TheMedicGraphic, Region));
	return true;
}

// ----
#define BALL_FRAME_WIDTH	32
#define	Y_BALL_PITCH		10
bool	CMatch::DrawBall()
{
	RECT	Region;
	int		Frame;
	int		RenderX, RenderY;

	// Not visible, just return.
	if (!TheBall->GetVisible())
	{
		ReplayBuffer[ReplayBufferIndex].Ball.Frame = -1;	// Store invisible state
		return true;
	}

	DWORD State = TheBall->getState();

	switch (State)
	{
	case STOPPED:
	case ONFLOOR:
		Frame = 0;
		break;

	case LOW:
	case PREPARE_THROW:
		Frame = 1;
		break;

	case LAUNCHING:
		if (TheBall->AccelCount >= 40)
			Frame = 24;					// Hmmm, nice hack Anthony. (Frame 24 is entirely blank).
		else
		if (TheBall->AccelCount > 38)
			Frame = 2;
		else
		if (TheBall->AccelCount > 36)
			Frame = 3;
		else
		if (TheBall->AccelCount > 34)
			Frame = 4;
		else
		if (TheBall->AccelCount > 32)
			Frame = 5;
		else
		if (TheBall->AccelCount > 29) 
			Frame = 6;
		else
		if (TheBall->AccelCount > 26)	// This case was broken! Had the condition ((val > 25) && (val <= 17))
			Frame = 7;
		else
		if (TheBall->AccelCount > 22)
			Frame = 8;
		else
		if (TheBall->AccelCount > 20) 
			Frame = 7;
		else
		if (TheBall->AccelCount > 17)
			Frame = 6;
		else
		if (TheBall->AccelCount > 14)
			Frame = 5;
		else
		if (TheBall->AccelCount > 11) 
			Frame = 4;
		else
		if (TheBall->AccelCount > 8) 
			Frame = 3;
		else
		if (TheBall->AccelCount > 5)
			Frame = 2;
		else
		if (TheBall->AccelCount > 2)
			Frame = 1;
		else
		if (TheBall->AccelCount > -1) 
			Frame = 0;

		break;

	case HIGH:
		Frame = 1 + 3 * TheBall->GetHeight(); // 1 + 3*1

		if (TheBall->AccelCount < 0)
			Frame = 0;
		else
		if (TheBall->AccelCount < 3)
			Frame += TheBall->AccelCount;
		else
		if (TheBall->AccelCount < 18)
			Frame += ((TheBall->AccelCount - 2) / 5); // Relies on integer maths  the number by accelcount was 4
		else
			Frame += TheBall->AccelCount - 17;

		break;
	case HELD:
		if(TheBall->GetHeight() != 2)
			Frame = 1;
		else
			Frame = 6;
		break;
	case IN_CHUTE:
		Frame = TheBall->ChuteFrame;
		break;
	default:
		return false;		// Welcome to the "red screen of death"...
		break;
	}

	if (TheBall->IsBallBurning())//getAniFrame();
		Frame += 12;

	Region.top		= (Frame / Y_BALL_PITCH) * BALL_FRAME_WIDTH;
	Region.bottom	= Region.top + BALL_FRAME_WIDTH;
	Region.left		= (Frame % Y_BALL_PITCH) * BALL_FRAME_WIDTH;
	Region.right	 = Region.left + BALL_FRAME_WIDTH;

	if(TheBall->IsBallHeld() == false)
	{
		// Collect
		RenderX = CMatch::PitchXToScreenX(TheBall->getXPos()) - (BALL_FRAME_WIDTH >> 1);
		RenderY = CMatch::PitchYToScreenY(TheBall->getYPos()) - (BALL_FRAME_WIDTH >> 1);

		// Store
		ReplayBuffer[ReplayBufferIndex].Ball.X = RenderX;
		ReplayBuffer[ReplayBufferIndex].Ball.Y = RenderY;
		ReplayBuffer[ReplayBufferIndex].Ball.Frame = Frame;

		//Use
		SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TheBallGraphic, Region));
	}
	else
	{
		int posX = 0;
		int posY = 0;
		switch(TheBall->heldBy->GetDirectionFacing())
		{
			//NORTH*********************************************************************
		case N:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = 12;
				posY = -8;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = 12;
					posY = -12;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = 12;
					posY = -8;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 12;
					posY = 8;
					break;
				}

				posX = 12;
				posY = -8;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = 12;
				posY = 10;
				break;
			case JUMPING:
				posX = 12;
				posY = 8;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//SOUTH**************************************************************
		case S:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = -12;
				posY = -8;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -12;
					posY = 8;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = -12;
					posY = -8;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = -12;
					posY = -12;
					break;
				}

				posX = -12;
				posY = -8;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = -12;
				posY = 10;
				break;
			case JUMPING:
				posX = -12;
				posY = 8;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//EAST****************************************************************************
		case E:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = 2;
				posY = -14;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -10;
					posY = -14;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = 2;
					posY = -14;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 10;
					posY = -14;
					break;
				}

				posX = 2;
				posY = -14;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = -12;
				posY = 10;
				break;
			case JUMPING:
				posX = 0;
				posY = 8;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//WEST*********************************************************************************
		case W:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = -2;
				posY = -14;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -10;
					posY = -14;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = -2;
					posY = -14;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 10;
					posY = -14;
					break;
				}

				posX = -2;
				posY = -14;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = 12;
				posY = 10;
				break;
			case JUMPING:
				posX = 0;
				posY = 8;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//NORTH EAST***************************************************************************
		case NE:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = 6;
				posY = -16;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -2;
					posY = -18;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = 6;
					posY = -16;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 12;
					posY = -4;
					break;
				}

				posX = 6;
				posY = -16;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = 4;
				posY = 8;
				break;
			case JUMPING:
				posX = 8;
				posY = 4;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//SOUTH WEST***************************************************************************
		case SW:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = 6;
				posY = -16;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -2;
					posY = -18;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = 6;
					posY = -16;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 12;
					posY = -4;
					break;
				}

				posX = 6;
				posY = -16;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = 10;
				posY = 8;
				break;
			case JUMPING:
				posX = 8;
				posY = 4;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//NORTH WEST*****************************************************************************
		case NW:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = -8;
				posY = -16;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -12;
					posY = -4;	
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = -8;
					posY = -16;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 2;
					posY = -18;
					break;
				}

				posX = -8;
				posY = -16;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = 10;
				posY = 8;
				break;
			case JUMPING:
				posX = -8;
				posY = 4;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
			//SOUTH EAST******************************************************************************
		case SE:
			switch(TheBall->heldBy->getState())
			{
			case NONE:
				posX = -8;
				posY = -16;
				break;
			case RUNNING:
				if(TheBall->heldBy->getAniFrame() < 2)
				{
					posX = -12;
					posY = -4;	
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 4)
				{
					posX = -8;
					posY = -16;
					break;
				}

				if(TheBall->heldBy->getAniFrame() < 6)
				{
					posX = 2;
					posY = -18;
					break;
				}

				posX = -8;
				posY = -16;
				break;
			case THROWING:
				break;
			case CATCHING:
				posX = -10;
				posY = 8;
				break;
			case JUMPING:
				posX = -8;
				posY = 4;
				break;
			default:
				posX = 0;
				posY = 0;
				break;
			}
			break;
		}

		if(TheBall->heldBy->getPosition() == GOAL)
		{
			if(	(TheBall->heldBy->getState() == NONE))
			{
				if(TheBall->heldBy->DirectionOfPlay == UP_MAP)
				{
					if(		(TheBall->heldBy->GetDirectionFacing() != NW)
						&&	(TheBall->heldBy->GetDirectionFacing() != NE) )
					{
						posX = 10;
						posY = 8;
					}
				}
				else
				{
					if(		(TheBall->heldBy->GetDirectionFacing() != SW)
						&&	(TheBall->heldBy->GetDirectionFacing() != SE) )
					{
						posX = -10;
						posY = -8;
					}
				}
			}

			if(	(TheBall->heldBy->getState() == RUNNING))
			{
				if(TheBall->heldBy->DirectionOfPlay == UP_MAP)
				{
					if(		(TheBall->heldBy->GetDirectionFacing() != NW)
						&&	(TheBall->heldBy->GetDirectionFacing() != NE) 
						&&	(TheBall->heldBy->GetDirectionFacing() != N))
					{
						posX = 10;
						posY = 8;
					}
				}
				else
				{
					if(		(TheBall->heldBy->GetDirectionFacing() != SW)
						&&	(TheBall->heldBy->GetDirectionFacing() != SE) 
						&&	(TheBall->heldBy->GetDirectionFacing() != S))
					{
						posX = -10;
						posY = -8;
					}
				}
			}
		}

		// Collect
		RenderX = CMatch::PitchXToScreenX((TheBall->getXPos() + posX)) - (BALL_FRAME_WIDTH >> 1);
		RenderY = CMatch::PitchYToScreenY((TheBall->getYPos() + posY)) - (BALL_FRAME_WIDTH >> 1);

		// Store
		ReplayBuffer[ReplayBufferIndex].Ball.X = RenderX;
		ReplayBuffer[ReplayBufferIndex].Ball.Y = RenderY;
		ReplayBuffer[ReplayBufferIndex].Ball.Frame = Frame;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TheBallGraphic, Region));
	}
	return true;
}


// ----
#define	POPUP_X						8
#define	POPUP_Y						8
#define	POPUP_WIDTH					112
#define	POPUP_HEIGHT				72

#define	POPUP_GOAL_IMAGE_INDEX		0
#define	POPUP_FLOORED_IMAGE_INDEX	1
#define	POPUP_LOGO_IMAGE_INDEX		2

#define	POPUP_TEXT_LINE1			((POPUP_HEIGHT * 1) / 7)
#define	POPUP_TEXT_LINE2			((POPUP_HEIGHT * 3) / 7)
#define	POPUP_TEXT_LINE3			((POPUP_HEIGHT * 5) / 7)
#define	POPUP_TEXT_LINE4			((POPUP_HEIGHT * 2) / 7)
#define	POPUP_TEXT_LINE5			((POPUP_HEIGHT * 4) / 7)
#define	POPUP_TEXT_LINE6			((POPUP_HEIGHT * 6) / 7)

#define	POPUP_VS_TEXT				"Vs"
#define POPUP_TO_TEXT				"TO"

bool	CMatch::DrawPopUp()
{
	if (!GraphicsObjectRef)
		return false;

	RECT	Region;
	CString	TempString;
	char	convert[24];

//	if (!ThePopUpGraphic)
//	{
//		NEW(ThePopUpGraphic, CGraphicsData());
//		SAFELY_CONTROL(if, ThePopUpGraphic, Init(GraphicsObjectRef, &CString(POPUP_IMAGE), PlatformRef), ==, false)
//			return false;
//	}

	switch (MatchState)
	{
	case StartingMatch:
		if(		(TimePlayed < (GAME_PER_SIDE_DURATION + 2))
			&&	(TimePlayed > (GAME_PER_SIDE_DURATION - 1)))
		{
			Region.top		= 0;
			Region.bottom	= Region.top + POPUP_HEIGHT;
			Region.left		= POPUP_LOGO_IMAGE_INDEX * POPUP_WIDTH;
			Region.right	= Region.left + POPUP_WIDTH;

			// Logo
			GraphicsObjectRef->DrawSprite(	POPUP_X,
											POPUP_Y,
											ThePopUpGraphic,
											Region);

			if(GameHalf == FIRST_HALF)
			{
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&CString("First Half"),
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE1,
												TEXT_CENTRED);
			}
			else
			{
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&CString("Second Half"),
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE1,
												TEXT_CENTRED);
			}
			// Player one team
			TempString = GameDataRef->GetActiveTeamByIndex(0)->GetShortTeamName();
			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&TempString,
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE4 + 5,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&CString(POPUP_VS_TEXT),
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE2 + 5,
											TEXT_CENTRED);

			if(GameDataRef->GetGameType() != PRACTICE)
			{
				TempString = GameDataRef->GetActiveTeamByIndex(1)->GetShortTeamName();
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE5 + 5,
												TEXT_CENTRED);
			}

			sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
			TempString = " ";
			TempString += convert;

			sprintf(convert, " To ");
			TempString += convert;


			//TempString = convert;
			sprintf(convert, "%03d ", GameDataRef->GetActiveTeamByIndex(1)->Score);
			TempString += convert;


			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&TempString,
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE3 + 10,
											TEXT_CENTRED);

		}
		break;
	case LaunchingBall:
			Region.top		= 0;
			Region.bottom	= Region.top + POPUP_HEIGHT;
			Region.left		= POPUP_LOGO_IMAGE_INDEX * POPUP_WIDTH;
			Region.right	= Region.left + POPUP_WIDTH;

			// Logo
			GraphicsObjectRef->DrawSprite(	POPUP_X,
											POPUP_Y,
											ThePopUpGraphic,
											Region);

			// Player one team
			TempString = GameDataRef->GetActiveTeamByIndex(0)->GetShortTeamName();
			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&TempString,
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE1,
											TEXT_CENTRED);

			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&CString(POPUP_VS_TEXT),
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE2 ,
											TEXT_CENTRED);

			if(GameDataRef->GetGameType() != PRACTICE)
			{
				TempString = GameDataRef->GetActiveTeamByIndex(1)->GetShortTeamName();
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE3,
												TEXT_CENTRED);
			}
/*
			sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
			TempString = " ";
			TempString += convert;

			sprintf(convert, " To ");
			TempString += convert;


			//TempString = convert;
			sprintf(convert, "%03d ", GameDataRef->GetActiveTeamByIndex(1)->Score);
			TempString += convert;


			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&TempString,
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE3 + 10,
											TEXT_CENTRED);
											*/
			break;
		//**************************************************************
	case MatchOver:

		// Show score board
		Region.top		= 0;
		Region.bottom	= Region.top + POPUP_HEIGHT;
		Region.left		= POPUP_LOGO_IMAGE_INDEX * POPUP_WIDTH;
		Region.right	= Region.left + POPUP_WIDTH;

		// Logo
		GraphicsObjectRef->DrawSprite(	POPUP_X,
										POPUP_Y,
										ThePopUpGraphic,
										Region);

		// Player one team
		TempString = GameDataRef->GetActiveTeamByIndex(0)->GetShortTeamName();
		sprintf(convert, " %03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
		TempString += convert;
		GraphicsObjectRef->DrawString(	ThePopUpFont,
										&TempString,
										POPUP_X + (POPUP_WIDTH >> 1),
										POPUP_Y + POPUP_TEXT_LINE1,
										TEXT_CENTRED);

		// Vs
		GraphicsObjectRef->DrawString(	ThePopUpFont,
										&CString(POPUP_VS_TEXT),
										POPUP_X + (POPUP_WIDTH >> 1),
										POPUP_Y + POPUP_TEXT_LINE2,
										TEXT_CENTRED);

		if(GameDataRef->GetGameType() != PRACTICE)
		{
			// Opponents
			TempString = GameDataRef->GetActiveTeamByIndex(1)->GetShortTeamName();
			sprintf(convert, " %03d", GameDataRef->GetActiveTeamByIndex(1)->Score);
			TempString += convert;
			GraphicsObjectRef->DrawString(	ThePopUpFont,
											&TempString,
											POPUP_X + (POPUP_WIDTH >> 1),
											POPUP_Y + POPUP_TEXT_LINE3,
											TEXT_CENTRED);
		}


		break;

	case DoingGoalSideA:
	case DoingGoalSideB:
	case DoingHomeGoalSideA:
	case DoingHomeGoalSideB:
		// Not sure what to do...
		Region.top		= 0;
		Region.bottom	= Region.top + POPUP_HEIGHT;
		Region.left		= POPUP_GOAL_IMAGE_INDEX * POPUP_WIDTH;
		Region.right	= Region.left + POPUP_WIDTH;

		// Logo
		GraphicsObjectRef->DrawSprite(	POPUP_X,
										POPUP_Y,
										ThePopUpGraphic,
										Region);

		break;

	case DoingInjury:
		// Show picture of injury
		if(InjuryCount < 75)
		{
			Region.top		= 0;
			Region.bottom	= Region.top + POPUP_HEIGHT;
			Region.left		= POPUP_FLOORED_IMAGE_INDEX * POPUP_WIDTH;
			Region.right	= Region.left + POPUP_WIDTH;

			// Logo
			GraphicsObjectRef->DrawSprite(	POPUP_X,
											POPUP_Y,
											ThePopUpGraphic,
											Region);
		}
		else
		{
			Region.top		= 0;
			Region.bottom	= Region.top + POPUP_HEIGHT;
			Region.left		= POPUP_LOGO_IMAGE_INDEX * POPUP_WIDTH;
			Region.right	= Region.left + POPUP_WIDTH;

			// Logo
			GraphicsObjectRef->DrawSprite(	POPUP_X,
											POPUP_Y,
											ThePopUpGraphic,
											Region);

			CTeamMember* injury	= NULL;
			CTeamMember* temp	= NULL;
			

			for(int i = 0; i < 9; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
				
				if(temp->getCurrentStamina() < 0)
				{
					injury = temp;
				}
			}

			for(i = 0; i < 9; i++)
			{
				temp = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
				
				if(temp->getCurrentStamina() < 0)
				{
					injury = temp;
				}
			}

			if(injury)
			{
				switch(injury->getPosition())
				{
				case CFWD:
					TempString = "Centre";
					break;
				case LWNG:
					TempString = "Left";
					break;
				case RWNG:
					TempString = "Right";
					break;
				case CMID:
					TempString = "Centre";
					break;
				case RMID:
					TempString = "Right";
					break;
				case LMID:
					TempString = "Left";
					break;
				case RDEF:
					TempString = "Right";
					break;
				case LDEF:
					TempString = "Left";
					break;
				case GOAL:
					TempString = "";
					break;
				default:
					TempString = "SOMEONE";
					break;
				}
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE1 + 10,
												TEXT_CENTRED);
				switch(injury->getPosition())
				{
				case CFWD:
					TempString = "Forward";
					break;
				case LWNG:
					TempString = "Winger";
					break;
				case RWNG:
					TempString = "Winger";
					break;
				case CMID:
					TempString = "Midfield";
					break;
				case RMID:
					TempString = "Midfield";
					break;
				case LMID:
					TempString = "Midfield";
					break;
				case RDEF:
					TempString = "Defence";
					break;
				case LDEF:
					TempString = "Defence";
					break;
				case GOAL:
					TempString = "Goalie";
					break;
				default:
					TempString = "SOMEONE";
					break;
				}

				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE2,
												TEXT_CENTRED);
				TempString = "Injured";
				
				GraphicsObjectRef->DrawString(	ThePopUpFont,
												&TempString,
												POPUP_X + (POPUP_WIDTH >> 1),
												POPUP_Y + POPUP_TEXT_LINE3 - 10,
												TEXT_CENTRED);
			}	
		}	
		break;
	default:
		return true;
	}


	return true;
}


// ----
#define TIME_REMAINING_OFFSET		4
#define SCORE_OFFSET				6
#define INFOBAR_ACTIVE_WIDTH		12
#define	INFOBAR_ACTIVE_HEIGHT		26
#define LEFT_INFOBAR_ACTIVE_POS		72
#define RIGHT_INFOBAR_ACTIVE_POS	132

#define INFOBAR_STAT_LINE_OFFSET	13
#define	INFOBAR_STAT_LINE_HEIGHT	5
#define	LEFT_STAT_POS				48
#define	RIGHT_STAT_POS				170
#define	STAT_LINE_WIDTH				22
bool	CMatch::DrawInfoBar()
{
	RECT		Region;
	CTeamMember	*PlayerRef	= NULL;
	int i;

	int xpos = (GraphicsObjectRef->GetDisplay()->cxWidth - INFOBAR_WIDTH) >> 1;
	int	xcentre	= GraphicsObjectRef->GetDisplay()->cxWidth >> 1;
	if (GraphicsObjectRef)
	{
		// Main bar.
		Region.top		= 0;
		Region.bottom	= INFOBAR_HEIGHT;
		Region.left		= 0;
		Region.right	 = INFOBAR_WIDTH;
		GraphicsObjectRef->DrawSprite(	xpos,
										GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT, 
										TheInfoBarGraphic, 
										Region);


		// Active player - team A
		for (i = 9; i--;)
		{
			PlayerRef = GameDataRef->GetActiveTeamByIndex(0)->GetTeamMember(i);
			if ((PlayerRef) && (PlayerRef->IsControlled))
			{
				// Stat bar
				Region.top		= GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT + INFOBAR_STAT_LINE_OFFSET;
				Region.bottom	= Region.top + INFOBAR_STAT_LINE_HEIGHT;

				Region.left		= xcentre - (INFOBAR_WIDTH >> 1) + LEFT_STAT_POS;
				float p = (float)PlayerRef->getCurrentStamina() / (float)PlayerRef->getSta();
				if (p > 1.0f)
					p = 1.0f;
				Region.right	= Region.left + p * STAT_LINE_WIDTH;
				GraphicsObjectRef->DrawRect(&Region, 0, 0, 255);

				// One of those three lights on InfoBar
				Region.top		= 0 + INFOBAR_HEIGHT;
				Region.bottom	= Region.top + INFOBAR_ACTIVE_HEIGHT;
				Region.left		= (PlayerRef->getPosition() / 3) * INFOBAR_ACTIVE_WIDTH;
				Region.right	= Region.left + INFOBAR_ACTIVE_WIDTH;
				GraphicsObjectRef->DrawSprite(	xcentre - (INFOBAR_WIDTH >> 1) + LEFT_INFOBAR_ACTIVE_POS + (PlayerRef->getPosition() / 3) * INFOBAR_ACTIVE_WIDTH,
												GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT, 
												TheInfoBarGraphic, 
												Region);
				break;
			}
		}

		// Active player - team B
		for (i = 9; i--;)
		{
			PlayerRef = GameDataRef->GetActiveTeamByIndex(1)->GetTeamMember(i);
			if ((PlayerRef) && (PlayerRef->IsControlled))
			{
				// Stat bar
				Region.top		= GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT + INFOBAR_STAT_LINE_OFFSET;
				Region.bottom	= Region.top + INFOBAR_STAT_LINE_HEIGHT;

				Region.left		= xcentre - (INFOBAR_WIDTH >> 1) + RIGHT_STAT_POS;
				float p = (float)PlayerRef->getCurrentStamina() / (float)PlayerRef->getSta();
				if (p > 1.0f)
					p = 1.0f;
				Region.right	= Region.left + p * STAT_LINE_WIDTH;
				GraphicsObjectRef->DrawRect(&Region, 255, 0, 0);

				// Lights
				Region.top		= 0 + INFOBAR_HEIGHT;
				Region.bottom	= Region.top + INFOBAR_ACTIVE_HEIGHT;
				Region.left		= (PlayerRef->getPosition() / 3) * INFOBAR_ACTIVE_WIDTH;
				Region.right	= Region.left + INFOBAR_ACTIVE_WIDTH;
				GraphicsObjectRef->DrawSprite(	xcentre - (INFOBAR_WIDTH >> 1) + RIGHT_INFOBAR_ACTIVE_POS + (2 - (PlayerRef->getPosition() / 3)) * INFOBAR_ACTIVE_WIDTH,
												GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT, 
												TheInfoBarGraphic, 
												Region, 
												SPRITE_FLIP_HORIZONTAL);	// Flip image horizontally.
				break;
			}
		}

		// Render time remaining.
		char	convert[8];
		sprintf(convert, "%02d", int(GAME_PER_SIDE_DURATION - (int)TimePlayed));
		GraphicsObjectRef->DrawString(	TheInfoBarFont, 
										&CString(convert), 
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1,
										GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT + TIME_REMAINING_OFFSET,
										TEXT_CENTRED);

		// Score for side A
		sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(0)->Score);
		GraphicsObjectRef->DrawString(	TheInfoBarFont, 
										&CString(convert), 
										(GraphicsObjectRef->GetDisplay()->cxWidth >> 1) - 120 + 13,
										GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT + SCORE_OFFSET,
										TEXT_LEFT);

		// Side B
		sprintf(convert, "%03d", GameDataRef->GetActiveTeamByIndex(1)->Score);
		GraphicsObjectRef->DrawString(	TheInfoBarFont, 
										&CString(convert), 
										(GraphicsObjectRef->GetDisplay()->cxWidth >> 1) + 90,
										GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT + SCORE_OFFSET,
										TEXT_LEFT);


#ifndef	BOXEDSALE
		if (!GameDataRef->GetPreferences()->Registered)
		{
			GraphicsObjectRef->DrawString(	ThePopUpFont, 
											&CString("Unregistered time limited demo"), 
											(GraphicsObjectRef->GetDisplay()->cxWidth >> 1),
											3,
											TEXT_CENTRED);
		}
#endif

#ifdef DEMO_BUILD
		GraphicsObjectRef->DrawString(	ThePopUpFont, 
										&CString("Speedball2 Rolling Demo"), 
										(GraphicsObjectRef->GetDisplay()->cxWidth >> 1),
										3,
										TEXT_CENTRED);

		GraphicsObjectRef->DrawString(	ThePopUpFont, 
										&CString("Press fire to exit"), 
										(GraphicsObjectRef->GetDisplay()->cxWidth >> 1),
										GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_HEIGHT - 10,
										TEXT_CENTRED);

#endif
	
	}
	return true;
}

// ----
// Draws all of the above in replay mode.
bool	CMatch::DrawReplay()
{
	bool			ret = true;
	RECT			Region;
	int				XStart, XRenderPos, YRenderPos;
	int				RenderX, RenderY, Frame;
	CGraphicsData	*TempGraphics;

	if (!GraphicsObjectRef)
		return false;
	
	// Don't render if on even frame
	if (ReplayTime % 2)
	{
#ifndef LIMIT_FRAME_RATE
		Sleep(1000 * FRAME_LIMIT);
#endif
		return true;
	}

	// Lets get ready.
	SAFELY_CALL(GraphicsObjectRef, StartDraw());

	ReplayBufferPlaybackIndex++;
	if (ReplayBufferPlaybackIndex >= REPLAY_BUFFER_LENGTH)
		ReplayBufferPlaybackIndex = 0;

	// Store camera
	CameraX = ReplayBuffer[ReplayBufferPlaybackIndex].CameraX;
	CameraY = ReplayBuffer[ReplayBufferPlaybackIndex].CameraY;

	XRenderPos = -(CameraX % TILE_WIDTH);
	YRenderPos = -(CameraY % TILE_HEIGHT);

	// Calculate X, Y screen coords here.
	XStart = XRenderPos;

	int LowerHeight, UpperHeight, LowerWidth, UpperWidth;
	LowerWidth = (CameraX / TILE_WIDTH);
	LowerHeight = (CameraY / TILE_HEIGHT);

	if (LowerWidth < 0)
		LowerWidth = 0;
	if (LowerWidth > PITCH_TILE_WIDTH)
		LowerWidth = PITCH_TILE_WIDTH;
	if (LowerHeight < 0)
		LowerHeight = 0;
	if (LowerHeight > PITCH_TILE_HEIGHT)
		LowerHeight = PITCH_TILE_HEIGHT;

	UpperWidth = LowerWidth + (GraphicsObjectRef->GetDisplay()->cxWidth / TILE_WIDTH) + 1;
	UpperHeight = LowerHeight + ((GraphicsObjectRef->GetDisplay()->cyHeight - INFOBAR_PITCH_CLIP_HEIGHT) / TILE_HEIGHT) + 1;

	if (UpperWidth > PITCH_TILE_WIDTH)
		UpperWidth = PITCH_TILE_WIDTH;
	if (UpperHeight > PITCH_TILE_HEIGHT)
		UpperHeight = PITCH_TILE_HEIGHT;

	int	TileID, CachedTileID, Flipper;
	CachedTileID = -1;
	for (int y = LowerHeight; y < UpperHeight; y++)
	{
		for (int x = LowerWidth; x < UpperWidth; x++)
		{
			TileID = (PitchMap[y][x]);
			Flipper = TileID & 0xF000;	// Top four bits store flipping info,
			TileID &= 0x0FFF;			// Bottom 12 contain tile number.

			if (TileID != CachedTileID) // This is a worth will time saving at the moment. Check when complete.
			{
				// Calculate co-ordinates for the given tile.
				Region.top = (TileID / Y_TILE_PITCH) << TILE_HEIGHT_SHIFT;
				Region.bottom = Region.top + TILE_HEIGHT;
				Region.left = (TileID % Y_TILE_PITCH) << TILE_WIDTH_SHIFT;
				Region.right = Region.left + TILE_WIDTH;

				CachedTileID	= TileID;
			}
			
			SAFELY_CALL(GraphicsObjectRef, DrawNonTransSprite(XRenderPos, YRenderPos, ThePitchGraphic, Region, Flipper));
			XRenderPos += TILE_WIDTH;
		}

		YRenderPos += TILE_HEIGHT;
		XRenderPos = XStart;
	}

	// ----
	// Top shroom
	if (	(ReplayBuffer[ReplayBufferPlaybackIndex].PitchItemsBitPattern & TOP_SHROOM_BIT_MASK)
		//&&	(CameraY < (CMatch::PitchYToScreenY(TSHROOM_Y) + ADDON_HEIGHT))
		)
	{
		// Top shroom is visible
		Region.top		= ((SHROOM_IMAGE_INDEX_OFFSET / Y_ADDON_PITCH) << ADDON_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + ADDON_HEIGHT);
		Region.left		= ((SHROOM_IMAGE_INDEX_OFFSET % Y_ADDON_PITCH) << ADDON_WIDTH_SHIFT);
		Region.right	= (Region.left + ADDON_WIDTH);
		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-(ADDON_WIDTH >> 1)),
										CMatch::PitchYToScreenY(TSHROOM_Y + (ADDON_HEIGHT >> 1)), 
										TheAddOnsGraphic, 
										Region);
	}

	// Bottom shroom
	if (	(ReplayBuffer[ReplayBufferPlaybackIndex].PitchItemsBitPattern & BOTTOM_SHROOM_BIT_MASK)
		//&&	(int(CameraY + GraphicsObjectRef->GetDisplay()->cyHeight) > CMatch::PitchYToScreenY(BSHROOM_Y))
		)
	{
		// Bottom shroom is visible
		Region.top		= ((SHROOM_IMAGE_INDEX_OFFSET / Y_ADDON_PITCH) << ADDON_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + ADDON_HEIGHT);
		Region.left		= ((SHROOM_IMAGE_INDEX_OFFSET % Y_ADDON_PITCH) << ADDON_WIDTH_SHIFT);
		Region.right	= (Region.left + ADDON_WIDTH);
		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-(ADDON_WIDTH >> 1)),
										CMatch::PitchYToScreenY(BSHROOM_Y + (ADDON_HEIGHT >> 1)), 
										TheAddOnsGraphic, 
										Region);
	}

	int ChuteState = (ReplayBuffer[ReplayBufferPlaybackIndex].PitchItemsBitPattern & CHUTE_BIT_MASK) >> 4;
	if (ChuteState > 0)
	{
		ChuteState--;
		RECT Region;
		Region.top		= ((ChuteState / CHUTE_ADDON_PITCH) << CHUTE_HEIGHT_SHIFT);
		Region.bottom	= (Region.top + CHUTE_HEIGHT);
		Region.left		= ((ChuteState % CHUTE_ADDON_PITCH) << CHUTE_WIDTH_SHIFT);
		Region.right	= (Region.left + CHUTE_WIDTH);

		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(LEFT_CHUTE_RENDER_X),
										CMatch::PitchYToScreenY(LEFT_CHUTE_RENDER_Y), 
										TheChuteGraphic, 
										Region);
	
		GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(RIGHT_CHUTE_RENDER_X),
										CMatch::PitchYToScreenY(RIGHT_CHUTE_RENDER_Y), 
										TheChuteGraphic, 
										Region,
										SPRITE_ROTATE_180);
	}


	// ----
	// Left wall items
	if (CameraX < ADDON_WIDTH)
	{
		RECT Region;
		
		Region.top		= 0;
		Region.bottom	= Region.top + ADDON_HEIGHT;

		// Dome / light thing
		if (ReplayBuffer[ReplayBufferPlaybackIndex].PitchItemsBitPattern & LEFT_DOME_BIT_MASK)
		{
			Region.left		= ADDON_WIDTH * DOME_IMAGE_INDEX_OFFSET;
			Region.right	= Region.left + ADDON_WIDTH;
			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-320),
											CMatch::PitchYToScreenY(LEFT_DOME_RENDER_Y_CENTRE + (ADDON_HEIGHT >> 1)),
											TheAddOnsGraphic,
											Region);
		}

		for (int i = 5; i--;)
		{
			if (ReplayBuffer[ReplayBufferPlaybackIndex].StarBank[0].BitPattern & (1 << i))
			{
				Region.left		= ADDON_WIDTH * (BLUE_STAR_IMAGE_INDEX_OFFSET + ReplayBuffer[ReplayBufferPlaybackIndex].StarBank[0].Owner);	
				Region.right	= Region.left + ADDON_WIDTH;
				GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(-320),
												CMatch::PitchYToScreenY(LEFT_STARBANK_SY + i * ADDON_HEIGHT + ADDON_HEIGHT),
												TheAddOnsGraphic,
												Region);
			}
		}
	}

	// RIGHT!!!
	// If the right bank is on screen.
	if ((CameraX + GraphicsObjectRef->GetDisplay()->cxWidth) > (PITCH_TILE_WIDTH * TILE_WIDTH - ADDON_WIDTH))
	{
		Region.top		= 0;
		Region.bottom	= Region.top + ADDON_HEIGHT;

		// Dome / light thing
		if (ReplayBuffer[ReplayBufferPlaybackIndex].PitchItemsBitPattern & RIGHT_DOME_BIT_MASK)
		{
			Region.left		= ADDON_WIDTH * DOME_IMAGE_INDEX_OFFSET;
			Region.right	= Region.left + ADDON_WIDTH;
			GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(320 - ADDON_WIDTH),
											CMatch::PitchYToScreenY(RIGHT_DOME_RENDER_Y_CENTRE + (ADDON_HEIGHT >> 1)),
											TheAddOnsGraphic,
											Region, 
											SPRITE_ROTATE_180);	// Right hand stars are rotated left hand stars
		}

		// Star bank
		for (int i = 5; i--;)
		{
			if (ReplayBuffer[ReplayBufferPlaybackIndex].StarBank[1].BitPattern & (1 << i))
			{
				Region.left		= ADDON_WIDTH * (BLUE_STAR_IMAGE_INDEX_OFFSET + ReplayBuffer[ReplayBufferPlaybackIndex].StarBank[1].Owner);
				Region.right	= Region.left + ADDON_WIDTH;
				GraphicsObjectRef->DrawSprite(	CMatch::PitchXToScreenX(320 - ADDON_WIDTH),
												CMatch::PitchYToScreenY(RIGHT_STARBANK_SY - i * ADDON_HEIGHT),
												TheAddOnsGraphic,
												Region, 
												SPRITE_ROTATE_180);	// Right hand stars are rotated left hand stars
			}
		}
	}

	// ----
	// Tokens
	for (int i = MAX_TOKENS; i--;)
	{
		if (ReplayBuffer[ReplayBufferPlaybackIndex].Tokens[i].Frame == -1)	// Skip null items.
		{
			continue;
		}

		RenderX	= ReplayBuffer[ReplayBufferPlaybackIndex].Tokens[i].X;
		RenderY	= ReplayBuffer[ReplayBufferPlaybackIndex].Tokens[i].Y;
		Frame	= ReplayBuffer[ReplayBufferPlaybackIndex].Tokens[i].Frame;

		if (!IsActorOnScreen(RenderX, RenderY, 16))
			continue;

		Region.top		= (Frame / Y_TOKEN_PITCH) << TOKEN_HEIGHT_SHIFT;
		Region.bottom	= Region.top + TOKEN_FRAME_HEIGHT;
		Region.left		= (Frame % Y_TOKEN_PITCH) << TOKEN_WIDTH_SHIFT;
		Region.right	= Region.left + TOKEN_FRAME_WIDTH;
		GraphicsObjectRef->DrawSprite(	RenderX,
										RenderY,
										ThePickupGraphic, 
										Region);
	}

	// ----
	// Draw the players
	for (i = 18; i--;)
	{
		if (ReplayBuffer[ReplayBufferPlaybackIndex].Players[i].Frame == -1)
			continue;

		if (ReplayBuffer[ReplayBufferPlaybackIndex].Players[i].Side)
			TempGraphics	= TheRedPlayerGraphic;
		else
			TempGraphics	= TheBluePlayerGraphic;
		
		RenderX	= ReplayBuffer[ReplayBufferPlaybackIndex].Players[i].X;
		RenderY	= ReplayBuffer[ReplayBufferPlaybackIndex].Players[i].Y;
		Frame	= ReplayBuffer[ReplayBufferPlaybackIndex].Players[i].Frame;
			
		// If the are not on the screen.
		if (	((RenderX + PLAYER_FRAME_WIDTH) < 0)
			||	(RenderX > (int)GraphicsObjectRef->GetDisplay()->cxWidth)
			||	((RenderY + PLAYER_FRAME_HEIGHT) < 0)
			||	(RenderY > (int)GraphicsObjectRef->GetDisplay()->cyHeight))
			continue;
			
		Region.top		= (Frame / Y_PLAYER_PITCH) << PLAYER_HEIGHT_SHIFT;
		Region.bottom	= Region.top + PLAYER_FRAME_HEIGHT;
		Region.left		= (Frame % Y_PLAYER_PITCH) << PLAYER_WIDTH_SHIFT;
		Region.right	= Region.left + PLAYER_FRAME_WIDTH;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TempGraphics, Region));
	}

	// ----
	// Ball!
	if (ReplayBuffer[ReplayBufferPlaybackIndex].Ball.Frame != -1)
	{
		RenderX = ReplayBuffer[ReplayBufferPlaybackIndex].Ball.X;
		RenderY = ReplayBuffer[ReplayBufferPlaybackIndex].Ball.Y;
		Frame = ReplayBuffer[ReplayBufferPlaybackIndex].Ball.Frame;

		Region.top		= (Frame / Y_BALL_PITCH) * BALL_FRAME_WIDTH;
		Region.bottom	= Region.top + BALL_FRAME_WIDTH;
		Region.left		= (Frame % Y_BALL_PITCH) * BALL_FRAME_WIDTH;
		Region.right	 = Region.left + BALL_FRAME_WIDTH;

		SAFELY_CALL(GraphicsObjectRef, DrawSprite(RenderX, RenderY, TheBallGraphic, Region));
	}

	ret &= DrawPopUp();
	ret &= DrawInfoBar();

	SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return ret;
}


// ---- 
// Returns true if the Actor is within '_Dimension' pixels of the rendered screen area.
bool	CMatch::IsActorOnScreen(CActor* _ActorRef, int _Dimension)
{
	if (!_ActorRef)
		return false;	// no object

	int RenderX = CMatch::PitchXToScreenX(_ActorRef->getXPos()) - (_Dimension >> 1);
	int RenderY = CMatch::PitchYToScreenY(_ActorRef->getYPos()) - (_Dimension >> 1);

	return (	(	
					((RenderX + _Dimension	) > 0)												// Left edge
				&&	((RenderX				) < (int)GraphicsObjectRef->GetDisplay()->cxWidth)	// Right edge
				&&	((RenderY +	_Dimension	) > 0)												// Top edge
				&&	((RenderY				) < (int)GraphicsObjectRef->GetDisplay()->cyHeight)	// Bottom edge
				)	
			);	

}
//overloaded function for tokens in replays
bool	CMatch::IsActorOnScreen(int x, int y, int _Dimension)
{
	int RenderX = x;
	int RenderY = y;

	return (	(	
					((RenderX + _Dimension	) > 0)												// Left edge
				&&	((RenderX				) < (int)GraphicsObjectRef->GetDisplay()->cxWidth)	// Right edge
				&&	((RenderY +	_Dimension	) > 0)												// Top edge
				&&	((RenderY				) < (int)GraphicsObjectRef->GetDisplay()->cyHeight)	// Bottom edge
				)	
			);	
}

bool	CMatch::DrawPauseScreen()
{
	if (	(!GraphicsObjectRef)
		||	(!SoundSystemRef)
		||	(!ControllerRef)
		||	(!GameDataRef))
		return false;

	// Quick exit for demo.
	if (GameDataRef->GetGameType() == DEMO)
		return true;

	//DialogInitialised = false;
	int		i;
	bool	MouseMovedSelected	= false;
	CString	MenuTextArray[3];

	//SAFELY_CALL(GraphicsObjectRef, StartDraw());

	MenuTextArray[0]	= "Continue";
	MenuTextArray[1]	= "Abandon";

	MenuTextArray[2]	= "Game Paused";
	

		// Force an immedicate redraw (in fact first draw) of the background.
	
	
	// ----
	// Process
	IfUpDB					// Macros
	{
		OptionSelected--;
	}

	IfDownDB
	{
		OptionSelected++;
	}

	if (OptionSelected < 0)	
		OptionSelected = 1;
	if (OptionSelected > (1))
		OptionSelected = 0;

//	char	convert[24];

	// Render
	if (	(GameWindowRef) 
		&&	(GraphicsObjectRef))
	{
		// Draw buttons
		for (i = 1; i >= 0; i--)
		{
			bool InRect;

			GraphicsObjectRef->DrawString(	TheMainMenuFont, 
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
				OptionSelected = i;
				MouseMovedSelected = true;
			}
		}

		GraphicsObjectRef->DrawString(	TheMainMenuFont, 
										&(MenuTextArray[2]),
										GraphicsObjectRef->GetDisplay()->cxWidth >> 1, 
										MENU_TOP - 2 * MENU_ITEM_SPACING,
										TEXT_CENTRED);	
		
		// 
		GraphicsObjectRef->DrawString(	TheMainMenuFont, 
										&(CString(">")),
										MENU_ARROW_OFFSET, 
										MENU_TOP + OptionSelected * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_LEFT);

		GraphicsObjectRef->DrawString(	TheMainMenuFont, 
										&(CString("<")),
										GraphicsObjectRef->GetDisplay()->cxWidth - MENU_ARROW_OFFSET, 
										MENU_TOP + OptionSelected * MENU_ITEM_SPACING,	// Random values, plucked from the air.
										TEXT_RIGHT);
	}
	
	

	// Now process the fire buttons
	IfSelectOrMouseSelectDB(MouseMovedSelected)
	{
		switch (OptionSelected)
		{
		case 0:
			MatchState = PreviousMatchState;
			break;

		case 1:
			MatchState = MatchAbandoned;
			break;

		default:
			NODEFAULT;
			break;
		}
	}
	

	//SAFELY_CALL(GraphicsObjectRef, EndDraw());

	return true;
}
