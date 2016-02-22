#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <iostream> // JUST FOR TESTING
#include <math.h>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(string assetDir)
: GameWorld(assetDir)
{
    // CAN'T TOUCH THIS (MC HAMMER)
}

int StudentWorld::init()
{
    // ADDING THE DIRT
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            if (i >= 30 && i <= 34 && j >= 4 && j <= 59)   // keep empty for mine shaft
                m_dirt[i][j] = nullptr;
            else if (j > 59)                          // keep the top four rows clear of dirt
                m_dirt[i][j] = nullptr;
            else
                m_dirt[i][j] = new Dirt(i, j, this);
        }
    }
    
    // AND HIS NAME IS...FRAAAAAAAAACKMAN (DUM DE DUM DUM DUM DUMDUMDUM)
    m_FrackMan = new FrackMan(this);
    
    int B = min(getLevel()/2 + 2, 6);  // number of boulders to be generated for each level
    int G = max(5-getLevel()/2, 2);   // number of nuggets
    int L = min(2+getLevel(), 20);  // number of barrels of oil 
    
    // generate boulders, nuggets, and oil barrels so that none are closer than 6 squares to each other
    
    m_numBarrels = 2;   // to ensure that the game runs
    
    // BEGIN TESTING CODE
    
    new Boulder(21, 24, this);
    
    new Boulder(21, 10, this);
    
    new Boulder(6, 8, this);
    
    new Boulder(0, 0, this);
    
    m_numBarrels = 2;
    
    new OilBarrel(60, 0, this);
    
    new OilBarrel(58, 6, this);
    
    new Nugget(50, 8, false, true, true, this);
    
    new SonarKit((max(100, 300-10*getLevel())), this);
    
    new WaterPool(20, 60, (max(100, 300-10*getLevel())), this);
    
    new Protester(this);
    
    new HardcoreProtester(this);
    
    // END TESTING CODE
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // update the game status line
    updateDisplayText();
    
    if (m_FrackMan->isStillAlive() == false)  // FRACK MAN IS DEAD NOOOOOOO
        return GWSTATUS_PLAYER_DIED;
    
    if (m_numBarrels == 0)   // all of the barrels have been collected, so the level is done!
    {
        playSound(SOUND_FINISHED_LEVEL);
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    m_FrackMan->doSomething();   // tell FrackMan to do something
    
    for (int i = 0; i < m_allActors.size(); i++)  // tell all of the other actors to do something (except dirt)
        m_allActors[i]->doSomething();
    
    for (vector<Actor*>::iterator it = m_allActors.begin(); it != m_allActors.end(); )
    {
        if (!(*it)->isStillAlive())    // if the object is dead, destroy it
        {
            delete *it;
            it = m_allActors.erase(it);
        }
        else
            it++;
    }
    
    return GWSTATUS_CONTINUE_GAME;  // FrackMan isn't dead and hasn't gotten all of the barrels, so continue the game
}

void StudentWorld::cleanUp()
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            delete m_dirt[i][j];            // delete all of the dirt objects
            m_dirt[i][j] = nullptr;
        }
    }
    
    for (vector<Actor*>::iterator it = m_allActors.begin(); it != m_allActors.end(); )   // delete all game objects
    {
        delete *it;
        it = m_allActors.erase(it);
    }
    
    delete m_FrackMan;    // bye bye FrackMan
}

StudentWorld::~StudentWorld()    // FREE HIM (all dynamically allocated memory)
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            delete m_dirt[i][j];            // delete all of the dirt objects
        }
    }
    
    for (vector<Actor*>::iterator it = m_allActors.begin(); it != m_allActors.end(); )   // delete all game objects
    {
        delete *it;
        it = m_allActors.erase(it);
    }
    
    delete m_FrackMan;    // bye bye FrackMan
}

