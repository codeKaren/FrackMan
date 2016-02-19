#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <iostream> // JUST FOR TESTING
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
     
    // END TESTING CODE
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    if (m_FrackMan->isStillAlive() == false)  // FRACK MAN IS DEAD NOOOOOOO
        return GWSTATUS_PLAYER_DIED;
    
    if (m_numBarrels == 0)
        return GWSTATUS_FINISHED_LEVEL;
    
    m_FrackMan->doSomething();
    
    for (int i = 0; i < m_allActors.size(); i++)
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
    
    setGameStatText("FRACK YOU, FRACK MAN!");   // garbage right now
    
    return GWSTATUS_CONTINUE_GAME;
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

bool StudentWorld::diggingIntoBoulder(int x, int y, Actor* actor) const  // FrackMan must be less than 3.00 from center of boulder??
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->isObstacle() == true)   // loop through all actors to find an obstacle
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

bool StudentWorld::closeToFrackMan(Actor* goodie, double howClose) const // returns true if FrackMan is close enough to make the goodie visible
{
    return (getRadiusBetween(goodie, m_FrackMan) <= howClose);
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