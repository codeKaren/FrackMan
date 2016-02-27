#include "Actor.h"
#include "StudentWorld.h"
#include <iostream> // TESTING ONLY
using namespace std;

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

bool Actor::canMove(Direction direction) const
{
    if (whereAmI()->canMoveXY(getX(), getY(), direction))
        return true;
    return false;
}

bool Actor::tryToMove(Direction direction)
{
    int addToX = 0;     // modify x or y for each possible direction to move in
    int addToY = 0;
    switch (direction)
    {
        case left:
            addToX = -1; break;
        case right:
            addToX = 1; break;
        case down:
            addToY = -1; break;
        case up:
            addToY = 1; break;
        case none:
            break;
    }
    if (canMove(direction))
    {
        moveTo(getX()+addToX, getY()+addToY);    // the space in front is open, so move!
        return true;
    }
    else
        return false;
}

bool Actor::canGetAnnoyed() const
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
            return;
        }
    }
    
    if (m_stableState == false)
    {
        if (numTicksWaiting >= 30)
        {
            if (numTicksWaiting == 30)
                whereAmI()->playSound(SOUND_FALLING_ROCK);       // only plays the sound one time
            
            if (!tryToMove(down))  // move downward (and kill the boulder if it can't move down)
                makeDead();
            
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
{ }

void Squirt::doSomething()
{
    if (!isStillAlive())
        return;
    
    // IMPLEMENT LATER: CAUSE 2 POINTS OF ANNOYANCE TO PROTESTORS
    whereAmI()->waterGun(this);
    
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
                if (!tryToMove(left)) // try to to move in that direction
                    makeDead();
                decreaseDistance();   // decrease the distance that the squirt can continue to travel for
                break;
            case right:
                if (!tryToMove(right))
                    makeDead();
                decreaseDistance();
                break;
            case down:
                if (!tryToMove(down))
                    makeDead();
                decreaseDistance();
                break;
            case up:
                if (!tryToMove(up))
                    makeDead();
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
{ }

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
    
    if (m_pickupableByFrackMan == false && whereAmI()->pickedUpByProtester(this))  // protester picks up nugget dropped by FrackMan
    {
        whereAmI()->playSound(SOUND_PROTESTER_FOUND_GOLD);
        return;
    }
    
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
: Goodies(IID_SONAR, 0, 60, right, 1.0, 2, true, numTicks, world)
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
: Goodies(IID_WATER_POOL, startX, startY, right, 1.0, 2, true, numTicks, world)
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

void Person::decreaseHealthPoints(int howMany)
{
    m_healthPoints -= howMany;
    // check if it's dead???
}

int Person::getHealthPoints() const
{
    return m_healthPoints;
}

bool Person::canGetAnnoyed() const
{
    return true;
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

int FrackMan::getNumSquirts() const
{
    return m_numSquirts;
}

int FrackMan::getNumSonars() const
{
    return m_numSonars;
}

int FrackMan::getNumNuggets() const
{
    return m_numNuggets;
}


// PROTESTOR IMPLEMENTATION ===========================================================================================

Protester::Protester(StudentWorld* world, int imageID, int HP)
: Person(imageID, 60, 60, left, 1.0, 0, HP, world)
{
    m_numSquaresToMoveInCurrentDirection = howManySquaresInCurrentDir();
    m_numTicksTotal = whereAmI()->max(0, 3-(whereAmI()->getLevel())/4);
    m_numTicksLeft = m_numTicksTotal;
    m_leaveOilFieldState = false;
    m_numTicksSinceShout = 0;
    m_numTicksSinceTurned = 0;
    whereAmI()->addActor(this);
}

Protester::~Protester()
{
    
}

bool Protester::doDifferentiatedStuff()
{
    return false;
}

void Protester::doSomething()
{
    if (!isStillAlive())
        return;
    
    if (getNumTicksLeft() > 0)
    {
        setNumTicksLeft(getNumTicksLeft()-1);
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
            // have a 64 by 64 2D array that holds the direction that the protester should move in to get to the exit
            Direction first;
            Direction second;
            Direction third;
            Direction fourth;
            Direction whichWay[4] = {first, second, third, fourth};
            whereAmI()->whichDirectionToGoIn(this, whichWay[0], whichWay[1], whichWay[2], whichWay[3]);
            for (int i = 0; i < 4; i++)  // try to move in one of the directions
            {
                if (canMove(whichWay[i]))   // can move
                {
                    setDirection(whichWay[i]);
                    tryToMove(whichWay[i]);
                    break;
                }
            }
            //            if (canMove(whichWay[0]))
            //            {
            //                setDirection(whichWay[0]);
            //                tryToMove(whichWay[0]);
            //            }
            //            else if (canMove(whichWay[1]))
            //            {
            //                setDirection(whichWay[1]);
            //                tryToMove(whichWay[1]);
            //            }
            //            else if (canMove(whichWay[2]))
            //            {
            //                setDirection(whichWay[2]);
            //                tryToMove(whichWay[2]);
            //            }
            //            else
            //            {
            //                setDirection(whichWay[3]);
            //                tryToMove(whichWay[3]);
            //            }
        }
    }
    
    else if (whereAmI()->closeToFrackMan(this, 4.00) && whereAmI()->isFacingFrackMan(this) && getNumTicksSinceShout() <= 0)
    {
        // close enough, is facing FrackMan, and hasn't shouted for 15 non-resting ticks
        whereAmI()->playSound(SOUND_PROTESTER_YELL);
        whereAmI()->annoyFrackMan(2);
        setNumTicksShout(15);  // reset the number of ticks for the Protester to wait
        setNumTicksLeft(30);   // prevent protester from moving for a while after shouting
        setNumTicksSinceTurned(getNumTicksSinceTurned()-1);
        return;
    }
    
    else if(doDifferentiatedStuff())
    {
        setNumTicksLeft(getNumTicksTotal());  // protester already had a non-resting turn, so reset the numTicksLeft
        setNumTicksShout(getNumTicksSinceShout()-1);  // decrease the number of ticks that the protester must wait to shout
        return;
    }
    
    else if (whereAmI()->isInLineOfSight(this) && !whereAmI()->closeToFrackMan(this, 4.00) && whereAmI()->canMoveToFrackMan(this))
    {
        // canMoveToFrackMan() changes the protester direction to face FrackMan if it returns true
        tryToMove(getDirection());  // move forward one square
        setNumSquaresToMoveInCurrDir(0);
        setNumTicksLeft(getNumTicksTotal());  // protester already had a non-resting turn, so reset the numTicksLeft
        setNumTicksShout(getNumTicksSinceShout()-1);  // decrease the number of ticks that the protester must wait to shout
        setNumTicksSinceTurned(getNumTicksSinceTurned()-1);
        return;
    }
    
    else   // can't directly see FrackMan
    {
        if (getNumSquaresInCurrDir() <= 0)
        {
            // pick a random new direction (that is not blocked)
            Direction dir = whereAmI()->generateRandomDirection();
            while (!canMove(dir))  // can't move in the randomly generated direction
            {
                dir = whereAmI()->generateRandomDirection();  // keep trying to find a direction that protester can move in
            }
            setDirection(dir);
            setNumSquaresToMoveInCurrDir(whereAmI()->generateRandomNumber(8, 60));
            setNumTicksSinceTurned(getNumTicksSinceTurned()-1);
        }
        else
        {
            // can move in perpendicular direction and hasn't that way for the past 200 non-resting ticks
            if (getNumTicksSinceTurned() <= 0)
            {
                bool turned = false;  // remember if the protester turned or not
                
                if (getDirection() == right || getDirection() == left) // check if can move up or down
                {
                    if (canMove(up) && canMove(down))   // can move both up and down
                    {
                        int i = whereAmI()->generateRandomNumber(0, 1);
                        if (i == 0)
                            setDirection(up);
                        else
                            setDirection(down);
                        turned = true;
                    }
                    if (canMove(up))              // can only move up
                    {
                        setDirection(up);
                        turned = true;
                    }
                    if (canMove(down))            // can only move down
                    {
                        setDirection(down);
                        turned = true;
                    }
                }
                else   // direction is either up or down, so check if it can move left or right
                {
                    if (canMove(left) && canMove(left))   // can move both left and right
                    {
                        int i = whereAmI()->generateRandomNumber(0, 1);
                        if (i == 0)
                            setDirection(left);
                        else
                            setDirection(right);
                        turned = true;
                    }
                    if (canMove(left))              // can only move left
                    {
                        setDirection(left);
                        turned = true;
                    }
                    
                    if (canMove(right))            // can only move right
                    {
                        setDirection(right);
                        turned = true;
                    }
                }
                if (turned == true)
                {
                    setNumSquaresToMoveInCurrDir(whereAmI()->generateRandomNumber(8, 60));
                    setNumTicksSinceTurned(200);
                }
            }
        }
        
        if (!tryToMove(getDirection()))  // if the protester can move, it will just move and not evaluate the rest
        {
            setNumSquaresToMoveInCurrDir(0);  // if the protester is blocked from moving in its current direction
        }
        else
            setNumSquaresToMoveInCurrDir(getNumSquaresInCurrDir()-1); // decrement the number of squares to move in the current direction
    }
    setNumTicksLeft(getNumTicksTotal());  // protester already had a non-resting turn, so reset the numTicksLeft
    setNumTicksShout(getNumTicksSinceShout()-1);  // decrease the number of ticks that the protester must wait to shout
}

int Protester::howManySquaresInCurrentDir()
{
    return whereAmI()->generateRandomNumber(8, 60);
}

int Protester::getNumTicksTotal() const
{
    return m_numTicksTotal;
}

int Protester::getNumTicksLeft() const
{
    return m_numTicksLeft;
}

bool Protester::isLeaveOilFieldState() const
{
    return m_leaveOilFieldState;
}

void Protester::setLeaveOilField()
{
    m_leaveOilFieldState = true;
}

int Protester::getNumTicksSinceShout() const
{
    return m_numTicksSinceShout;
}

void Protester::setNumTicksShout(int numTicksShout)
{
    m_numTicksSinceShout = numTicksShout;
}

int Protester::getNumSquaresInCurrDir() const
{
    return m_numSquaresToMoveInCurrentDirection;
}

void Protester::setNumSquaresToMoveInCurrDir(int squares)
{
    m_numSquaresToMoveInCurrentDirection = squares;
}

void Protester::setNumTicksLeft(int numTicksLeft)
{
    m_numTicksLeft = numTicksLeft;
}

int Protester::getNumTicksSinceTurned() const
{
    return m_numTicksSinceTurned;
}

void Protester::setNumTicksSinceTurned(int numTicksSinceTurned)
{
    m_numTicksSinceTurned = numTicksSinceTurned;
}

bool Protester::isHardcore() const
{
    return false;
}

// HARDCOREPROTESTOR IMPLEMENTATION ==================================================================================

HardcoreProtester::HardcoreProtester(StudentWorld* world)
: Protester(world, IID_HARD_CORE_PROTESTER, 20)
{
    
}

HardcoreProtester::~HardcoreProtester()
{
}

bool HardcoreProtester::doDifferentiatedStuff()
{
    if (!whereAmI()->closeToFrackMan(this, 4.00))  // hardcore protester is more than 4.00 units away from FrackMan
    {
        int M = 16 + whereAmI()->getLevel()*2;
        /*
         If the Hardcore Protester is less than or equal to a total of M legal
         horizontal or vertical moves away from the current location of the
         FrackMan (as can be determined using the same basic queue-based mazesearching
         algorithm described in Hardcore Protester item 3.b above), then
         the Hardcore Protester will sense the FrackMan’s cell phone signal and
         know where he is. This is true even if the Hardcore Protester has no direct
         line of sight to the FrackMan, so long as he can be reached in M moves
         without digging through Dirt or going through a Boulder. In such a
         situation, the Hardcore Protester will:
         i. Determine which horizontal/vertical direction to move in (as
         dictated by its maze-searching algorithm) such that if it were to
         make such a move it would be one square closer to the FrackMan.
         ii. Change its current direction to face this new direction.
         iii. Move one square in this direction such that after the move it is one
         square closer to the FrackMan’s current location.
         iv. Return immediately.
         */
        if (whereAmI()->getSquaresFromFrackMan(this) < M)
        {
            Direction first;
            Direction second;
            Direction third;
            Direction fourth;
            Direction whichWay[4] = {first, second, third, fourth};
            whereAmI()->whichDirToFrackMan(this, whichWay[0], whichWay[1], whichWay[2], whichWay[3]);
            for (int i = 0; i < 4; i++)  // try to move in one of the directions
            {
                if (canMove(whichWay[i]))   // can move
                {
                    setDirection(whichWay[i]);
                    tryToMove(whichWay[i]);
                    return true;
                }
            }
            return false;
        }
    }
    return false;
}


bool HardcoreProtester::isHardcore() const
{
    return true; 
}