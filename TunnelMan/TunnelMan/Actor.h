#ifndef ACTOR_H_
#define ACTOR_H_
#include "GameConstants.h"
#include "GraphObject.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth);
	virtual ~Actor();
	StudentWorld* getWorld();
	bool isAlive();
	void die();
	void moveTo(int x, int y);
	virtual void doSomething() = 0;
    virtual void isAnnoyed(int hP) {}
private:
	StudentWorld* m_world;
	bool m_alive;

};
class Man :public Actor {
public:
	Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp);
	int getHP();
	void decHP(int points);
	virtual void moveInDirection(Direction direction) = 0;
    virtual void isAnnoyed(int hp)=0;
private:
	int m_hP;
};

class Tunnelman :public Man{
public:
	Tunnelman(StudentWorld* world);
	void add(int id);
	int getWtr();
	int getSonar();
	int getGld();
	virtual void doSomething();
	void shoot();
	virtual void isAnnoyed(int hP);
	virtual void moveInDirection(Direction direction);
private:
	int m_wtr;
	int m_sonar;
	int m_gld;
};

class Earth :public Actor {
public:
	Earth(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};

class Boulder : public Actor {
public:
	Boulder(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
	void annoyMan();
private:
	bool isStable;
	bool isFalling;
	int m_ticks;

};
class Squirt : public Actor {
public:
	Squirt(StudentWorld* world, int startX, int startY, Direction dir);
	virtual void doSomething();
	bool annoyProtesters();
private:
	int m_travel;
};

class Pickupable :public Actor {
public:
	Pickupable(StudentWorld* world, int imageID, int startX, int startY);
	virtual void doSomething() = 0;
	void disappearIn(int ticks);
private:
	int m_tick;
};

class Barrel : public Pickupable {
public:
	Barrel(StudentWorld* world, int startX, int startY);
	virtual void doSomething();
};
class Gold :public Pickupable {
public:
	Gold(StudentWorld* world, int startX, int startY, bool visible,bool dropped);
	virtual void doSomething();
private:
	bool isDropped;
};

class Goodie : public Pickupable {
public:
	Goodie(StudentWorld* world, int imageID, int startX, int startY);
	virtual void doSomething();
};

class Sonar :public Goodie {
public:
	Sonar(StudentWorld* world, int startX, int startY);
};
class WaterPool: public Goodie {
public:
	WaterPool(StudentWorld* world, int startX, int startY);
};

class Protester : public Man{
public:
	Protester(StudentWorld* world, int imageID,int hP);
	virtual void doSomething();
    virtual void getBribed();
	void moveInDirection(Direction direction);
	void randomNumToMove();
	virtual void isAnnoyed(int hP);
	void getStunned();
	bool isFacingPlayer();
	Direction directionToPlayer();
	bool straightPathToPlayer(Direction direction);
	Direction randomDirection();
	bool isAtIntersection();
	void pickViableDirectionToTurn();

private:
	bool m_leave;
	int m_numToMove;
	int m_tickSinceLastTurn;
	int m_tickNoYell;
	int m_tickRest;
};
class RegularProtester : public Protester {
public:
	RegularProtester(StudentWorld* world);
};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* world);
};


#endif // ACTOR_H_
