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
{ }
        
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

// SQUIRT IMPLEMENTATION =========================================================================================

Squirt::Squirt(int x, int y, Direction direction, StudentWorld* world)
:Actor(IID_WATER_SPURT, x, y, direction, 1.0, 1, true, world)
{
    m_travelDistance = 4;
}

Squirt::~Squirt()
{
    
}

void Squirt::doSomething()
{
    // IMPLEMENT LATER: CAUSE 2 POINTS OF ANNOYANCE TO PROTESTORS
    
    if (m_travelDistance == 0)   // travelled full distance
    {
        makeDead();
        return;
    }
    
    else
    {

        // check to see if there is dirt in front of FrackMan (no squirt can be put there)
        for (int i = getX(); i < getX() + 4; i++)
        {
            for (int j = getY(); j < getY() + 4; j++)
            {
                if (whereAmI()->isThereDirt(i, j))    // dirt found
                {
                    makeDead();
                    return;
                }
            }
        }

        // Squirt can be generated in front of FrackMan, so check to see if it can move around
        switch (getDirection())
        {
            case left:
                if (getX()-1 < 0 || getX()-1 > 60)     // gone out of bounds
                {
                    makeDead();
                    return;
                }
                else if(whereAmI()->isThereObstacle(getX()-1, getY()))   // location occupied by boulder
                {
                    makeDead();
                    return;
                }
                else
                {
                    // check to see if there is dirt on the left side
                    for (int k = getY(); k < getY() + 4; k++)
                    {
                        if (whereAmI()->isThereDirt(getX()-1, k))
                        {
                            makeDead();
                            return;
                        }
                    }
                    
                    // no dirt found on the left, so move over one square to the left (to the left, everything you own in a box to the left)
                    moveTo(getX()-1, getY());
                    decreaseDistance();
                    return;
                }
                break;
                
            case right:
                if (getX()+1 < 0 || getX()+1 > 60)
                {
                    makeDead();
                    return;
                }
                else if(whereAmI()->isThereObstacle(getX()+4, getY()))
                {
                    makeDead();
                    return;
                }
                else
                {
                    // check to see if there is dirt on the right side
                    for (int k = getY(); k < getY() + 4; k++)
                    {
                        if (whereAmI()->isThereDirt(getX()+4, k))    // +4 because x,y is the lower left corner
                        {
                            makeDead();
                            return;
                        }
                    }
                    
                    // no dirt found on the right, so move over one square
                    moveTo(getX()+1, getY());
                    decreaseDistance();
                    return;
                }
                break;
            case down:
                if (getY()-1 < 0 || getY()-1 > 60)
                {
                    makeDead();
                    return;
                }
                else if (whereAmI()->isThereObstacle(getX(), getY()-1))
                {
                    makeDead();
                    return;
                }
                else
                {
                    // check to see if there is dirt on the bottom
                    for (int k = getX(); k < getX() + 4; k++)
                    {
                        if (whereAmI()->isThereDirt(k, getY()-1))
                        {
                            makeDead();
                            return;
                        }
                    }
                    
                    // no dirt on the bottom, so move down
                    moveTo(getX(), getY() - 1);
                    decreaseDistance();
                    return;
                }
                break;
            case up:
                if (getY()+1 < 0 || getY()+1 > 60)
                {
                    makeDead();
                    return;
                }
                else if(whereAmI()->isThereObstacle(getX(), getY()+4))
                {
                    makeDead();
                    return;
                }
                else
                {
                    // check to see if there is dirt on the top
                    for (int k = getX(); k < getX() + 4; k++)
                    {
                        if (whereAmI()->isThereDirt(k, getY()+4))
                        {
                            makeDead();
                            return;
                        }
                    }
                    
                    // no dirt on the top, so move up
                    moveTo(getX(), getY() + 1);
                    decreaseDistance();
                    return;
                }
                break;
            case none:
                break;
        }
    }
}

void Squirt::decreaseDistance()
{
    m_travelDistance--;
}

// GOODIES IMPLEMENTATION ========================================================================================

Goodies::Goodies(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, int numTicks, StudentWorld* world)
: Actor(imageID, startX, startY, startDirection, size, depth, visible, world)
{
    setVisible(visible);
    whereAmI()->addActor(this);
    m_visible = visible;
    m_numTicks = numTicks;
}

Goodies::~Goodies()
{
    
}

bool Goodies::isVisible() const
{
    return m_visible;
}

void Goodies::makeVisible()
{
    m_visible = true;
}

int Goodies::howManyTicksLeft() const  // returns how many ticks the goodie has left before it must disappear
{
    return m_numTicks;
}

void Goodies::decreaseNumTicks()  // decreases the number of ticks by one
{
    m_numTicks--; 
}

// OIL BARREL IMPLEMENTATION ======================================================================================