void StudentWorld::updateDisplayText()  // update the display text at the top of the screen
{
    string s = "00000000";   // default string that we can take 0s from using substr()
    string t = "     ";   // default string that we can take spaces from using substr()
    
    string score = to_string(getScore());
    string level = to_string(getLevel());
    string lives = to_string(getLives());
    string health = to_string(m_FrackMan->getHealthPoints() * 10);
    string squirts = to_string(m_FrackMan->getNumSquirts());
    string nuggets = to_string(m_FrackMan->getNumNuggets());
    string sonar = to_string(m_FrackMan->getNumSonars());
    string barrelsLeft = to_string(m_numBarrels);
    
    score = s.substr(0, 6-score.size()) + score;
    level = t.substr(0, 2-level.size()) + level;
    health = t.substr(0, 3-health.size()) + health + "%";
    squirts = t.substr(0, 2-squirts.size()) + squirts;
    nuggets = t.substr(0, 2-nuggets.size()) + nuggets;
    sonar = t.substr(0, 2-sonar.size()) + sonar;
    barrelsLeft = t.substr(0, 2-barrelsLeft.size()) + barrelsLeft;
    
    string final = "Scr: " + score + "  " + "Lvl: " + level + "  " + "Lives: " + lives + "  " + "Hlth: " + health + "  " + "Wtr: " + squirts + "  " + "Gld: " + nuggets + "  " + "Sonar: " + sonar + "  " + "Oil Left: " + barrelsLeft;
    
    setGameStatText(final);
}

bool StudentWorld::isThereDirt(int x, int y) const  // GET DIRTY
{
    if (m_dirt[x][y] == nullptr)
        return false;
    return true;
}

bool StudentWorld::isThereObstacle(int x, int y) const    // return true if there is an obstacle in that position
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->isObstacle() == true)   // loop through all actors to find an obstacle
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    if ((m_allActors[i]->getX() + j) == x && (m_allActors[i]->getY() + k) == y)  // found obstacle in x,y
                        return true;
                }
            }
        }
    }
    return false;
}

bool StudentWorld::radiusCloseToBoulder(int x, int y, Actor* actor) const  // FrackMan must be less than 3.00 from boulder
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->isObstacle() == true)   // loop through all actors to find an obstacle (boulder)
        {
            if (m_allActors[i] == actor)    // check to see if boulder is comparing the radius between itself
                continue;
            double xDistance = x - (m_allActors[i]->getX());
            double yDistance = y - (m_allActors[i]->getY());
            int radius = sqrt(pow(xDistance,2) + pow(yDistance,2));
            if (radius <= 3.00)   // distance from FrackMan to boulder is less than 3.00
                return true;
        }
    }
    return false;
}

void StudentWorld::boulderSmash(Boulder* smasher)
{
    if (closeToFrackMan(smasher, 3.00))   // boulder hits FrackMan
    {
        m_FrackMan->decreaseHealthPoints(100);    // decreases health points by 100
        m_FrackMan->makeDead();   // then FrackMan immediately loses a life
        decLives();
        playSound(SOUND_PLAYER_GIVE_UP);
    }
    
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->canGetAnnoyed() == true)  // loop through all actors to find protestors
        {
            if (getRadiusBetween(m_allActors[i], smasher) <= 3.00 && !dynamic_cast<Protester*>(m_allActors[i])->isLeaveOilFieldState())
            {
                dynamic_cast<Person*>(m_allActors[i])->decreaseHealthPoints(100);
                increaseScore(500);  // protester getting smashed by boulder increases player score by 500
                playSound(SOUND_PROTESTER_GIVE_UP);
                dynamic_cast<Protester*>(m_allActors[i])->setLeaveOilField();  // protester is put into leave-oil-field state
                dynamic_cast<Protester*>(m_allActors[i])->setNumTicksLeft(0);  // so protester moves the very next tick
            }
        }
    }
}

