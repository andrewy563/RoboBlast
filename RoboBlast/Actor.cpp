#include "Actor.h"
#include "StudentWorld.h"

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
Wall::Wall(int startX, int startY) : solidObject(IID_WALL, startX, startY) {
	setVisible(true);
}