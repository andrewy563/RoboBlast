#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementation
Actor::Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection = none, StudentWorld* world = nullptr) :
	GraphObject(imageID, startX, startY, startDirection, sublevel) {
	m_alive = true;
	m_sublevel = sublevel;
	setVisible(true);
	m_world = world;
}
bool Actor::alive() {
	return m_alive;
}
void Actor::setDead() {
	m_alive = false;
	setVisible(false);
	if (getID() == IID_ROBOT_BOSS) {
		Hostage* h = new Hostage(getX(), getY(), world(), sublevel());
		world()->placeObject(h, sublevel());
	}
	if (getID() == IID_GANGSTER) {
		Waterpool* w = new Waterpool(getX(), getY(), sublevel());
		world()->placeObject(w, sublevel());
	}
}
int Actor::sublevel() {
	return m_sublevel;
}
void Actor::moveSubLevel(int in) {
	if (getID() == IID_PLAYER)					// only the player should be moving through sublevels
		m_sublevel = in;
}
void Actor::activate(bool on) {
	ACTIVE = on;
	setVisible(on);
}
bool Actor::active() {
	return ACTIVE;
}
StudentWorld* Actor::world() {
	return m_world;
}

ImmobileObject::ImmobileObject(int imageID, int startX, int startY, int sublevel, StudentWorld* world = nullptr) 
	: Actor(imageID, startX, startY, sublevel, none, world) {}
void ImmobileObject::doSomething() {}
int ImmobileObject::bulletCollision() {
	return 2;
}
bool ImmobileObject::passThrough() {
	return true;
}

Item::Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel) 
	: ImmobileObject(imageID, startX, startY, sublevel, world) {
}
void Item::doSomething() {
	if (!alive()) {
		return;
	}
	if (world()->player()->getX() == getX() && world()->player()->getY() == getY() && world()->player()->sublevel() == sublevel() && active()) {
		itemPickUp();
		setDead();
	}
}



Gate::Gate(int startX, int startY, StudentWorld* world, int sublevel, int destination) : Item(IID_GATE, startX, startY, world, sublevel) {
	m_dest = destination;
}

void Gate::itemPickUp() {
	world()->setSubLevel(m_dest);
	world()->player()->moveSubLevel(m_dest);
}

Exit::Exit(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_EXIT, startX, startY, world, sublevel) {
	activate(false);
}

void Exit::itemPickUp() {
	world()->increaseScore(1500);
	world()->endLevel();
}

Hostage::Hostage(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_HOSTAGE, startX, startY, world, sublevel) {}

void Hostage::itemPickUp() {
	world()->playSound(SOUND_GOT_GOODIE);
}

Gold::Gold(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_GOLD, startX, startY, world, sublevel) {}

void Gold::itemPickUp() {
	world()->increaseScore(100);
	world()->playSound(SOUND_GOT_GOLD);
}
RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_RESTORE_HEALTH, startX, startY, world, sublevel) {}

void RestoreHealth::itemPickUp() {
	world()->increaseScore(100);
	world()->player()->setHealth(20);
	world()->playSound(SOUND_GOT_GOODIE);
}

Ammo::Ammo(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_AMMO, startX, startY, world, sublevel) {}

void Ammo::itemPickUp() {
	world()->increaseScore(150);
	world()->player()->increaseAmmo(25);
	world()->playSound(SOUND_GOT_GOODIE);
}

ExtraLife::ExtraLife(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_EXTRA_LIFE, startX, startY, world, sublevel) {}

void ExtraLife::itemPickUp() {
	world()->increaseScore(150);
	world()->incLives();
	world()->playSound(SOUND_GOT_GOODIE);
}

Farplane::Farplane(int startX, int startY, StudentWorld* world, int sublevel) : Item(IID_FARPLANE_GUN, startX, startY, world, sublevel) {}

void Farplane::itemPickUp() {
	world()->player()->damage(10);
	world()->playSound(SOUND_GOT_FARPLANE_GUN);
}

SolidObject::SolidObject(int imageID, int startX, int startY, int sublevel) : ImmobileObject(imageID, startX, startY, sublevel) {}
int SolidObject::bulletCollision() {
	return 0;
}
bool SolidObject::passThrough() {
	return false;
}


MobileObject::MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel)
	: Actor(imageID, startX, startY,  sublevel, startDirection, world) {
	m_health = health;
}

bool MobileObject::passThrough() {
	return false;
}

int MobileObject::bulletCollision() {
	m_health -= 2;
	if (m_health <= 0) {
		setDead();
		if (getID() == IID_PLAYER) {
			world()->playSound(SOUND_PLAYER_DIE);
		}
		else
			world()->playSound(SOUND_ENEMY_DIE);
		return 1;
	}
	if (getID() == IID_PLAYER) {
		world()->playSound(SOUND_PLAYER_IMPACT);
	}
	else {
		world()->playSound(SOUND_ENEMY_IMPACT);
	}
	return 1;
}
int MobileObject::health() {
	return m_health;
}

void MobileObject::fire() {
	Bullet* b;
	switch (getDirection()) {
	case left:
		b = new Bullet(getX() - 1, getY(), left, world(), sublevel());
		break;
	case right:
		b = new Bullet(getX() + 1, getY(), right, world(), sublevel());
		break;
	case up:
		b = new Bullet(getX(), getY() + 1, up, world(), sublevel());
		break;
	case down:
		b = new Bullet(getX(), getY() - 1, down, world(), sublevel());
		break;
	}
	world()->placeObject(b, sublevel());
}
void MobileObject::setHealth(int newh) {
	m_health = newh;
}

