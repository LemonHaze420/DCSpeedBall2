#ifndef _GAMEDEFINES_H_
#define	_GAMEDEFINES_H_

#ifdef NOT_FINAL_BUILD
#define	REG_SNAPSHOT_PATH		"SnapshotPath"
#endif

// ----
// Application specific.
#define	GAME_NAME				TEXT("SpeedBall2")
#define GAME_WINDOW_CLASS_NAME	TEXT("658554D5-23EB-4953-B9E1-6A1F9F4AA754")
#define	PRODUCT_ACTIVATION_RPN_STRING		"key c % i + 11 * c 18 % + key i * -"

// ----
// Generic. 
#define	MAX_PLAYERS				1
#define	DEFAULT_PLAYER_NAME		"Player One"
#define	MAX_SAVE_NAME_LENGTH	8
#define FRAME_LIMIT				(0.05f)				/* Locked frame rate */
#define SOUND_SAMPLE_FORMAT		WAVE_FORMAT_1M16
#define	REPLAY_BUFFER_LENGTH	50
#define	DEFAULT_DEMO_TIME_OUT	(30)				/* Pause before demo starts */
#define	MIN_DEMO_TIMEOUT		5

// Duration.
#ifdef ULTRA_SHORT_GAME
	#define	DEFAULT_SIDE_DURATION	(4.0f)				/* 4 second a side game, for very quick in and out testing */
	#define	LOWEST_GAME_DURATION	(DEFAULT_SIDE_DURATION)
#else
	#ifdef SHORT_GAME
		#define	DEFAULT_SIDE_DURATION	(15.0f)				/* 15 second a side game, for quick in and out testing */
	#define	LOWEST_GAME_DURATION		(DEFAULT_SIDE_DURATION)
	#else
		#define	DEFAULT_SIDE_DURATION	(90.0f)				/* Default game duration */
		#define	LOWEST_GAME_DURATION	(30.0f)
	#endif
#endif
#define	GAME_PER_SIDE_DURATION	(GameDataRef->GetGameDuration())
#define	UPPER_GAME_DURATION		(120.0f)

// Difficulty.
#define	LOWEST_GAME_DIFFICULTY	-2
#define	UPPER_GAME_DIFFICULTY	2
#define	DEFAULT_DIFFICULTY		0

// ----
// Frontend menu stuff, needed in both FrontEnd.cpp and FrontEnd_Help.cpp
#define SCREEN_CAPTION_POSITION			5
#define SCREEN_FOOTER_POSITION			301	/* 320 - 19 */
#define SCREEN_EXIT						275

#define	MENU_TOP						100
#define MENU_ITEM_SPACING				22
#define	MENU_ARROW_OFFSET				15

// ----
// Path / data info,
#define	REGISTRY_PATH			"Software\\Broadsword\\SpeedBall2"
#define	REG_INSTALL_PATH		"InstallPath"
#define	REG_DATA_PATH			"DataPath"
#define	REG_AUDIO_DATA_PATH		"AudioPath"
#define	REG_IMAGE_DATA_PATH		"ImagePath"
#define	REG_FONTS_PATH			"FontsPath"
#define	REG_SAVE_PATH			"SavePath"

// ----
// Preference settings
#define	REG_VOLUME_SETTING		"Volume"
#define	REG_MUSIC_SETTING		"Music"
#define	REG_ENABLE_DB_SETTING	"EnableDoubleBuffering"
#define	REG_SLOW_VRAM_SETTING	"SlowVRAM"
#define	REG_STYLUS_SETTING		"StylusControl"
//#define	REG_POWER_SAVE_SETTING	"PowerSavingTimeOut"
#define	REG_DURATION_SETTING	"SideDuration"
#define	REG_DIFFICULTY_SETTING	"GameDifficulty"
#define	REG_DEMO_TIMEOUT_SETTING	"DemoTimeOut"
#define	REG_ACTIVATION_CODE		"Code"

#define	STYLUS_CONTROL_NONE		0
#define	STYLUS_CONTROL_SCREEN	1
#define	STYLUS_CONTROL_PLAYER	2
#define	STYLUS_CONTROL_DPAD_R	3
#define	STYLUS_CONTROL_DPAD_L	4
#define	MAX_STYLUS_CONTROL		STYLUS_CONTROL_DPAD_L
#define	SLEEP_NONE				0

#define	DEFAULT_MUSIC_STATE		true
#define	DEFAULT_VOLUME_LEVEL	7
#define	DEFAULT_STYLUS_CONTROL	STYLUS_CONTROL_PLAYER
#define	DEFAULT_AUTO_SLEEP		SLEEP_NONE

// ----
// Message passing
#ifndef WM_MAIN_THREAD_ACTIVATE
#define WM_MAIN_THREAD_ACTIVATE	(WM_USER+0)
#endif

#ifndef WM_MAIN_THREAD_KEYUP
#define WM_MAIN_THREAD_KEYUP	(WM_USER+1)
#endif

#ifndef WM_MAIN_THREAD_KEYDOWN
#define WM_MAIN_THREAD_KEYDOWN	(WM_USER+2)
#endif

