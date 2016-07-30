#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementation
Actor::Actor(int imageID, int startX, int startY, int sublevel, Direction startDirection = none) :
	GraphObject(imageID, startX, startY, startDirection, sublevel) {
	m_alive = true;
	m_sublevel = sublevel;
}
bool Actor::alive() {
	return m_alive;
}
void Actor::setDead() {
	m_alive = false;
	setVisible(false);
}
int Actor::sublevel() {
	return m_sublevel;
}
void Actor::moveSubLevel(int in) {
	if (getID() == IID_PLAYER)					// only the player should be moving through sublevels
		m_sublevel = in;
}

ImmobileObject::ImmobileObject(int imageID, int startX, int startY, int sublevel) : Actor(imageID, startX, startY, sublevel) {}
void ImmobileObject::doSomething() {}
int ImmobileObject::bulletCollision() {
	return 2;
}
bool ImmobileObject::passThrough() {
	return true;
}

Item::Item(int imageID, int startX, int startY, StudentWorld* world, int sublevel) : ImmobileObject(imageID, startX, startY, sublevel) {
	m_world = world;
	setVisible(true);
}
StudentWorld* Item::world() {
	return m_world;
}
void Item::doSomething() {
	if (!alive()) {
		return;
	}
	if (m_world->player()->getX() == getX() && m_world->player()->getY() == getY() && m_world->player()->sublevel() == sublevel()) {
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

SolidObject::SolidObject(int imageID, int startX, int startY, int sublevel) : ImmobileObject(imageID, startX, startY, sublevel) {}
int SolidObject::bulletCollision() {
	return 0;
}
bool SolidObject::passThrough() {
	return false;
}


MobileObject::MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world, int sublevel)
	: Actor(imageID, startX, startY,  sublevel, startDirection) {
	m_health = health;
	m_world = world;
}

bool MobileObject::passThrough() {
	return false;
}

int MobileObject::bulletCollision() {
	m_health -= 2;
	if (m_health <= 0) {
		setDead();
	}
	return 1;
}
int MobileObject::health() {
	return m_health;
}
StudentWorld* MobileObject::world() {
	return m_world;
}

void MobileObject::fire() {
	Bullet* b;
	switch (getDirection()) {
	case left:
		b = new Bullet(getX() - 1, getY(), left, m_world, sublevel());
		break;
	case right:
		b = new Bullet(getX() + 1, getY(), right, m_world, sublevel());
		break;
	case up:
		b = new Bullet(getX(), getY() + 1, up, m_world, sublevel());
		break;
	case down:
		b = new Bullet(getX(), getY() - 1, down, m_world, sublevel());
		break;
	}
	world()->placeObject(b, sublevel());
}
void MobileObject::setHealth(int newh) {
	m_health = newh;
}

Player::Player(int startX, int startY, StudentWorld* world, int sublevel) : MobileObject(IID_PLAYER, startX, startY, right, 20, world, sublevel) {
	ammunition = 0; 
	setVisible(true); 
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
Bullet::Bullet(int startX, int startY, Direction startDir, StudentWorld* world, int sublevel) : Actor(IID_BULLET, startX, startY, sublevel, startDir) {
	m_world = world;
	setVisible(true);
}

void Bullet::doSomething() {
	if (!alive()) {
		return;
	}
	int check = 2;
	check = m_world->bulletColCheck(getX(), getY(), sublevel());
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
	check = m_world->bulletColCheck(getX(), getY(), sublevel());
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

Wall::Wall(int startX, int startY, int sublevel) : SolidObject(IID_WALL, startX, startY, sublevel) {
	setVisible(true);
}

FakeWall::FakeWall(int startX, int startY, int sublevel) : ImmobileObject(IID_FAKE_WALL, startX, startY, sublevel) {
	setVisible(true);
}