#include "Actor.h"
#include "StudentWorld.h"
#include <iostream>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// Actor Implementation
Actor::Actor(int imageID, int startX, int startY, Direction startDirection = none) : 
	GraphObject(imageID, startX, startY, startDirection) {}

immobileObject::immobileObject(int imageID, int startX, int startY) : Actor(imageID, startX, startY) {

}
void immobileObject::doSomething() {}


solidObject::solidObject(int imageID, int startX, int startY) : immobileObject(imageID, startX, startY) {}
int solidObject::bulletCollision() {
	return 0;
}
bool solidObject::passThrough() {
	return false;
}


mobileObject::mobileObject(int imageID, int startX, int startY, Direction startDirection, int health) : Actor(imageID, startX, startY, startDirection) {}

int mobileObject::damage(int d) {
	m_health -= d;
	if (m_health <= 0) {
		setDead();
	}
	return d;
}
bool mobileObject::passThrough() {
	return false;
}
bool mobileObject::alive() {
	return ALIVE;
}
void mobileObject::setDead() {
	ALIVE = false;
}

int mobileObject::bulletCollision() {
	damage(1);
	return 1;
}

Player::Player(int startX, int startY, int sub, StudentWorld* world) : mobileObject(IID_PLAYER, startX, startY, right, 20) { 
	ammunition = 0; 
	subLevel = sub; 
	setVisible(true); 
	m_world = world;
}

int Player::bulletCollision() {
	damage(2);
	return 1;
}

void Player::doSomething() {
	//if (!alive()) {
		//return;
	//}
	int ch;
	if (m_world->getKey(ch)) {

		switch (ch) {
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			break;
		case KEY_PRESS_LEFT:
		{
			setDirection(left);
			if (m_world->canIPass(getX() - 1, getY()))
				moveTo(getX() - 1, getY());
			break;
		}
		case KEY_PRESS_RIGHT:
		{
			setDirection(right);
			if (m_world->canIPass(getX() + 1, getY()))
				moveTo(getX() + 1, getY());
			break;
		}
		case KEY_PRESS_UP:
		{
			setDirection(up);
			if (m_world->canIPass(getX(), getY() + 1))
				moveTo(getX(), getY() + 1);
			break;
		}
		case KEY_PRESS_DOWN:
		{
			setDirection(down);
			if (m_world->canIPass(getX(), getY() - 1))
				moveTo(getX(), getY() - 1);
			break;
		}
		}
	}
}

Wall::Wall(int startX, int startY) : solidObject(IID_WALL, startX, startY) {
	setVisible(true);
}