OilBarrel::OilBarrel(int startX, int startY, StudentWorld* world)
: Goodies(IID_BARREL, startX, startY, right, 1.0, 2, false, 0, world)
{

}

OilBarrel::~OilBarrel()
{
    
}

void OilBarrel::doSomething()
{
    if (!isStillAlive())    // barrel is dead, so return right away
        return;
    if (isVisible() == false && whereAmI()->closeToFrackMan(this, 4.00))   // FrackMan is discovers the OilBarrel
    {
        setVisible(true);
        makeVisible();
        return;
    }
    else if (whereAmI()->closeToFrackMan(this, 3.00))   // FrackMan is close enough to pick up the oil barrel
    {
        makeDead();
        whereAmI()->playSound(SOUND_FOUND_OIL);
        whereAmI()->increaseScore(1000);
        whereAmI()->decreaseBarrelNum();
    }
}

// NUGGET IMPLEMENTATION =========================================================================================

Nugget::Nugget(int startX, int startY, bool visible, bool pickupState, bool permanentState, StudentWorld* world)
: Goodies(IID_GOLD, startX, startY, right, 1.0, 2, visible, 100, world)
{
    m_pickupableByFrackMan = pickupState;
    m_permanentState = permanentState;
}

Nugget::~Nugget()
{
    
}

void Nugget::doSomething()
{
    if (!isStillAlive())    // nugget is dead, so return right away
        return;
    if (!isVisible() && whereAmI()->closeToFrackMan(this, 4.00))   // FrackMan discovers the nugget
    {
        setVisible(true);
        makeVisible();
        return;
    }
    if (m_pickupableByFrackMan == true && whereAmI()->closeToFrackMan(this, 3.00))   // nugget picked up by FrackMan
    {
        makeDead();
        whereAmI()->playSound(SOUND_GOT_GOODIE);
        whereAmI()->increaseScore(10);
        whereAmI()->pickedUpByFrackMan(this, 'n');
    }
    // NUGGET MUST CHECK TO SEE IF IT IS PICKUPABLE BY PROTESTORS AND STUFF
    
    if (!m_permanentState)   // nugget is temporary (since it was dropped by FrackMan)
    {
        if (howManyTicksLeft() == 0)
        {
            makeDead();
            return;
        }
        decreaseNumTicks();
    }
}

// SONAR KIT IMPLEMENTATION ======================================================================================

SonarKit::SonarKit(int numTicks, StudentWorld* world)
: Goodies(IID_SONAR, 0, 60, right, 1.0, 2, true, numTicks, world)  // GARBAGE TICK NUM
{

}

SonarKit::~SonarKit()
{
    
}

void SonarKit::doSomething()
{
    if (!isStillAlive())    // sonar is dead, so return right away
        return;
    if (whereAmI()->closeToFrackMan(this, 3.00))
    {
        makeDead();
        whereAmI()->playSound(SOUND_GOT_GOODIE);
        whereAmI()->increaseScore(75);
        whereAmI()->pickedUpByFrackMan(this, 's');
    }
    if (howManyTicksLeft() == 0)
    {
        makeDead();
        return;
    }
    decreaseNumTicks();   // since always temporary, always decrease the number of ticks left
}

// WATER POOL IMPLEMENTATION =====================================================================================

WaterPool::WaterPool(int startX, int startY, int numTicks, StudentWorld* world)
: Goodies(IID_WATER_POOL, startX, startY, right, 1.0, 2, true, numTicks, world)  // GARBAGE TICK NUM
{

}

WaterPool::~WaterPool()
{
    
}

