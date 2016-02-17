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
    
    // BEGIN TESTING CODE
    Boulder* b1 = new Boulder(21, 24, this);
    
    Boulder* b2 = new Boulder(21, 10, this);
    
    cout << getRadiusBetween(b1, b2) << endl;  // should print 14
    
    Boulder* b3 = new Boulder(6, 8, this);
    
    Boulder* b4 = new Boulder(0, 0, this);
    
    cout << getRadiusBetween(b3, b4) << endl;   // should print 10
    // END TESTING CODE
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_FrackMan->doSomething();
    
    if (m_FrackMan->isStillAlive() == false)  // FRACK MAN IS DEAD NOOOOOOO
        return GWSTATUS_PLAYER_DIED;
    
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

double StudentWorld::getRadiusBetween(Actor* first, Actor* second) const
{
    double xDistance = first->getX() - second->getX();
    double yDistance = first->getY() - second->getY();
    return sqrt(pow(xDistance,2) + pow(yDistance,2));
}