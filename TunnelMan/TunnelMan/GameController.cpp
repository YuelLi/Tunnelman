#include "freeglut.h"
#include "GameController.h"
#include "GameWorld.h"
#include "GameConstants.h"
#include "GraphObject.h"
#include "SoundFX.h"
#include "SpriteManager.h"
#include <string>
#include <map>
#include <utility>
#include <cstdlib>
#include <algorithm>
using namespace std;

/*
spriteWidth = .67
spritesPerRow = 16

RowWidth = spriteWidth*spritesPerRow = 10.72
PixelWidth = RowWidth/256 = .041875
newSpriteWidth = PixelWidth * NumPixels

spriteHeight = .54
spritesPerRow = 16

RowHeight = spriteHeight*spritesPerCol = 8.64

PixelHeight = RowHeight/256 = .03375

newSpriteHeight = PixelHeight * NumPixels
*/

static const int WINDOW_WIDTH = 768; //1024;
static const int WINDOW_HEIGHT = 768;

static const int PERSPECTIVE_NEAR_PLANE = 4;
static const int PERSPECTIVE_FAR_PLANE	= 22;

static const double VISIBLE_MIN_X = -2.15; // -2.04375; //	-2.4375; //-3.25;
static const double VISIBLE_MAX_X = 1.85; //  2.04375;	  //1.65;// 2.4375; //3.25;
static const double VISIBLE_MIN_Y = -2.1;
static const double VISIBLE_MAX_Y = 1.9;
static const double VISIBLE_MIN_Z = -20;
static const double VISIBLE_MAX_Z = -6;

static const double FONT_SCALEDOWN = 760.0;

static const double SCORE_Y = 3.8;
static const double SCORE_Z = -10;

static const int MS_PER_FRAME = 1;

static const double PI = 4 * atan(1.0);

struct SpriteInfo
{
	unsigned int imageID;
	unsigned int frameNum;
	std::string	 tgaFileName;
};

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz);
static void drawPrompt(string mainMessage, string secondMessage);
static void drawScoreAndLives(string);

void GameController::initDrawersAndSounds()
{
	SpriteInfo drawers[] = {
		{ TID_PLAYER		   , 0, "dig1.tga"},
		{ TID_PLAYER		   , 1, "dig2.tga"},
		{ TID_PLAYER		   , 2, "dig3.tga" },
		{ TID_PLAYER		   , 3, "dig4.tga"},
		{ TID_PROTESTER, 0, "protester1.tga"},
		{ TID_PROTESTER, 1, "protester2.tga" },
		{ TID_PROTESTER, 2, "protester3.tga" },
		{ TID_HARD_CORE_PROTESTER, 0, "hardcore1.tga" },
		{ TID_HARD_CORE_PROTESTER, 1, "hardcore2.tga" },
		{ TID_HARD_CORE_PROTESTER, 2, "hardcore3.tga" },
		{ TID_HARD_CORE_PROTESTER, 3, "hardcore4.tga" },
		{ TID_WATER_SPURT, 0, "water1.tga" },
		{ TID_WATER_SPURT, 1, "water2.tga" },
		{ TID_WATER_SPURT, 2, "water3.tga" },
		{ TID_BARREL , 0, "barrel.tga"},
		{ TID_GOLD, 0, "gold.tga" },
		{ TID_SONAR, 0, "sonar.tga" },
		{ TID_WATER_POOL , 0, "waterpool.tga" },
		{ TID_EARTH, 0, "earth.tga" },
		{ TID_BOULDER, 0, "rock1.tga" },
		{ TID_BOULDER, 1, "rock2.tga" },
		{ TID_BOULDER, 2, "rock3.tga" },
		{ TID_BOULDER, 3, "rock4.tga" },
	};

	SoundMapType::value_type sounds[] = {
		make_pair(SOUND_THEME					, "theme.wav"),
		make_pair(SOUND_PROTESTER_YELL		   , "goaway.wav"),
		make_pair(SOUND_PROTESTER_GIVE_UP		, "giveup.wav"),
		make_pair(SOUND_PLAYER_GIVE_UP			   , "die.wav"),
		make_pair(SOUND_GOT_GOODIE			   , "woohoo.wav"),
		make_pair(SOUND_FINISHED_LEVEL		   , "finished.wav"),
		make_pair(SOUND_PROTESTER_ANNOYED		   , "ouch.wav"),
		make_pair(SOUND_PROTESTER_FOUND_GOLD , "bribed.wav"),			// to add
		make_pair(SOUND_FOUND_OIL,				"foundoil.wav"),			// to add
		make_pair(SOUND_SONAR				   , "sonar.wav"),
		make_pair(SOUND_PLAYER_SQUIRT, "squirt.wav"),
		make_pair(SOUND_DIG,					"digging.wav"),
		make_pair(SOUND_FALLING_ROCK, "rockslide.wav")
	};

	for (int k = 0; k < sizeof(drawers)/sizeof(drawers[0]); k++)
	{
		string path = m_gw->assetDirectory();
		if (!path.empty())
			path += '/';
		const SpriteInfo& d = drawers[k];
		if (!m_spriteManager.loadSprite(path + d.tgaFileName, d.imageID, d.frameNum))
			exit(0);
	}
	for (int k = 0; k < sizeof(sounds)/sizeof(sounds[0]); k++)
		m_soundMap[sounds[k].first] = sounds[k].second;
}

