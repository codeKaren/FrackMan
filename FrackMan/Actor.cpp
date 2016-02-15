#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR IMPLEMENTATION ===========================================================================================

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, size, depth)
{
    m_stillAlive = true;
    setVisible(visible);
    m_studentWorld = world;
}

Actor::~Actor()
{ }

bool Actor::isStillAlive() const
{
    return m_stillAlive;
}

StudentWorld* Actor::whereAmI() const
{
    return m_studentWorld;
}

// DIRT IMPLEMENTATION ===========================================================================================

Dirt::Dirt(int startX, int startY, StudentWorld* world)
: Actor(IID_DIRT, startX, startY, right, 0.25, 3, true, world)
{ }

Dirt::~Dirt()   // do i need to add any code to this??
{ }

void Dirt::doSomething()
{ }

// PERSON IMPLEMENTATION =========================================================================================

Person::Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, int HP, StudentWorld* world)
: Actor(imageID, startX, startY, startDirection, size, depth, true, world)
{
    m_healthPoints = HP;
}

Person::~Person()
{ }

// FRACKMAN IMPLEMENTATION =======================================================================================

FrackMan::FrackMan(StudentWorld* world)
: Person(IID_PLAYER, 30, 60, right, 1.0, 0, 10, world)
{
    // ADD MORE STUFF
    m_numSquirts = 5;
    m_numSonars = 1;
    m_numNuggets = 0;
}

FrackMan::~FrackMan()
{ }

void FrackMan::doSomething() // moves FrackMan but doesn't delete the dirt (dirt is deleted inside StudentWorld::move())
{
    if (!isStillAlive())
        return;
    int keyPressed;
    if (whereAmI()->getKey(keyPressed) == true)
    {
        switch (keyPressed)
        {
            case KEY_PRESS_LEFT:
                setDirection(left);
                if (getX()-1 < 0 || getX()-1 > 60)    // gone out of bounds
                    moveTo(getX(), getY());   // stay in the same position but maintain animations
                else
                    moveTo(getX()-1, getY());     // since it is a valid position, move to that position
                break;
                
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getX()+1 < 0 || getX()+1 > 60)
                    moveTo(getX(), getY());
                else
                    moveTo(getX()+1, getY());
                break;
                
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getY()-1 < 0 || getY()-1 > 60)
                    moveTo(getX(), getY());
                else
                    moveTo(getX(), getY()-1);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getY()+1 < 0 || getY()+1 > 60)
                    moveTo(getX(), getY());
                else
                    moveTo(getX(), getY()+1);
                break;
        }
    }
}