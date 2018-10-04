#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_

// IDs for the game objects

const int TID_PLAYER		   = 0;
const int TID_PROTESTER		   = 1;
const int TID_HARD_CORE_PROTESTER = 2;
const int TID_WATER_SPURT	   = 3;
const int TID_BOULDER		   = 4;
const int TID_BARREL		   = 5;
const int TID_EARTH			   = 6;
const int TID_GOLD			   = 7;
const int TID_SONAR			   = 8;
const int TID_WATER_POOL	   = 9;

// sounds

const int SOUND_THEME					= 0;
const int SOUND_PROTESTER_GIVE_UP		= 1;
const int SOUND_PLAYER_GIVE_UP			= 2;
const int SOUND_PROTESTER_YELL			= 3;
const int SOUND_PLAYER_SQUIRT			= 4;
const int SOUND_GOT_GOODIE				= 5;
const int SOUND_DIG						= 6;
const int SOUND_FINISHED_LEVEL			= 7;
const int SOUND_FOUND_OIL				= 8;
const int SOUND_PROTESTER_ANNOYED		= 9;
const int SOUND_PLAYER_ANNOYED			= 10;
const int SOUND_PROTESTER_FOUND_GOLD	= 11;
const int SOUND_SONAR					= 12;
const int SOUND_FALLING_ROCK			= 13;

const int SOUND_NONE				   = -1;

// keys the user can hit

const int KEY_PRESS_LEFT  = 1000;
const int KEY_PRESS_RIGHT = 1001;
const int KEY_PRESS_UP	  = 1002;
const int KEY_PRESS_DOWN  = 1003;
const int KEY_PRESS_SPACE = ' ';
const int KEY_PRESS_ESCAPE = '\x1b';
const int KEY_PRESS_TAB	  = '\t';

// board and sprite dimensions

const int VIEW_WIDTH = 64; //256;
const int VIEW_HEIGHT = 64;	 //256;

const int SPRITE_WIDTH = VIEW_WIDTH/16;
const int SPRITE_HEIGHT = VIEW_HEIGHT/16;

const double SPRITE_WIDTH_GL = .5; // note - this is tied implicitly to SPRITE_WIDTH due to carey's sloppy openGL programming
const double SPRITE_HEIGHT_GL = .5; // note - this is tied implicitly to SPRITE_HEIGHT due to carey's sloppy openGL programming

// status of each tick (did the player die?)

const int GWSTATUS_PLAYER_DIED	 = 0;
const int GWSTATUS_CONTINUE_GAME = 1;
const int GWSTATUS_PLAYER_WON	 = 2;
const int GWSTATUS_FINISHED_LEVEL= 3;
const int GWSTATUS_LEVEL_ERROR	 = 4;


// test parameter constants

const int NUM_TEST_PARAMS			  = 1;

#endif // GAMECONSTANTS_H_