Enemy::Enemy(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel)
	: MobileObject(imageID, startX, startY, startDirection, health, world, sublevel) {
	m_moveRate = (28 - this->world()->getLevel()) / 4;
	if (m_moveRate < 3)
		m_moveRate = 3;
}

bool Enemy::doIRest() {
	if (world()->getTick() % m_moveRate == 0) {
		return false;
	}
	return true;
}
bool Enemy::eyeSight() {
	int x = getX();
	int y = getY();
	switch (getDirection()) {
	case right:
		while (x < VIEW_WIDTH) {
			x += 1;
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}
			if (world()->player()->getX() == x && world()->player()->getY() == y)
				return true;
		}
		break;
	case left:
		while (x > 0) {
			x -= 1;
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}
			if (world()->player()->getX() == x && world()->player()->getY() == y)
				return true;
		}
		break;	
	case up:
			while (y < VIEW_WIDTH) {
				y += 1;
				if (!world()->canIPass(x, y, sublevel())) {
					return false;
				}
				if (world()->player()->getX() == x && world()->player()->getY() == y)
					return true;
			}
			break;
	case down:
		while (y > 0) {
			y -= 1;
			if (!world()->canIPass(x, y, sublevel())) {
				return false;
			}
			if (world()->player()->getX() == x && world()->player()->getY() == y)
				return true;
		}
		break;
	}
	return false;
}

Gangster::Gangster(int startX, int startY, int startDirection, StudentWorld* world, int sublevel, int health, int imageID) 
	: Enemy(imageID, startX, startY, right, health, world, sublevel) {
	if (startDirection == 2) {
		setDirection(down);
	}
}

void Gangster::doSomething() {
	if (!alive() || doIRest()) {
		return;
	}
	if (eyeSight()) {
		world()->playSound(SOUND_ENEMY_FIRE);
		fire();
		return;
	}
	switch (getDirection()) {
	case left:
		if (world()->canIPass(getX() - 1, getY(), sublevel()))
			moveTo(getX() - 1, getY());
		else
			setDirection(right);
		break;
	case right:
		if (world()->canIPass(getX() + 1, getY(), sublevel()))
			moveTo(getX() + 1, getY());
		else
			setDirection(left);
		break;
	case up:
		if (world()->canIPass(getX(), getY() + 1, sublevel()))
			moveTo(getX(), getY() + 1);
		else
			setDirection(down);
		break;
	case down:
		if (world()->canIPass(getX(), getY() - 1, sublevel()))
			moveTo(getX(), getY() - 1);
		else
			setDirection(up);
		break;
	}
}

Robot::Robot(int startX, int startY, StudentWorld* world, int sublevel) 
	: Gangster(startX, startY, 1, world, sublevel, 50, IID_ROBOT_BOSS) {}


Player::Player(int startX, int startY, StudentWorld* world, int sublevel) : MobileObject(IID_PLAYER, startX, startY, right, 20, world, sublevel) {
	ammunition = 0; 
}

void Player::doSomething() {
	int ch;
	if (world()->getKey(ch)) {
		switch (ch) {
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
		{
			world()->playSound(SOUND_PLAYER_FIRE);
			fire();
			break; 
		}		
		case KEY_PRESS_LEFT:
		{
			setDirection(left);
			if (world()->canIPass(getX() - 1, getY(), sublevel()))
				moveTo(getX() - 1, getY());
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			if (world()->canIPass(getX() + 1, getY(), sublevel()))
				moveTo(getX() + 1, getY());
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			if (world()->canIPass(getX(), getY() + 1, sublevel()))
				moveTo(getX(), getY() + 1);
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			if (world()->canIPass(getX(), getY() - 1, sublevel()))
				moveTo(getX(), getY() - 1);
			break;
		}
		}
	}
}
int Player::ammo() {
	return ammunition;
}
void Player::increaseAmmo(int n) {
	ammunition += n;
}
void Player::damage(int n) {
	setHealth(health() - n);
}

Bullet::Bullet(int startX, int startY, Direction startDir, StudentWorld* world, int sublevel) : Actor(IID_BULLET, startX, startY, sublevel, startDir, world) {
}

void Bullet::doSomething() {
	if (!alive()) {
		return;
	}
	int check = 2;
	check = world()->bulletColCheck(getX(), getY(), sublevel());
	if (check == 1 || check == 0) {
		setDead();
		return;
	}
	
	switch (getDirection()) {
	case left:
		moveTo(getX() - 1, getY());

		break;
	case right:
		moveTo(getX() + 1, getY());
		break;
	case up:
		moveTo(getX(), getY() + 1);
		break;
	case down:
		moveTo(getX(), getY() - 1);
		break;
	}
	check = world()->bulletColCheck(getX(), getY(), sublevel());
	if (check == 1 || check == 0) {
		setDead();
		return;
	}
}

int Bullet::bulletCollision() {
	return 2;
}
bool Bullet::passThrough() {
	return true;
}

Wall::Wall(int startX, int startY, int sublevel) : SolidObject(IID_WALL, startX, startY, sublevel) {}

FakeWall::FakeWall(int startX, int startY, int sublevel) : ImmobileObject(IID_FAKE_WALL, startX, startY, sublevel) {}

Waterpool::Waterpool(int startX, int startY, int sublevel) : SolidObject(IID_WATERPOOL, startX, startY, sublevel) {
	m_health = 30;
}
void Waterpool::doSomething() {
	if (!alive())
		return;

	m_health -= 1;
	if (m_health <= 0) {
		setDead();
		return;
	}
}

Nest::Nest(int startX, int startY, int sublevel) : SolidObject(IID_BULLY_NEST, startX, startY, sublevel) {}

void Nest::doSomething() {

}