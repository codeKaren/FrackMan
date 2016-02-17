#include "Actor.h"
#include "StudentWorld.h"
#include <iostream> // TESTING ONLY

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

void Actor::makeDead()
{
    m_stillAlive = false;
}

StudentWorld* Actor::whereAmI() const
{
    return m_studentWorld;
}

bool Actor::isObstacle() const
{
    return false;
}

// DIRT IMPLEMENTATION ===========================================================================================

Dirt::Dirt(int startX, int startY, StudentWorld* world)
: Actor(IID_DIRT, startX, startY, right, 0.25, 3, true, world)
{ }

Dirt::~Dirt()   // do i need to add any code to this??
{ }

void Dirt::doSomething()
{ }

// BOULDER IMPLEMENTATION =====================================================================================

Boulder::Boulder(int startX, int startY, StudentWorld* world)
: Actor(IID_BOULDER, startX, startY, down, 1.0, 1, true, world)
{
    m_stableState = true;
    numTicksWaiting = 0;
    whereAmI()->addActor(this);
    whereAmI()->deleteDirt(this);
}
        
Boulder::~Boulder()
{
    std::cout << "Boulder deleted" << std::endl;
}
        
void Boulder::doSomething()
{
    if (!isStillAlive())
        return;
    if (m_stableState == true)   // boulder is currently in a stable state
    {
        // check to see if there is any dirt below the boulder
        int numMissingDirt = 0;
        for (int i = getX(); i < getX() + 4; i++)
        {
            if (getY() - 1 < 0)   // going out of bounds below the below the boulder
                break;
            if (i > 64)  // going out of bounds beside the boulder
                break;
            if (! whereAmI()->isThereDirt(i, getY()-1))    // no dirt in the position below
                numMissingDirt++;
        }
        if (numMissingDirt == 4)          // no dirt at all under the boulder
        {
            m_stableState = false;
        }
    }
    
    if (m_stableState == false)
    {
        if (numTicksWaiting >= 30)
        {
            if (numTicksWaiting == 30)
                whereAmI()->playSound(SOUND_FALLING_ROCK);       // only plays the sound one time
            if (getY()-1 < 0)         // hits the bottom of the oil field
            {
                makeDead();
                return;
            }
            for (int i = getX(); i < getX() + 4; i++)   // make sure to check the whole width of the boulder
            {
                if (whereAmI()->isThereDirt(i, getY()-1))  // runs into dirt under it
                {
                    makeDead();
                    return;
                }
                
                if (whereAmI()->isThereObstacle(i, getY()-1))   // runs into an obstacle below it
                {
                    makeDead();
                    return;
                }
            }
            // the boulder doesn't have anything underneath it, so it can fall
            moveTo(getX(), getY()-1);
            
            // ADD CODE FOR WHEN BOULDER HITS PROTESTORS, ETC.
        }
            numTicksWaiting++;
    }
}

bool Boulder::isObstacle() const
{
    return true;
}

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

void FrackMan::doSomething() 
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
                else if(whereAmI()->isThereObstacle(getX()-1, getY()))  // can't dig into boulder
                        break;
                else
                    moveTo(getX()-1, getY());     // since it is a valid position, move to that position
                if (whereAmI()->deleteDirt(this))        // if you dig into dirt, play the digging sound
                    whereAmI()->playSound(SOUND_DIG);
                break;
                
            case KEY_PRESS_RIGHT:
                setDirection(right);
                if (getX()+1 < 0 || getX()+1 > 60)
                    moveTo(getX(), getY());
                else if(whereAmI()->isThereObstacle(getX()+4, getY()))  // must do +4 because getX() is the lower left corner
                    break;
                else
                    moveTo(getX()+1, getY());
                if (whereAmI()->deleteDirt(this))
                    whereAmI()->playSound(SOUND_DIG);
                break;
                
            case KEY_PRESS_DOWN:
                setDirection(down);
                if (getY()-1 < 0 || getY()-1 > 60)
                    moveTo(getX(), getY());
                else if(whereAmI()->isThereObstacle(getX(), getY()-1))
                    break;
                else
                    moveTo(getX(), getY()-1);
                if (whereAmI()->deleteDirt(this))
                    whereAmI()->playSound(SOUND_DIG);
                break;
            case KEY_PRESS_UP:
                setDirection(up);
                if (getY()+1 < 0 || getY()+1 > 60)
                    moveTo(getX(), getY());
                else if(whereAmI()->isThereObstacle(getX(), getY()+4))
                    break;
                else
                    moveTo(getX(), getY()+1);
                if (whereAmI()->deleteDirt(this))
                    whereAmI()->playSound(SOUND_DIG);
                break;
        }
    }
}