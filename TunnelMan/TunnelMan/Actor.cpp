#include "Actor.h"
#include "GraphObject.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

//Actor
Actor::Actor(StudentWorld* world, int imageID, int startX, int startY, Direction dir, double size, unsigned int depth)
	:GraphObject(imageID, startX, startY, dir, size, depth), m_world(world),m_alive(true)
{
	setVisible(true);
}
Actor::~Actor()
{
	setVisible(false);
}
StudentWorld* Actor::getWorld() {
	return m_world;
}
bool Actor::isAlive() {
	return m_alive;
}
void Actor::die() {
	m_alive = false;
}
void Actor::moveTo(int x, int y)
{
	if (x < 0) x = 0;
	if (x > 60) x = 60;
	if (y < 0) y = 0;
	if (y > 60)  y = 60;
	GraphObject::moveTo(x, y);
}



//Man
Man::Man(StudentWorld* world, int imageID, int startX, int startY, Direction dir, int hp)
	:Actor(world, imageID, startX, startY, dir, 1.0, 0), m_hP(hp) {}
int Man::getHP() {
	return m_hP;
}

void Man::decHP(int points)
{
	m_hP -= points;
}

//Tunnelman
Tunnelman::Tunnelman(StudentWorld* world)
	:Man(world, TID_PLAYER, 30, 60, right, 10), m_wtr(5), m_sonar(1), m_gld(0) {}

void Tunnelman::add(int id)
{
	if (id == TID_GOLD) m_gld++;
	if (id == TID_WATER_POOL) m_wtr+=5;
	if (id == TID_SONAR) m_sonar += 2;
}

void Tunnelman::doSomething()
{
	if (!isAlive()) return;
	if (getWorld()->digEarth(getX(), getY())) 
		getWorld()->playSound(SOUND_DIG);
	int input;
	if (getWorld()->getKey(input) == true) {
		// user hit a key this tick!
		switch (input)
		{
		case KEY_PRESS_ESCAPE:
			die();
			break;
		case KEY_PRESS_LEFT: moveInDirection(left); break;
		case KEY_PRESS_RIGHT:moveInDirection(right); break;
		case KEY_PRESS_UP:   moveInDirection(up); break;
		case KEY_PRESS_DOWN: moveInDirection(down); break;
		case KEY_PRESS_SPACE:
			if (m_wtr > 0) {
				m_wtr--;
				shoot();
			}
			break;
		case 'z':
		case 'Z':			//use sonar kit
			if (m_sonar > 0)
			{
				m_sonar--;
				getWorld()->detectNearActors(getX(), getY(), 12);
				getWorld()->playSound(SOUND_SONAR);
			}
			break;
		case KEY_PRESS_TAB:		//bribe with gold
			if (m_gld > 0) {
				getWorld()->addActor(new Gold(getWorld(), getX(), getY(), true, true));
				m_gld--;
			}
			break;
		}
	}
}

void Tunnelman::shoot() {
	switch (getDirection())
	{
    case left:
		if(!getWorld()->isThereEarth(getX()-4,getY())&& !getWorld()->isThereBoulder(getX()-4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() - 4, getY(), getDirection()));
		break;
    case right:
		if (!getWorld()->isThereEarth(getX() + 4, getY()) && !getWorld()->isThereBoulder(getX()+4, getY()))
			getWorld()->addActor(new Squirt(getWorld(), getX() + 4, getY(), getDirection()));
		break;
    case down:
		if (!getWorld()->isThereEarth(getX() , getY()-4) && !getWorld()->isThereBoulder(getX(), getY()-4))
			getWorld()->addActor(new Squirt(getWorld(), getX(), getY() - 4, getDirection()));
		break;
	case up:
		if (!getWorld()->isThereEarth(getX(), getY() + 4) && !getWorld()->isThereBoulder(getX(), getY()+4 ))
		getWorld()->addActor(new Squirt(getWorld(), getX(), getY() + 4, getDirection()));
		break;
    case none:return;
	}
	getWorld()->playSound(SOUND_PLAYER_SQUIRT);
}
void Tunnelman::isAnnoyed(int hP)
{
	decHP(hP);
	if (getHP() <= 0) {
		die();
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	}
}