#ifndef WM_MAIN_THREAD_DESTROY
#define WM_MAIN_THREAD_DESTROY	(WM_USER+3)
#endif

// Cost of body parts
#define	INT_COST			15
#define	AGR_COST			5
#define	ATT_COST			10
#define	THR_COST			10
#define	POW_COST			10
#define	DEF_COST			10
#define	STA_COST			15
#define	SPD_COST			15

#define	CUP_STARTING_CASH	2000

// Win/lose screen stuff.
#define	POINTS_FOR_LEAGUE_WIN		10
#define	POINTS_FOR_LEAGUE_DRAW		5
#define	POINTS_FOR_LEAGUE_LOSE		0

#define	WIN_TOP						0
#define	WIN_LEFT					0
#define	WIN_HEIGHT					160
#define	WIN_WIDTH					240

#define	LOSE_TOP					0
#define	LOSE_LEFT					0
#define	LOSE_HEIGHT					184
#define	LOSE_WIDTH					224

#define	WIN_MATCH_SCREEN			"win"
#define	LOSE_MATCH_SCREEN			"lose"

#define	POPUP_FONT					"popupfont"

//************************************************************************************
//defines for CActor, type definitions
#define	PLAYER				0
#define TEAM2_PLAYER		1
#define	BALL				2
#define MAP_OBJECT			3
#define BONUS_TOKEN         4
#define WALL				5
#define OTHER               6
//************************************************************************************
//Bounding box size definitions. Done in width and height from centre
#define PLAYER_WIDTH		8
#define PLAYER_HEIGHT		8

#define BALL_WIDTH			5
#define BALL_HEIGHT			5

#define TOKEN_WIDTH			5
#define	TOKEN_HEIGHT		5

//************************************************************************************
//Team positions.
#define CFWD 0
#define RWNG 1 
#define LWNG 2
#define CMID 3
#define RMID 4
#define LMID 5
#define RDEF 6
#define LDEF 7
#define GOAL 8
#define SUB1 9
#define SUB2 10
#define SUB3 11

//lines of positions
#define FWDS 0
#define MID	 3
#define DEF  6
#define SUBS 9
//************************************************************************************
//Actor current action and state flags for players

#define NONE			0
#define	FLOORED			4
#define RUNNING			5
#define SLIDING			6
#define JUMPING			7
#define PUNCHING		8
#define FALLING			9
#define THROWING		10
#define DANCING			11
#define KNEE_SLIDE		12
#define	DIVING			13
#define CATCHING		14
#define GOALWALK		15
//************************************************************************************
//ball state flags

#define STOPPED			0
#define ONFLOOR			1
#define	LOW				2
#define	HIGH			3
#define LAUNCHING		4
#define PREPARE_THROW	5
#define	HELD			6
#define	IN_CHUTE		7
//*************************************************************************************
//map object state flags
#define UNLIT			0
#define	LIT				1
#define IN_USE			1
//*************************************************************************************
//Actor AI Flags

#define NOBODY			0x00000000
#define HUMAN			0xF0000000
//*****************************************************************************************
//TeamNameDefinitions

#define BRUTAL_DELUXE		0
#define REVOLVER			1
#define RAW_MESSIAHS		2
#define VIOLENT_DESIRE		3
#define BAROQUE				4
#define THE_RENEGADES		5
#define DAMOCLES			6
#define STEEL_FURY			7
#define POWERHOUSE			8
#define RAGE				9
#define MEAN_MACHINE		10
#define EXPLOSIVE_LORDS		11
#define LETHAL_FORMULA		12
#define TURBO_HAMMERS		13
#define FATAL_JUSTICE		14
#define SUPER_NASHWAN		15
#define OPPONENTS			16	//any opponents... ie.. player one is always brutal deluxe
//*****************************************************************************************
//Game Type Defs
#define PRACTICE			0
#define LEAGUE				1
#define CUP					2
#define KNOCKOUT			3
#define	DEMO				4
#define	LEAGUE_TEAM_MANAGER	5
//#define 2PLAYER			4 not used
//**************************************************************************************
//Match team defines
#define NEITHER				0
#define TEAM_ONE			1 //normally player one
#define TEAM_TWO			2 //normally computer
//****************************************************************************************

#define	NUM_OF_DIVISIONS				2
#define	NUM_OF_TEAMS_PER_DIVISION		8
#define	LEAGUE_FIXTURES					8 //number of fixtures in a league season

// If in division one the player can only by stars costing
// more than these values, if div 2 only cheaper players.
// _TM applies if playing TeamManager
// _NEW applies for controlled games
#define	DIVISION_1_PRICE_THRESHOLD_TM	900	
#define	DIVISION_1_PRICE_THRESHOLD_NEW	900
#define	NUM_OF_CUP_ROUNDS				4
#define	MAX_CUP_FIXTURES				8

//*******************************************************************************************
//Direction of travel defines
#define N		0
#define NE		1
#define E		2
#define SE		3
#define S		4
#define SW		5
#define W		6
#define NW		7

