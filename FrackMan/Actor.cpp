#include "Actor.h"
#include "StudentWorld.h"
#include <iostream> // TESTING ONLY

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

// ACTOR IMPLEMENTATION ===========================================================================================

Actor::Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, StudentWorld* world)
:GraphObject(imageID, startX, startY, startDirection, size, depth)
{
    m_stillAlive = true;
    m_visible = visible;
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

bool Actor::isVisible() const
{
    return m_visible;
}

void Actor::makeVisible()
{
    m_visible = true;
    setVisible(true);
}

StudentWorld* Actor::whereAmI() const
{
    return m_studentWorld;
}

bool Actor::isObstacle() const
{
    return false;
}

void Actor::tryToMove(Direction direction)
{
    int addToX = 0;     // modify x or y for each possible direction to move in
    int addToY = 0;
    int obstacleX = 0;
    int obstacleY = 0;
    switch (direction)
    {
        case left:
            addToX = -1; obstacleX = -1; break;
        case right:
            addToX = 1; obstacleX = 4; break;
        case down:
            addToY = -1; obstacleY = -1; break;
        case up:
            addToY = 1; obstacleY = 4; break;
        case none:
            break;
    }
    
    if (getX()+addToX < 0 || getX()+addToX > 60 || getY()+addToY < 0 || getY()+addToY > 60)  // gone out of bounds
    {
        makeDead();
        return;
    }
    else if (direction == left || direction == right)
    {
        for (int k = getY(); k < getY() + 4; k++)   // runs into obstacle or dirt
        {
            if (whereAmI()->isThereDirt(getX() + obstacleX, k) || whereAmI()->isThereObstacle(getX()+obstacleX, getY()+obstacleY))
            {
                makeDead();
                return;
            }
        }
    }
    else if (direction == down || direction == up)
    {
        for (int k = getX(); k < getX() + 4; k++)
        {
            if (whereAmI()->isThereDirt(k, getY()+ obstacleY) || whereAmI()->isThereObstacle(getX()+obstacleX, getY()+obstacleY))
            {
                makeDead();
                return;
            }
        }
    }
    moveTo(getX()+addToX, getY()+addToY);    // the space in front is open, so move!
}

bool Actor::canGetAnnoyed()
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

            tryToMove(down);  // move downward
            
            // if there is boulder directly below, kill the current boulder
            if (whereAmI()->isThereObstacle(getX(), getY()-1))
            {
                makeDead();
                return;
            }
            
            // boulder sees if it hits FrackMan or protesters
            whereAmI()->boulderSmash(this);
            
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
    
    // check to see if there is a boulder near the starting location of the squirt 
    if (whereAmI()->radiusCloseToBoulder(x, y, this))
    {
        makeDead();
        return;
    }
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
        switch (getDirection())    // for each direction
        {
            case left:
                tryToMove(left);      // try to to move in that direction
                decreaseDistance();   // decrease the distance that the squirt can continue to travel for
                break;
            case right:
                tryToMove(right);
                decreaseDistance();
                break;
            case down:
                tryToMove(down);
                decreaseDistance();
                break;
            case up:
                tryToMove(up);
                decreaseDistance();
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
    whereAmI()->addActor(this);
    m_numTicks = numTicks;
}

Goodies::~Goodies()
{
    
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

bool Person::canGetAnnoyed() const
{
    return true;
}

void Person::decreaseHealthPoints(int howMany)
{
    m_healthPoints -= howMany;
    // check if it's dead???
}

int Person::getHealthPoints() const
{
    return m_healthPoints;
}

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
    if (getHealthPoints() <= 0)
    {
        makeDead();
        whereAmI()->playSound(SOUND_PLAYER_GIVE_UP);
        whereAmI()->decLives();
    }
    int keyPressed;
    if (whereAmI()->getKey(keyPressed) == true)
    {
        switch (keyPressed)
        {
            case KEY_PRESS_LEFT:
                moveOrDig(left, -1, 0);
                break;
            case KEY_PRESS_RIGHT:
                moveOrDig(right, 1, 0);
                break;
            case KEY_PRESS_DOWN:
                moveOrDig(down, 0, -1);
                break;
            case KEY_PRESS_UP:
                moveOrDig(up, 0, 1);
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
                whereAmI()->decLives();
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
                if (m_numSonars > 0)
                {
                    whereAmI()->playSound(SOUND_SONAR);
                    m_numSonars--;
                    whereAmI()->sonarFunction();  // make all hidden objects within a radius of 12 become visible
                }
                break;
                
        }
    }
}

void FrackMan::getAnnoyed()
{
    
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

void FrackMan::moveOrDig(Direction direction, int addToX, int addToY)
{
    if (getDirection() != direction)    // only turn FrackMan in the current direction, DON'T MOVE
    {
        setDirection(direction);
        return;
    }
    
    if (getX()+addToX < 0 || getX()+addToX > 60 || getY()+addToY < 0 || getY()+addToY > 60)    // gone out of bounds
        moveTo(getX(), getY());   // stay in the same position but maintain animations
    
    else if(whereAmI()->radiusCloseToBoulder(getX()+addToX, getY()+addToY, this))  // can't dig into boulder
        return;
    
    else     // no boulders and not out of bounds, so move
        moveTo(getX()+addToX, getY()+addToY);
       
    if (whereAmI()->deleteDirt(this))        // if you dig into dirt, play the digging sound
        whereAmI()->playSound(SOUND_DIG);
}

// PROTESTOR IMPLEMENTATION ===========================================================================================

Protester::Protester(StudentWorld* world)
: Person(IID_PROTESTER, 60, 60, left, 1.0, 0, 5, world)
{
    m_numSquaresToMoveInCurrentDirection = howManySquaresInCurrentDir();
    m_numTicksTotal = whereAmI()->max(0, 3-(whereAmI()->getLevel())/4);
    m_numTicksLeft = m_numTicksTotal;
    m_leaveOilFieldState = false;
}

Protester::~Protester()
{
    
}

void Protester::doSomething()
{
    if (!isStillAlive())
        return;
    if (getNumTicksLeft() > 0)
    {
        timePasses();
        return;
    }
    
    // Protester is not resting during this tick so...
    if (isLeaveOilFieldState()) // protester is in a leave-the-oil-field state
    {
        if (getX() == 60 && getY() == 60)
            makeDead();
        else
        {
            // move one step closer to the exit (60, 60) by using a queue-based maze-searching algorithm
            // a single data structure that enables all protesters to determine their optimal path regardless of location???
        }
    }
    
}

void Protester::getAnnoyed()
{
    
}

int Protester::howManySquaresInCurrentDir()
{
    // generate a random number where 8 <= n <= 60
    return 10;   // garbage right now
}

int Protester::getNumTicksTotal() const
{
    return m_numTicksTotal;
}

int Protester::getNumTicksLeft() const
{
    return m_numTicksLeft;
}

void Protester::timePasses()
{
    m_numTicksLeft--;
}

bool Protester::isLeaveOilFieldState() const
{
    return m_leaveOilFieldState;
}

void Protester::setLeaveOilField()
{
    m_leaveOilFieldState = true;
}

// HARDCOREPROTESTOR IMPLEMENTATION ==================================================================================

HardcoreProtester::HardcoreProtester(StudentWorld* world)
: Protester(world)
{
    
}

HardcoreProtester::~HardcoreProtester()
{
    
}

void HardcoreProtester::doSomething()
{
    if (!isStillAlive())
        return;
}

void HardcoreProtester::getAnnoyed()
{
    
}