void Tunnelman::moveInDirection(Direction direction)
{
	switch (direction) {
	case left:
		if (getDirection() == left) {
			if (!getWorld()->isThereBoulder(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			else moveTo(getX(), getY());
		}
		else setDirection(left);
		break;
	case right:
		if (getDirection() == right) {
			if (!getWorld()->isThereBoulder(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			else moveTo(getX(), getY());
		}
		else setDirection(right);
		break;
	case up:
		if (getDirection() == up) {
			if (!getWorld()->isThereBoulder(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			else moveTo(getX(), getY());
		}
		else setDirection(up);
		break;
	case down:
		if (getDirection() == down) {
			if (!getWorld()->isThereBoulder(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			else moveTo(getX(), getY());
		}
		else setDirection(down);
		break;
    case none: return;
	}
}

int Tunnelman::getWtr()
{
	return m_wtr;
}

int Tunnelman::getSonar()
{
	return m_sonar;
}

int Tunnelman::getGld()
{
	return m_gld;
}

//Earth
Earth::Earth(StudentWorld* world, int startX, int startY)
	:Actor(world, TID_EARTH, startX, startY, right, 0.25, 3) {}
void Earth::doSomething() {}  // dont do anything


//Boulder
Boulder::Boulder(StudentWorld* world, int startX, int startY)
	: Actor(world, TID_BOULDER, startX, startY, down, 1.0, 1),isStable(true),m_ticks(0)
{
	world->digEarth(startX, startY);
}
void Boulder::doSomething() {
	if (!isAlive()) return;
	if (isStable) {
		if (getWorld()->isAboveEarth(getX(), getY() - 1))
			return;
		else isStable = false;
	}
	if (m_ticks == 30) {
		isFalling = true;
		getWorld()->playSound(SOUND_FALLING_ROCK);
	}
	m_ticks++;
	if (isFalling) {
		if (getWorld()->isAboveEarth(getX(), getY()-1)|| getWorld()->isThereBoulder(getX(), getY()-4, 0) || getY()==0)
			die();
		else moveTo(getX(), getY() - 1);
		annoyMan();
	}
}

void Boulder::annoyMan()
{
	if (getWorld()->isPlayerInRadius(this, 3)) {
		getWorld()->getPlayer()->isAnnoyed(100);
	}
	Protester * p = getWorld()->protesterInRadius(this, 3);
	if (p != nullptr) {
		p->isAnnoyed(100);
	}
}

//Squirt
Squirt::Squirt(StudentWorld* world, int startX, int startY, Direction dir)
	:Actor(world, TID_WATER_SPURT, startX, startY, dir, 1.0, 1),m_travel(0) {}
void Squirt::doSomething() {
	if (!isAlive()) return;
	if (annoyProtesters()||m_travel == 4 ) {
		die();
		return;
	}
	switch (getDirection()) {
	case left:
		if (getWorld()->isThereEarth(getX() - 1, getY()) || getWorld()->isThereBoulder(getX() - 1, getY())) {
			die(); return;
		}
		else   moveTo(getX() - 1, getY());
		break;
	case right:
		if (getWorld()->isThereEarth(getX() + 1, getY()) || getWorld()->isThereBoulder(getX() + 1, getY())) {
			die(); return;
		}
		else   moveTo(getX() + 1, getY());  
		break;
	case up:
		if (getWorld()->isThereEarth(getX(), getY() + 1) || getWorld()->isThereBoulder(getX(), getY() + 1)) {
			die(); return;
		}
		else moveTo(getX(), getY() + 1);
		break;
	case down:
		if (getWorld()->isThereEarth(getX(), getY() - 1) || getWorld()->isThereBoulder(getX(), getY() - 1)) {
			die(); return;
		}
		else moveTo(getX(), getY() - 1);
		break;
    case none : return;
	}
	m_travel++;
}

bool Squirt::annoyProtesters()
{
	Protester * p= getWorld()->protesterInRadius(this,3);
	if (p != nullptr) {
		p->isAnnoyed(2);
		return true;
	}
	return false;
}


//Pickupable
Pickupable::Pickupable(StudentWorld* world, int imageID, int startX, int startY)
	:Actor(world, imageID, startX, startY, right, 1.0, 2) {}

void Pickupable::disappearIn(int ticks)
{
	if (m_tick == ticks) die();
	else m_tick++;
}

//Barrel
Barrel::Barrel(StudentWorld* world, int startX, int startY)
	:Pickupable(world, TID_BARREL, startX, startY)
{
	setVisible(false);
}
void Barrel::doSomething() {
	if (!isAlive()) return;
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
		setVisible(true);
		return;
	}
	if (getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrel();
		return;
	}
}

//Gold
Gold::Gold(StudentWorld* world, int startX, int startY, bool isVisible, bool dropped)
	:Pickupable(world, TID_GOLD, startX, startY), isDropped(dropped)
{
	setVisible(isVisible);
}
void Gold::doSomething() {
	if (!isAlive()) return;
	if (!isVisible() && getWorld()->isPlayerInRadius(this, 4)) {
		setVisible(true);
		return;
	}
	if (!isDropped && getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(10);
		return;
	}
	if (isDropped) {
		Protester* p = (getWorld()->protesterInRadius(this, 3));
		if (p != nullptr) {
			die();
			p->getBribed();
		};
		disappearIn(100);
	}
}

//Goodie
Goodie::Goodie(StudentWorld* world, int imageID, int startX, int startY)
	:Pickupable(world, imageID, startX, startY) {}
void Goodie::doSomething(){
	if (!isAlive()) return;
	if (getWorld()->isPlayerInRadius(this, 3)) {
		die();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getPlayer()->add(getID());
		getWorld()->increaseScore(75);
		return;
	}
	disappearIn(std::max(100, 300 - 10 * (int)getWorld()->getLevel()));
}

//Sonar
Sonar::Sonar(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_SONAR, startX, startY) {}

//WaterPool
WaterPool::WaterPool(StudentWorld* world, int startX, int startY)
	:Goodie(world, TID_WATER_POOL, startX, startY) {}


//Protester
Protester::Protester(StudentWorld* world, int imageID, int hP)
	: Man(world, imageID, 60, 60, left, hP), m_leave(false), m_tickSinceLastTurn(200), m_tickNoYell(15)
{
	randomNumToMove();
	m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
}
void Protester::randomNumToMove()
{
	m_numToMove = rand() % 53 + 8;
}
void Protester::doSomething()
{	//step 1
	if (!isAlive()) return;
	//step 2
	if (m_tickRest > 0) {
		m_tickRest--;
		return;
	}
	else {
		m_tickRest = max(0, 3 - (int)getWorld()->getLevel() / 4);
		m_tickSinceLastTurn++;
		m_tickNoYell++;
	}
	//step 3
	if (m_leave) {
		if (getX() == 60 && getX() == 60) {
			die();
            getWorld()->decProtester();
			return;
		}
        getWorld()->moveToExit(this);// move one square closer to exit
		return;	
	}

	// step 4  shout at player

	if (getWorld()->isPlayerInRadius(this, 4) && isFacingPlayer()) {
		if (m_tickNoYell > 15) {
			getWorld()->getPlayer()->isAnnoyed(2);
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			m_tickNoYell = 0;
			return;
		}
		return;
	}
    // only for hardCore protester
    if(getID()==TID_HARD_CORE_PROTESTER){
        int M =16 + int(getWorld()->getLevel());
        Direction s= getWorld()->senseSignalFromPlayer(this, M);
        if(s != none){
            moveInDirection(s);
            return;
        }
    }
	// step 5   line of sight to player and go to find player
	Direction d = directionToPlayer();
	if (d != none && straightPathToPlayer(d) && (!getWorld()->isPlayerInRadius(this, 4))) {
		setDirection(d);
		moveInDirection(d);
		m_numToMove = 0;
		return;
	}
	// step 6
	m_numToMove--;
	if (m_numToMove <= 0) {
		Direction k = none;
		k = randomDirection();
		while (true) {
			k = randomDirection();
			if (getWorld()->canMoveInDirection(getX(),getY(), k)) break;
		}
		setDirection(k);
		randomNumToMove();
	}
	// step 7
	else if (isAtIntersection() && m_tickSinceLastTurn > 200) {
		pickViableDirectionToTurn();
		m_tickSinceLastTurn = 0;
		randomNumToMove();
	}
	// step 8
	moveInDirection(getDirection());
	// step 9
	if (!getWorld()->canMoveInDirection(getX(),getY(),getDirection())) {
		m_numToMove = 0;
	}
}

void Protester::moveInDirection(Direction direction)
{
	switch (direction) {
	case left:
		if (getDirection() == left) {
			if (getX() == 0) setDirection(right);
				moveTo(getX() - 1, getY());
		}
		else setDirection(left);
		break;
	case right:
		if (getDirection() == right) {
			if (getX() == 60) setDirection(left);
				moveTo(getX() + 1, getY());
		}
		else setDirection(right);
		break;
	case up:
		if (getDirection() == up) {
			if (getY() == 60) setDirection(down);
			moveTo(getX(), getY() + 1);
		}
		else setDirection(up);
		break;
	case down:
		if (getDirection() == down) {
			if (getY() == 0) setDirection(up);
			moveTo(getX(), getY() - 1);
		}
		else setDirection(down);
		break;
    case none: return;
	}
}



GraphObject:: Direction Protester::directionToPlayer()
{
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getY();
	if (getY() == playerY && getX() == playerX)
	{
		return getDirection();
	}
	if (getX() == playerX) {
		if (getY() < playerY)
			return up;
		if (getY() > playerY)
			return down;
	}
	if (getY() == playerY) {
		if (getX() > playerX)
			return left;
		if (getY() < playerY)
			return right;
	}
	
	return none;   // didnt see player in straight line
}

bool Protester::straightPathToPlayer(Direction direction)
{
	int playerX = getWorld()->getPlayer()->getX();
	int playerY = getWorld()->getPlayer()->getX();
	switch (direction) {
	case left:
		for (int i = getX(); i >= playerX; i--) {
			if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
				return false;
		}
		return true;
		break;	
	case right:
		for (int i = getX(); i <= playerX; i++) {
			if (getWorld()->isThereEarth(i, getY()) || getWorld()->isThereBoulder(i, getY()))
				return false;
		}
		return true;
		break;
	case up:
		for (int j = getY(); j <= playerY; j++) {
			if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case down:
		for (int j = getY(); j >= playerY; j--) {
			if (getWorld()->isThereEarth(getX(), j) || getWorld()->isThereBoulder(getX(), j))
				return false;
		}
		return true;
		break;
	case none:
		return false;
	}
}
GraphObject::Direction Protester::randomDirection()
{
	int r;
	r = rand() % 4;
	switch (r) {
		case 0: return left; 
		case 1: return  right;
		case 2: return up;
		case 3: return down;
	}
    return none;
}
bool Protester::isAtIntersection()
{
	if (getDirection() == up || getDirection() == down) {
		return (getWorld()->canMoveInDirection(getX(),getY(), left) || getWorld()->canMoveInDirection(getX(),getY(), right));
	}
	else
		return (getWorld()->canMoveInDirection(getX(),getY(), up) || getWorld()->canMoveInDirection(getX(),getY(), down));
}
void Protester::pickViableDirectionToTurn()
{
	if (getDirection() == up || getDirection() == down) {
		if (!getWorld()->canMoveInDirection(getX(),getY(), left)) setDirection(right);
		else if (!getWorld()->canMoveInDirection(getX(),getY(), right)) setDirection(left);
		else {
			switch (rand() % 2) {
			case 0: setDirection(left);
			case 1: setDirection(right);
			}
		}
	}
	else {
		if (!getWorld()->canMoveInDirection(getX(),getY(), up)) setDirection(down);
		else if (!getWorld()->canMoveInDirection(getX(),getY(), down)) setDirection(up);
		else {
			switch (rand() % 2) {
			case 0: setDirection(up);
			case 1: setDirection(down);
			}
		}
	}

}
void Protester::isAnnoyed(int hP)
{
    if(m_leave) return;
	decHP(hP);
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	getStunned();
    if (getHP() <= 0) {
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_leave = true;
        m_tickRest=0;
        if(hP==100) getWorld()->increaseScore(500); // hit by boulder
        else if(getID()==TID_PROTESTER) getWorld()->increaseScore(100);
        else getWorld()->increaseScore(250);
    }
}
void Protester::getStunned()
{
	m_tickRest = max(50, 100 - (int)getWorld()->getLevel()*10);
}
void Protester::getBribed()
{
    getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
    if(getID()==TID_PROTESTER) {
        getWorld()->increaseScore(25);
        m_leave = true;
    }
    else {
        getWorld()->increaseScore(50);
        m_tickRest = max(50,100- int(getWorld()->getLevel())*10);
    }
}

bool Protester::isFacingPlayer()
{
	switch (getDirection()) {
		case left: 
			return getWorld()->getPlayer()->getX() <= getX(); 
		case right:
			return getWorld()->getPlayer()->getX() >= getX();
		case up:
			return getWorld()->getPlayer()->getY() >= getY();
		case down:
			return getWorld()->getPlayer()->getY() <= getY();
        case none:
            return false;
	}
    return false;
}

//RegularProtester
RegularProtester::RegularProtester(StudentWorld* world)
	: Protester(world, TID_PROTESTER, 5) {}



//HardcoreProtester
HardcoreProtester::HardcoreProtester(StudentWorld* world)
	: Protester(world, TID_HARD_CORE_PROTESTER, 20) {}