void StudentWorld::waterGun(Squirt* squirt)
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->canGetAnnoyed() == true)  // loop through all actors to find a protestors
        {
            if (getRadiusBetween(m_allActors[i], squirt) <= 3.00 && !dynamic_cast<Protester*>(m_allActors[i])->isLeaveOilFieldState())
            {
                dynamic_cast<Person*>(m_allActors[i])->decreaseHealthPoints(2);  // decrease HP of protester
                squirt->makeDead();
                if (dynamic_cast<Person*>(m_allActors[i])->getHealthPoints() > 0)  // not fully annoyed yet
                {
                    playSound(SOUND_PROTESTER_ANNOYED);
                    dynamic_cast<Protester*>(m_allActors[i])->setNumTicksLeft(max(50, 100-getLevel()*10));  // stun the protester
                }
                else  // must have been fully annoyed
                {
                    if (dynamic_cast<Protester*>(m_allActors[i])->isHardcore()) // hardcore protester got squirted to death
                        increaseScore(250);
                    else    // regular protester got squirted to death
                        increaseScore(100);
                    playSound(SOUND_PROTESTER_GIVE_UP);
                    dynamic_cast<Protester*>(m_allActors[i])->setLeaveOilField();  // protester is put into leave-oil-field state
                    dynamic_cast<Protester*>(m_allActors[i])->setNumTicksLeft(0);  // so protester moves the very next tick
                }
            }
        }
    }
}

bool StudentWorld::deleteDirt(Actor* actor)    // delete dirt based on object/s current position
{
    bool dirtDeleted = false;
    for (int i = actor->getX(); i < actor->getX() + 4; i++) // if there are dirt objects in the object's position
    {                                                                 // delete them
        for (int j = actor->getY(); j < actor->getY() + 4; j++)
        {
            if (isThereDirt(i, j))
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = nullptr;
                dirtDeleted = true;
            }
        }
    }
    return dirtDeleted;
}

void StudentWorld::addActor(Actor* actor)
{
    m_allActors.push_back(actor);
}

void StudentWorld::decreaseBarrelNum()
{
    m_numBarrels--;
}

void StudentWorld::pickedUpByFrackMan(Goodies* goodie, char label)
{
    m_FrackMan->addToInventory(goodie, label);
}

bool StudentWorld::pickedUpByProtester(Nugget* nugget)
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->canGetAnnoyed() == true)  // loop through all actors to find protestors
        {
            if (getRadiusBetween(nugget, m_allActors[i]) <= 3.00)  // protester is close enough to pick up the nugget
            {
                nugget->makeDead();   // nugget must make itself dead since it was picked up
                
                if (!dynamic_cast<Protester*>(m_allActors[i])->isHardcore()) // regular protester picked up the nugget
                {
                    dynamic_cast<Protester*>(m_allActors[i])->setLeaveOilField();  // protester is bribed, so leave the oil field
                    increaseScore(25);
                    return true;
                }
                else  // hardcore protester picked up the nugget
                {
                    increaseScore(50);
                    dynamic_cast<Protester*>(m_allActors[i])->setNumTicksLeft(max(50, 100-getLevel()*10)); // staring at the nugget
                    return true;
                }
            }
        }
    }
    return false;
}

bool StudentWorld::closeToFrackMan(Actor* actor, double howClose) const // returns true if FrackMan is close enough to make the goodie visible
{
    return (getRadiusBetween(actor, m_FrackMan) <= howClose);
}

bool StudentWorld::isFacingFrackMan(Protester* protester) const
{
    // check quadrant 1
    if (m_FrackMan->getX() >= protester->getX() && m_FrackMan->getY() >= protester->getY())
    {
        if (protester->getDirection() == Actor::right || protester->getDirection() == Actor::up)
            return true;
        return false;
    }
    // check quadrant 2
    if (m_FrackMan->getX() <= protester->getX() && m_FrackMan->getY() >= protester->getY())
    {
        if (protester->getDirection() == Actor::left || protester->getDirection() == Actor::up)
            return true;
        return false;
    }
    // check quadrant 3
    if (m_FrackMan->getX() <= protester->getX() && m_FrackMan->getY() <= protester->getY())
    {
        if (protester->getDirection() == Actor::left || protester->getDirection() == Actor::down)
            return true;
        return false;
    }
    // check quadrant 4
    if (m_FrackMan->getX() >= protester->getX() && m_FrackMan->getY() <= protester->getY())
    {
        if (protester->getDirection() == Actor::right || protester->getDirection() == Actor::down)
            return true;
        return false;
    }
    return false;  // this line should never be evaluated unless there is an error in my code
}

