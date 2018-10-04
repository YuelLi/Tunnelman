#include "GameWorld.h"
#include "GameController.h"
#include <string>
#include <cstdlib>
using namespace std;

bool GameWorld::getKey(int& value)
{
	bool gotKey = m_controller->getLastKey(value);

	if (gotKey)
	{
		if (value == 'q')
			m_controller->quitGame();
		else if (value == '\x03')  // CTRL-C
			exit(0);
	}
	return gotKey;
}

void GameWorld::playSound(int soundID)
{
	m_controller->playSound(soundID);
}

void GameWorld::setGameStatText(string text)
{
	m_controller->setGameStatText(text);
}