void WaterPool::doSomething()
{
    if (!isStillAlive())    // dead, so return
        return;
    if (whereAmI()->closeToFrackMan(this, 3.00))
    {
        makeDead();
        whereAmI()->playSound(SOUND_GOT_GOODIE);
        whereAmI()->increaseScore(100);
        whereAmI()->pickedUpByFrackMan(this, 'w');
    }
    if (howManyTicksLeft() == 0)
    {
        makeDead();
        return;
    }
    decreaseNumTicks();   // since always temporary, always decrease the number of ticks left
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
//                if (getDirection() != left)     // only turn FrackMan in the correct direction, DON'T MOVE
//                {
//                    setDirection(left);
//                    return;
//                }
//                if (getX()-1 < 0 || getX()-1 > 60)    // gone out of bounds
//                    moveTo(getX(), getY());   // stay in the same position but maintain animations
//                else if(whereAmI()->isThereObstacle(getX()-1, getY()))  // can't dig into boulder
//                        break;
//                else
//                    moveTo(getX()-1, getY());     // since it is a valid position, move to that position
//                if (whereAmI()->deleteDirt(this))        // if you dig into dirt, play the digging sound
//                    whereAmI()->playSound(SOUND_DIG);
//                break;
                attemptToMove(left, -1, 0, -1, 0);
                break;
            case KEY_PRESS_RIGHT:
//                if (getDirection() != right)
//                {
//                    setDirection(right);
//                    return;
//                }
//                if (getX()+1 < 0 || getX()+1 > 60)
//                    moveTo(getX(), getY());
//                else if(whereAmI()->isThereObstacle(getX()+4, getY()))  // must do +4 because getX() is the lower left corner
//                    break;
//                else
//                    moveTo(getX()+1, getY());
//                if (whereAmI()->deleteDirt(this))
//                    whereAmI()->playSound(SOUND_DIG);
//                break;
                attemptToMove(right, 1, 0, 4, 0);
                break;
            case KEY_PRESS_DOWN:
//                if (getDirection() != down)
//                {
//                    setDirection(down);
//                    return;
//                }
//                if (getY()-1 < 0 || getY()-1 > 60)
//                    moveTo(getX(), getY());
//                else if(whereAmI()->isThereObstacle(getX(), getY()-1))
//                    break;
//                else
//                    moveTo(getX(), getY()-1);
//                if (whereAmI()->deleteDirt(this))
//                    whereAmI()->playSound(SOUND_DIG);
//                break;
                attemptToMove(down, 0, -1, 0, -1);
                break;
            case KEY_PRESS_UP:
//                if (getDirection() != up)     
//                {
//                    setDirection(up);
//                    return;
//                }
//                if (getY()+1 < 0 || getY()+1 > 60)
//                    moveTo(getX(), getY());
//                else if(whereAmI()->isThereObstacle(getX(), getY()+4))
//                    break;
//                else
//                    moveTo(getX(), getY()+1);
//                if (whereAmI()->deleteDirt(this))
//                    whereAmI()->playSound(SOUND_DIG);
//                break;
                attemptToMove(up, 0, 1, 0, 4);
                break;
            case KEY_PRESS_SPACE:     // SQUIRT TIME
                if (m_numSquirts > 0)    // still has squirts left
                {
                    whereAmI()->playSound(SOUND_PLAYER_SQUIRT);
                    m_numSquirts--;
                    switch (getDirection())
                    {
                        case left:
                        {
                            Squirt* leftSquirt = new Squirt(getX()-4, getY(), left, whereAmI());
                            whereAmI()->addActor(leftSquirt);
                            break;
                        }
                        case right:
                        {
                            Squirt* rightSquirt = new Squirt(getX()+4, getY(), right, whereAmI());
                            whereAmI()->addActor(rightSquirt);
                            break;
                        }
                        case down:
                        {
                            Squirt* downSquirt = new Squirt(getX(), getY()-4, down, whereAmI());
                            whereAmI()->addActor(downSquirt);
                            break;
                        }
                        case up:
                        {
                            Squirt* upSquirt = new Squirt(getX(), getY()+4, up, whereAmI());
                            whereAmI()->addActor(upSquirt);
                            break;
                        }
                        case none:
                            break;
                    }
                }
                break;
                
            case KEY_PRESS_ESCAPE:   // END THE CURRENT LEVEL
                makeDead();
                break;
                
            case KEY_PRESS_TAB:       // FrackMan drops a nugget (and his new mixtape, which is fire)
                if (m_numNuggets > 0)
                {
                    new Nugget(getX(), getY(), true, false, false, whereAmI());
                    m_numNuggets--;
                }
                break;
            case 'z':
            case 'Z':
                // implement the sonar functions
                break;
                
        }
    }
}

void FrackMan::addToInventory(Goodies *goodie, char label)
{
    switch (label)
    {
            case 'n':     // nugget
            m_numNuggets++;
            break;
            case 's':     // sonar kit
            m_numSonars++;
            break;
            case 'w':     // water
            m_numSquirts+=5;
            break;
    }
}

void FrackMan::attemptToMove(Direction direction, int addToX, int addToY, int obstacleX, int obstacleY)
{
    if (getDirection() != direction)    // only turn FrackMan in the current direction, DON'T MOVE
    {
        setDirection(direction);
        return;
    }
    
    if (getX()+addToX < 0 || getX()+addToX > 60 || getY()+addToY < 0 || getY()+addToY > 60)    // gone out of bounds
        moveTo(getX(), getY());   // stay in the same position but maintain animations
    
    else if(whereAmI()->isThereObstacle(getX()+obstacleX, getY()+obstacleY))  // can't dig into boulder
        return;
    
    else     // no boulders and not out of bounds, so move
        moveTo(getX()+addToX, getY()+addToY);
       
    if (whereAmI()->deleteDirt(this))        // if you dig into dirt, play the digging sound
        whereAmI()->playSound(SOUND_DIG);
}