bool StudentWorld::isInLineOfSight(Protester* protester) const
{
    if (m_FrackMan->getX() == protester->getX() || m_FrackMan->getY() == protester->getY())
    {
        return true;
    }
    return false;
}

bool StudentWorld::canMoveToFrackMan(Protester* protester) // protester can continue moving in the current direction to reach FrackMan; makes the protester face FrackMan if it returns true
{
    // moving up or down
    if (protester->getX() == m_FrackMan->getX())
    {
        if (protester->getY() < m_FrackMan->getY())  // move up to get to FrackMan
        {
            for (int k = protester->getX(); k < protester->getX() + 4; k++)  // check the whole width of protester
            {
                for (int n = protester->getY(); n < m_FrackMan->getY(); n++)
                {
                    if (isThereDirt(k, n) || isThereObstacle(k, n))
                    {
                        return false;
                    }
                }
            }
            protester->setDirection(Actor::up);   // since the protester can move to Frackman, change its direction
            return true;
        }
        
        if (protester->getY() > m_FrackMan->getY())  // move down to get to FrackMan
        {
            for (int k = protester->getX(); k < protester->getX() + 4; k++)
            {
                for (int n = m_FrackMan->getY(); n < protester->getY(); n++)
                {
                    if (isThereDirt(k, n) || isThereObstacle(k, n))
                    {
                        return false;
                    }
                }
            }
            protester->setDirection(Actor::down);
            return true;
        }
    }
    
    // moving left or right
    if (protester->getY() == m_FrackMan->getY())
    {
        if (protester->getX() < m_FrackMan->getX())  // move right to get to FrackMan
        {
            for (int k = protester->getY(); k < protester->getY() + 4; k++)
            {
                for (int n = protester->getX(); n < m_FrackMan->getX(); n++)
                {
                    if (isThereDirt(n, k) || isThereObstacle(n, k))
                    {
                        return false;
                    }
                }
            }
            protester->setDirection(Actor::right);
            return true;
        }
        
        if (protester->getX() > m_FrackMan->getX())  // move left to get to FrackMan
        {
            for (int k = protester->getY(); k < protester->getY() + 4; k++)
            {
                for (int n = m_FrackMan->getX(); n < protester->getX(); n++)
                {
                    if (isThereDirt(n, k) || isThereObstacle(n, k))
                    {
                        return false;
                    }
                }
            }
            protester->setDirection(Actor::left);
            return true;
        }
    }
    
    return true;  // no dirt or obstacles in the path, so return true
}

void StudentWorld::sonarFunction()
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (!m_allActors[i]->isVisible() && closeToFrackMan(m_allActors[i], 12.00))
            {
                m_allActors[i]->makeVisible();
            }
    }
}

void StudentWorld::annoyFrackMan(int decreaseHP)
{
    m_FrackMan->getAnnoyed(decreaseHP);
}

double StudentWorld::getRadiusBetween(Actor* first, Actor* second) const
{
    double xDistance = first->getX() - second->getX();
    double yDistance = first->getY() - second->getY();
    return sqrt(pow(xDistance,2) + pow(yDistance,2));
}

int StudentWorld::max(int a, int b)
{
    if (a > b)
        return a;
    return b;
}

int StudentWorld::min(int a, int b)
{
    if (a < b)
        return a;
    return b;
}

int StudentWorld::generateRandomNumber(int startNum, int endNum)  
{
    return rand() % (endNum-startNum) + startNum;  // returns something within the range from startNum to endNum
}

Actor::Direction StudentWorld::generateRandomDirection()
{
    int dir = generateRandomNumber(0, 3);
    switch (dir)
    {
        case 0:
            return Actor::left;
            break;
        case 1:
            return Actor::right;
            break;
        case 2:
            return Actor::up;
            break;
        case 3:
            return Actor::down;
            break;
    }
    return Actor::none;   // this line of code should never get evaluated
}