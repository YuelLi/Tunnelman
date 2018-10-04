#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "GraphObject.h"
#include <string>
#include <queue>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp
class GraphObject;
class Earth;
class Tunnelman;
class Actor;
class Protester;
class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
	~StudentWorld();

	virtual int init();
	virtual int move();
	virtual void cleanUp();

	Tunnelman* getPlayer();
	void updateDisplayText();
	std::string displayText(int score, int level, int lives, int health, int squirts, int gold, int sonar, int barrels);
	void detectNearActors(int x, int y, int radius);
	bool digEarth(int x, int y); 
	void addActor(Actor* actor);
	void addGoodies();
	void addProtesters();
	void decBarrel();
    void decProtester();
	bool withInRadius(int x1, int y1, int x2, int y2, int radius); // return true if it is in radius
	bool ActorsWithInRadius(int x, int y, int radius);  //return if there is an actor in radius
	void addBoulderorGoldorBarrel(int num, char actor);
	bool isAboveEarth(int x, int y);
	bool isThereEarth(int x, int y);
	bool isThereBoulder(int x, int y,int radius=3);
	bool canMoveInDirection(int x, int y, GraphObject::Direction direction);
	bool isPlayerInRadius(Actor* actor, int radius);
	Protester* protesterInRadius(Actor* actor, int radius);
	void moveToExit(Protester* pr);
    GraphObject::Direction senseSignalFromPlayer(Protester* pr, int M);
private:
	bool m_isFirstTick;
	int m_tickSinceLast;
	int m_protestersAlive;
	int m_barrelsLeft;
	Earth* m_earth[64][64];
	
	Tunnelman* m_player;
	std::vector<Actor*> m_actors;
	
    int m_maze[64][64];
	struct Grid {
		int x;
		int y;
        Grid(int a, int b): x(a),y(b){}
    };
};

#endif // STUDENTWORLD_H_