static void doSomethingCallback()
{
	Game().doSomething();
}

static void reshapeCallback(int w, int h)
{
	Game().reshape(w, h);
}

static void keyboardEventCallback(unsigned char key, int x, int y)
{
	Game().keyboardEvent(key, x, y);
}

static void specialKeyboardEventCallback(int key, int x, int y)
{
	Game().specialKeyboardEvent(key, x, y);
}

static void timerFuncCallback(int val)
{
	Game().doSomething();
	glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);
}

void GameController::run(int argc, char* argv[], GameWorld* gw, string windowTitle)
{
	gw->setController(this);
	m_gw = gw;
	setGameState(welcome);
	m_lastKeyHit = INVALID_KEY;
	m_singleStep = false;
	m_curIntraFrameTick = 0;
	m_playerWon = false;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(windowTitle.c_str());

	initDrawersAndSounds();

	glutKeyboardFunc(keyboardEventCallback);
	glutSpecialFunc(specialKeyboardEventCallback);
	glutReshapeFunc(reshapeCallback);
	glutDisplayFunc(doSomethingCallback);
	glutTimerFunc(MS_PER_FRAME, timerFuncCallback, 0);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
	delete m_gw;
}

void GameController::keyboardEvent(unsigned char key, int /* x */, int /* y */)
{
	switch (key)
	{
		case 'a': case '4': m_lastKeyHit = KEY_PRESS_LEFT;	break;
		case 'd': case '6': m_lastKeyHit = KEY_PRESS_RIGHT; break;
		case 'w': case '8': m_lastKeyHit = KEY_PRESS_UP;	break;
		case 's': case '2': m_lastKeyHit = KEY_PRESS_DOWN;	break;
		case 't':			m_lastKeyHit = KEY_PRESS_TAB;	break;
		case 'f':			m_singleStep = true;			break;
		case 'r':			m_singleStep = false;			break;
		case 'q': case 'Q': setGameState(quit);				break;
		default:			m_lastKeyHit = key;				break;
	}
}

void GameController::specialKeyboardEvent(int key, int /* x */, int /* y */)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:	 m_lastKeyHit = KEY_PRESS_LEFT;	 break;
		case GLUT_KEY_RIGHT: m_lastKeyHit = KEY_PRESS_RIGHT; break;
		case GLUT_KEY_UP:	 m_lastKeyHit = KEY_PRESS_UP;	 break;
		case GLUT_KEY_DOWN:	 m_lastKeyHit = KEY_PRESS_DOWN;	 break;
		default:			 m_lastKeyHit = INVALID_KEY;	 break;
	}
}

void GameController::playSound(int soundID)
{
	if (soundID == SOUND_NONE)
		return;

	SoundMapType::const_iterator p = m_soundMap.find(soundID);
	if (p != m_soundMap.end())
	{
		string path = m_gw->assetDirectory();
		if (!path.empty())
			path += '/';
		SoundFX().playClip(path + p->second);
	}
}

