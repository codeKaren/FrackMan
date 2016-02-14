#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR IMPLEMENTATION ===========================================================================================

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth)
:GraphObject(imageID, startX, startY, startDirection, size, depth)
{
    m_stillAlive = true;
}

Actor::~Actor()
{
    
}

void Actor::doSomething()
{
    
}

bool Actor::isStillAlive()
{
    return m_stillAlive;
}

// DIRT IMPLEMENTATION ===========================================================================================

Dirt::Dirt(int startX, int startY)
: Actor(IID_DIRT, startX, startY, right, 0.25, 3)
{
    // other stuff I have to implement
    // I am dirt
    // dirt is me
    
    setVisible(true);
}

Dirt::~Dirt()
{
    
}

void Dirt::doSomething()
{ }

// PERSON IMPLEMENTATION =========================================================================================

Person::Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, int HP)
: Actor(imageID, startX, startY, startDirection, size, depth)
{
    m_healthPoints = HP;
}

Person::~Person()
{
}

// FRACKMAN IMPLEMENTATION =======================================================================================

FrackMan::FrackMan()
: Person(IID_PLAYER, 30, 60, right, 1.0, 0, 10)
{
    // ADD MORE STUFF
    m_numSquirts = 5;
    m_numSonars = 1;
    m_numNuggets = 0;
    setVisible(true);
}

FrackMan::~FrackMan()
{
    
}

void FrackMan::doSomething()
{
    
}