#define STILL	-1
//*************************************************************************************
//Position Definitions
//These are all in pixel distances from the TL.. I am going with a pitch that is
#define CENTRE_PITCHX		0
#define CENTRE_PITCHY	    0

#define PITCH_TLX			-266
#define PITCH_TLY			528

#define	PITCH_BLX			-266
#define	PITCH_BLY			-528

#define	PITCH_BRX			266
#define	PITCH_BRY			-528

#define	PITCH_TRX			266
#define	PITCH_TRY			528

//yardage lines
#define MIDDLEY				0
#define	MIDDLEX				0
//going up
#define TWENTY_FIVEU			144
#define	FIFTYU					288
#define SEVENTY_FIVEU			432
//going down
#define TWENTY_FIVED			-144
#define FIFTYD					-288
#define	SEVENTY_FIVED			-432

//player position default definitions.. only on the x...

#define LHS_DEFAULT_X		-160
#define	MIDDLE_DEFAULT_X	0
#define	RHS_DEFAULT_X		160

//goal dimensions
#define GOAL_X_START	-50
#define	GOAL_X_END		 50
#define TOP_GOAL_Y		538
#define	BOTTOM_GOAL_Y  -538
//***********************************************************************************
//next defines are where the pitch objects are. These will probably change once we get an idea
//of scale on the pocket pc

//the left and right multiplier ramps
#define RAMP_LEFT_Y1	0	
#define RAMP_LEFT_X1	305	
#define RAMP_LEFT_Y2	-56	
#define RAMP_LEFT_X2	320	

#define RAMP_RIGHT_Y1	0	
#define RAMP_RIGHT_X1	320	
#define RAMP_RIGHT_Y2	66	
#define RAMP_RIGHT_X2	305	

//the shrooms.... not really sure about the size of these
#define SHROOM_WIDTH		16 //actually half width.. but works for my evil purposes
#define	SHROOM_X			0
#define	TSHROOM_Y			256
#define	BSHROOM_Y		   -256

//the warps

#define TOP_WARP_SY			182
#define	TOP_WARP_EY			214

#define	BOTTOM_WARP_SY		-182
#define BOTTOM_WARP_EY		-214

//the stars...... will be done once I know how they are done.
#define	LEFT_STARBANK_SY	32
#define	LEFT_STARBANK_EY	192

#define	RIGHT_STARBANK_SY	-32
#define	RIGHT_STARBANK_EY	-192


//bumpers on the side
#define	DOME_LEFT_SY		-352
#define	DOME_LEFT_EY		-384

#define DOME_RIGHT_SY		352
#define DOME_RIGHT_EY		384

//***********************************************************************************
//Token Definitions
#define GONE		1000	//state definition for when it is picked up

#define FREEZETEAM	0	//freeze opposition
//#define REVERSE		1	//reverse opponent's joystick
#define REDUCE		2	//reduce opposition to minimum
#define INCREASE	3	//boost whole team to maximum
#define MANIC		4	//boost all players to maximum
#define SLOWDOWN	5	//reduce opposition to slowest
#define GRABBALL	6	//grab ball
#define TRANSPORT	7	//transport ball to centre forward
#define GOALDOOR	8	//activate goal door
#define SHIELDTEAM	9	//make team immune to tackles
#define FULLENERGY	10	//boost all attributes to initial values
#define ZAPTEAM		11	//tackle opposing team
#define COIN		20
//Weapon Definitions

#define ATTACKWEAPON	12 //increases attack power
#define DEFENCEWEAPON	13 //increases defence strength
#define SPEEDWEAPON		14 //increases speed
#define THROWWEAPON		15 //increases throw power
#define POWERWEAPON		16 //increases power power
#define STAMINAWEAPON	17 //increases stamina
#define IQWEAPON		18 //increases IQ
#define AGGRWEAPON		19 //Increases Aggressiveness

#define	MAX_TOKENS		15//maximum amount of token on the map at once
#define PICKED_UP		100
#define WEAPON_GOT		101
//***********************************************************************************
//Animation frame constants

#define	FALLING_FRAMES		20 //this number is just wrong....... but I don't know how many there are yet
#define	JUMPING_FRAMES		10 //same again;
#define PUNCHING_FRAMES		8 //well duhhh
#define SLIDING_FRAMES		5 //just for a change
#define CHUTE_FRAMES		50 //that fooled ya!!!
#define RUNNING_FRAMES		8
#define THROWING_FRAMES		5
#define DANCING_FRAMES		4
#define KNEESLIDE_FRAMES	60
#define	DIVING_FRAMES		8   //was 5
#define	FLOORED_FRAMES		10

//***********************************************************************************
//game play stuff
//which way are u playing mr.player sir

#define UP_MAP		 1 //playing up the map towards the top goal
#define DOWN_MAP	 0 //playing down the map towards the enigmatic bottom goal

#define FIRST_HALF		 1
#define SECOND_HALF		 0

//defines for the colour of stars
#define RED			1
#define BLUE		0	// are: Swapped.

//***********************************************************************************

#endif