void GameController::doSomething()
{
	switch (m_gameState)
	{
		case not_applicable:
			break;
		case welcome:
			playSound(SOUND_THEME);
			m_mainMessage = "Welcome to TunnelMan!";
			m_secondMessage = "Press Enter to begin play...";
			setGameState(prompt);
			m_nextStateAfterPrompt = init;
			break;
		case contgame:
			m_mainMessage = "You lost a life!";
			m_secondMessage = "Press Enter to continue playing...";
			setGameState(prompt);
			m_nextStateAfterPrompt = cleanup;
			break;
		case finishedlevel:
			m_mainMessage = "Woot! You finished the level!";
			m_secondMessage = "Press Enter to continue playing...";
			setGameState(prompt);
			m_nextStateAfterPrompt = cleanup;
			break;
		case makemove:
			m_curIntraFrameTick = ANIMATION_POSITIONS_PER_TICK;
			m_nextStateAfterAnimate = not_applicable;
			{
				int status = m_gw->move();
				if (status == GWSTATUS_PLAYER_DIED)
				{
					  // animate one last frame so the player can see what happened
					m_nextStateAfterAnimate = (m_gw->isGameOver() ? gameover : contgame);
				}
				else if (status == GWSTATUS_FINISHED_LEVEL)
				{
					m_gw->advanceToNextLevel();
					  // animate one last frame so the player can see what happened
					m_nextStateAfterAnimate = finishedlevel;
				}
			}
			setGameState(animate);
			break;
		case animate:
			displayGamePlay();
			if (m_curIntraFrameTick-- <= 0)
			{
				if (m_nextStateAfterAnimate != not_applicable)
					setGameState(m_nextStateAfterAnimate);
				else
				{
					int key;
					if (!m_singleStep  ||  getLastKey(key))
						setGameState(makemove);
				}
			}
			break;
		case cleanup:
			m_gw->cleanUp();
			setGameState(init);
			break;
		case gameover:
			{
				ostringstream oss;
				oss << (m_playerWon ? "You won the game!" : "Game Over!")
					<< " Final score: " << m_gw->getScore() << "!";
				m_mainMessage = oss.str();
			}
			m_secondMessage = "Press Enter to quit...";
			setGameState(prompt);
			m_nextStateAfterPrompt = quit;
			break;
		case prompt:
			drawPrompt(m_mainMessage, m_secondMessage);
			{
				int key;
				if (getLastKey(key) && key == '\r')
					setGameState(m_nextStateAfterPrompt);
			}
			break;
		case init:
			{
				int status = m_gw->init();
				SoundFX().abortClip();
				if (status == GWSTATUS_PLAYER_WON)
				{
					m_playerWon = true;
					setGameState(gameover);
				}
				else if (status == GWSTATUS_LEVEL_ERROR)
				{
					m_mainMessage = "Error in level data file encoding!";
					m_secondMessage = "Press Enter to quit...";
					setGameState(prompt);
					m_nextStateAfterPrompt = quit;
				}
				else
					setGameState(makemove);
			}
			break;
		case quit:
			glutLeaveMainLoop();
			break;
	}
}

