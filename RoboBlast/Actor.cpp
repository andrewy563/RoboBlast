#include "Actor.h"
#include "StudentWorld.h"
#include <cstdlib>
#include <ctime>
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
void Actor::kill() {
	m_alive = false;
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
	Actor* p;
	while (world()->checkGrid(9, IID_GANGSTER, sublevel(), this, p)) {
		p->setDead();
	}
	while (world()->checkGrid(9, IID_BULLY, sublevel(), this, p)) {
		p->setDead();
	}
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
	m_tick = 0;
	if (m_moveRate < 3)
		m_moveRate = 3;
}

bool Enemy::doIRest() {
	m_tick += 1;
	if (m_tick % m_moveRate == 0) {
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
bool Enemy::move() {
	switch (getDirection()) {
	case left:
		if (world()->canIPass(getX() - 1, getY(), sublevel())) {
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
		
	case right:
		if (world()->canIPass(getX() + 1, getY(), sublevel())) {
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	case up:
		if (world()->canIPass(getX(), getY() + 1, sublevel())) {
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;
	case down:
		if (world()->canIPass(getX(), getY() - 1, sublevel())) {
			moveTo(getX(), getY() - 1);
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
	if (!move()) {
		switch (getDirection()) {
		case left:
			setDirection(right);
			break;
		case right:
			setDirection(left);
			break;
		case up:
			setDirection(down);
			break;
		case down:
			setDirection(up);
			break;
		}
	}
}

void Gangster::setDead() {
	setVisible(false);
	kill();
	Waterpool* w = new Waterpool(getX(), getY(), sublevel());
	world()->placeObject(w, sublevel());
	world()->playSound(SOUND_ENEMY_DIE);
}

Robot::Robot(int startX, int startY, StudentWorld* world, int sublevel) 
	: Gangster(startX, startY, 1, world, sublevel, 50, IID_ROBOT_BOSS) {}

void Robot::setDead() {
	setVisible(false);
	kill();
	Hostage* h = new Hostage(getX(), getY(), world(), sublevel());
	world()->placeObject(h, sublevel());
	world()->playSound(SOUND_ENEMY_DIE);
}

Bully::Bully(int startX, int startY, StudentWorld* world, int sublevel)
	: Enemy(IID_BULLY, startX, startY, right, 5, world, sublevel) {
	m_goodies[0] = 0;
	m_goodies[1] = 0;
	m_goodies[2] = 0;
	std::cerr << m_goodies[1] << std::endl;
	srand(time(NULL));
	TURN_DISTANCE = (((rand()) * this->world()->getTick()) % 6) + 1;
}

void Bully::doSomething() {
	if (!alive())
		return;
	if (doIRest()) {
		return;
	}
	if (eyeSight()) {
		world()->playSound(SOUND_ENEMY_FIRE);
		fire();
		return;
	}
	Actor* p;
	if (world()->checkGrid(1, IID_RESTORE_HEALTH, sublevel(), this, p) ||
		world()->checkGrid(1, IID_AMMO, sublevel(), this, p) ||
		world()->checkGrid(1, IID_EXTRA_LIFE, sublevel(), this, p)) {
		if (p->getID() == IID_RESTORE_HEALTH) {
			m_goodies[0] += 1;
		}
		else if (p->getID() == IID_AMMO) {
			m_goodies[1] += 1;
		}
		else if (p->getID() == IID_EXTRA_LIFE) {
			m_goodies[2] += 1;
		}
		if (p != nullptr) {
			p->setDead();
		}
		world()->playSound(SOUND_BULLY_MUNCH);
		p->setDead();
		return;
	}
	if (TURN_DISTANCE != 0) {
		if (!move())
			TURN_DISTANCE = 0;
		else {
			TURN_DISTANCE -= 1;
			return;
		}
	}
	if (TURN_DISTANCE == 0) {
		srand(time(NULL));
		TURN_DISTANCE = (((rand()) * this->world()->getTick()) % 6) + 1;
		int d = rand() % 4;
		Direction list[4] = { left, right, up, down };
		int count = 0;
		while (count < 4) {
			setDirection(list[(d + count) % 4]);			
			if (move()) {
				return;
			}
			count++;
		}
	}
}
void Bully::setDead() {
	kill();
	setVisible(false);
	world()->playSound(SOUND_ENEMY_DIE);
	for (int i = 0; i < m_goodies[0]; i++) {
		RestoreHealth* p = new RestoreHealth(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
	for (int i = 0; i < m_goodies[1]; i++) {
		Ammo* p = new Ammo(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
	for (int i = 0; i < m_goodies[2]; i++) {
		ExtraLife* p = new ExtraLife(getX(), getY(), world(), sublevel());
		world()->placeObject(p, sublevel());
	}
}
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
			if (ammunition > 0) {
				ammunition -= 1;
				world()->playSound(SOUND_PLAYER_FIRE);
				fire();
			}
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
void Player::setAmmo(int n) {
	ammunition = n;
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

Nest::Nest(int startX, int startY, StudentWorld* world, int sublevel) : SolidObject(IID_BULLY_NEST, startX, startY, sublevel) {
	m_world = world;
}

void Nest::doSomething() {
	srand(time(NULL));
	int num = (rand()) % 50 + 1;
	if (m_world->countGrid(7, IID_BULLY, sublevel(), this) < 3 && m_world->countGrid(1, IID_BULLY, sublevel(), this) == 0) {
		int chance = ((rand() * m_world->getTick()) % 50) + 1;
		if (chance == num) {
			Bully * b = new Bully(getX(), getY(), m_world, sublevel());
			m_world->placeObject(b, sublevel());
			m_world->playSound(SOUND_BULLY_BORN);
		}
	}
}