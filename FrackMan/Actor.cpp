#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR IMPLEMENTATION ===========================================================================================

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth)
:GraphObject(0, 0, 0, up)    // GARBAGE
{
    
}

Actor::~Actor()
{
    
}

void Actor::doSomething()
{
    
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

// PERSON IMPLEMENTATION =========================================================================================

Person::Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth)
: Actor(imageID, startX, startY, startDirection, size, depth)
{
    
}

Person::~Person()
{
}

// FRACKMAN IMPLEMENTATION =======================================================================================

FrackMan::FrackMan()
: Person(IID_PLAYER, 30, 60, right, 1.0, 0)
{
    // ADD MORE STUFF
    
    setVisible(true);
}

FrackMan::~FrackMan()
{
    
}

void FrackMan::doSomething()
{
    
}

