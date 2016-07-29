#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementation
Actor::Actor(int imageID, int startX, int startY, Direction startDirection = none) : 
	GraphObject(imageID, startX, startY, startDirection) {
	m_alive = true;
}
bool Actor::alive() {
	return m_alive;
}
void Actor::setDead() {
	m_alive = false;
	setVisible(false);
}

ImmobileObject::ImmobileObject(int imageID, int startX, int startY) : Actor(imageID, startX, startY) {

}
void ImmobileObject::doSomething() {}
int ImmobileObject::bulletCollision() {
	return 2;
}
bool ImmobileObject::passThrough() {
	return true;
}

Item::Item(int imageID, int startX, int startY, StudentWorld* world) : ImmobileObject(imageID, startX, startY) {
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
	if (m_world->player()->getX() == getX() && m_world->player()->getY() == getY()) {
		itemPickUp();
		setDead();
	}
}

Gold::Gold(int startX, int startY, StudentWorld* world) : Item(IID_GOLD, startX, startY, world) {}

void Gold::itemPickUp() {
	world()->increaseScore(100);
	world()->playSound(SOUND_GOT_GOLD);
}
RestoreHealth::RestoreHealth(int startX, int startY, StudentWorld* world) : Item(IID_RESTORE_HEALTH, startX, startY, world) {}

void RestoreHealth::itemPickUp() {
	world()->increaseScore(100);
	world()->player()->setHealth(20);
	world()->playSound(SOUND_GOT_GOODIE);
}

Ammo::Ammo(int startX, int startY, StudentWorld* world) : Item(IID_AMMO, startX, startY, world) {}

void Ammo::itemPickUp() {
	world()->increaseScore(150);
	world()->player()->increaseAmmo(25);
	world()->playSound(SOUND_GOT_GOODIE);
}

ExtraLife::ExtraLife(int startX, int startY, StudentWorld* world) : Item(IID_EXTRA_LIFE, startX, startY, world) {}

void ExtraLife::itemPickUp() {
	world()->increaseScore(150);
	world()->incLives();
	world()->playSound(SOUND_GOT_GOODIE);
}

SolidObject::SolidObject(int imageID, int startX, int startY) : ImmobileObject(imageID, startX, startY) {}
int SolidObject::bulletCollision() {
	return 0;
}
bool SolidObject::passThrough() {
	return false;
}


MobileObject::MobileObject(int imageID, int startX, int startY, Direction startDirection, int health, StudentWorld* world) 
	: Actor(imageID, startX, startY, startDirection) {
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
		b = new Bullet(getX() - 1, getY(), left, m_world);
		break;
	case right:
		b = new Bullet(getX() + 1, getY(), right, m_world);
		break;
	case up:
		b = new Bullet(getX(), getY() + 1, up, m_world);
		break;
	case down:
		b = new Bullet(getX(), getY() - 1, down, m_world);
		break;
	}
	world()->placeObject(b);
}
void MobileObject::setHealth(int newh) {
	m_health = newh;
}

Player::Player(int startX, int startY, int sub, StudentWorld* world) : MobileObject(IID_PLAYER, startX, startY, right, 20, world) { 
	ammunition = 0; 
	subLevel = sub; 
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
			if (world()->canIPass(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			if (world()->canIPass(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			if (world()->canIPass(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			if (world()->canIPass(getX(), getY() - 1))
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
Bullet::Bullet(int startX, int startY, Direction startDir, StudentWorld* world) : Actor(IID_BULLET, startX, startY, startDir) {
	m_world = world;
	setVisible(true);
}

void Bullet::doSomething() {
	if (!alive()) {
		return;
	}

	int check = 2;
	check = m_world->bulletColCheck(getX(), getY());
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
	check = m_world->bulletColCheck(getX(), getY());
	if (check == 1 || check == 0) {
		setDead();
		return;
	}
}

int Bullet::bulletCollision() {
	return 2;
}
bool Bullet::passThrough() {
	std::cerr << "Object has passed through a bullet." << std::endl;
	return true;
}

Wall::Wall(int startX, int startY) : SolidObject(IID_WALL, startX, startY) {
	setVisible(true);
}

FakeWall::FakeWall(int startX, int startY) : ImmobileObject(IID_FAKE_WALL, startX, startY) {
	setVisible(true);
}