void GameController::drawEarth(double gx, double gy, double gz, double size)
{
	glPushMatrix();


	const GLfloat finalWidth = static_cast<GLfloat>(SPRITE_WIDTH_GL * size);
	const GLfloat finalHeight = static_cast<GLfloat>(SPRITE_HEIGHT_GL * size);

	// object's x/y location is center-based, but sprite plotting is upper-left-corner based
	const double xoffset = SPRITE_WIDTH_GL / 2;
	const double yoffset = SPRITE_HEIGHT_GL / 2;

	glTranslatef(static_cast<GLfloat>(gx - xoffset), static_cast<GLfloat>(gy - yoffset), static_cast<GLfloat>(gz));
	glPushAttrib(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const int pos = static_cast<int>(173 * gx*gx*gy*gy + 647*gx*gy + 397*gx*gx + 817*gy*gy);
	const GLfloat r = static_cast<GLfloat>(.7 + (pos % 11)*1.0 / 100.0 - .05);
	const GLfloat g = static_cast<GLfloat>(.6 + (pos % 7)*1.0 / 100.0 - .05);
	const GLfloat b = static_cast<GLfloat>(0.8 + (pos % 13)*1.0 / 100.0 - .05);
	glColor3f(r, g, b);

	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(finalWidth, 0, 0);
	glVertex3f(finalWidth, finalHeight, 0);
	glVertex3f(0, finalHeight, 0);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	glPopAttrib();
	glPopMatrix();

}

void GameController::displayGamePlay()
{
	glEnable(GL_DEPTH_TEST); // must be done each time before displaying graphics or gets disabled for some reason
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt(0, 0, 0, 0, 0, -1, 0, 1, 0);

	for (int i = NUM_LAYERS - 1; i >= 0; --i)
	{
		std::set<GraphObject*> &graphObjects = GraphObject::getGraphObjects(i);

		for (auto it = graphObjects.begin(); it != graphObjects.end(); it++)
		{
			GraphObject* cur = *it;
			if (cur->isVisible())
			{
				cur->animate();

				double x, y, gx, gy, gz;
				cur->getAnimationLocation(x, y);
				convertToGlutCoords(x, y, gx, gy, gz);

				SpriteManager::Angle angle;
				switch (cur->getDirection())
				{
				case GraphObject::up:
					angle = SpriteManager::face_up;
					break;
				case GraphObject::down:
					angle = SpriteManager::face_down;
					break;
				case GraphObject::left:
					angle = SpriteManager::face_left;
					break;
				case GraphObject::right:
				case GraphObject::none:
				default:
					angle = SpriteManager::face_right;
					break;
				}

				int imageID = cur->getID();

				// the specialized Earth plotting is an optimization to deal with the background Earth, which requires a lot of horsepower to plot
				if (imageID == TID_EARTH)
					drawEarth(gx, gy, gz, cur->getSize());
				else
					m_spriteManager.plotSprite(imageID, cur->getAnimationNumber() % m_spriteManager.getNumFrames(imageID), gx, gy, gz, angle, cur->getSize());
			}
		}
	}

	drawScoreAndLives(m_gameStatText);

	glutSwapBuffers();
}

void GameController::reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(45.0, double(WINDOW_WIDTH) / WINDOW_HEIGHT, PERSPECTIVE_NEAR_PLANE, PERSPECTIVE_FAR_PLANE);
	glMatrixMode (GL_MODELVIEW);
}

static void convertToGlutCoords(double x, double y, double& gx, double& gy, double& gz)
{
	x /= VIEW_WIDTH;
	y /= VIEW_HEIGHT;
	gx = 2 * VISIBLE_MIN_X + .3 + x * 2 * (VISIBLE_MAX_X - VISIBLE_MIN_X);
	gy = 2 * VISIBLE_MIN_Y +	  y * 2 * (VISIBLE_MAX_Y - VISIBLE_MIN_Y);
	gz = .6 * VISIBLE_MIN_Z;
}

static void doOutputStroke(double x, double y, double z, double size, const char* str, bool centered)
{
	if (centered)
	{
		double len = glutStrokeLength(GLUT_STROKE_ROMAN, reinterpret_cast<const unsigned char*>(str)) / FONT_SCALEDOWN;
		x = -len / 2;
		size = 1;
	}
	GLfloat scaledSize = static_cast<GLfloat>(size / FONT_SCALEDOWN);
	glPushMatrix();
	glLineWidth(1);
	glLoadIdentity();
	glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), static_cast<GLfloat>(z));
	glScalef(scaledSize, scaledSize, scaledSize);
	for ( ; *str != '\0'; str++)
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *str);
	glPopMatrix();
}

static void outputStroke(double x, double y, double z, double size, const char* str)
{
	doOutputStroke(x, y, z, size, str, false);
}

static void outputStrokeCentered(double y, double z, const char* str)
{
	doOutputStroke(0, y, z, 1, str, true);
}

static void drawPrompt(string mainMessage, string secondMessage)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	glLoadIdentity ();
	outputStrokeCentered(1, -5, mainMessage.c_str());
	outputStrokeCentered(-1, -5, secondMessage.c_str());
	glutSwapBuffers();
}

static void drawScoreAndLives(string gameStatText)
{
	static int RATE = 1;
	static GLfloat rgb[3] =
		{ static_cast<GLfloat>(.6), static_cast<GLfloat>(.6), static_cast<GLfloat>(.6) };
	for (int k = 0; k < 3; k++)
	{
		double strength = rgb[k] + (-RATE + rand() % (2*RATE+1)) / 100.0;
		if (strength < .6)
			strength = .6;
		else if (strength > 1.0)
			strength = 1.0;
		rgb[k] = static_cast<GLfloat>(strength);
	}
	glColor3f(rgb[0], rgb[1], rgb[2]);
	outputStrokeCentered(SCORE_Y, SCORE_Z, gameStatText.c_str());
}
