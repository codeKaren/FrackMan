#include "StudentWorld.h"
#include "Actor.h"
#include <string>
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
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_FrackMan->doSomething();

    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            delete m_dirt[i][j];            // delete all of the dirt objects
        }
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
    
    delete m_FrackMan;    // bye bye FrackMan
}

bool StudentWorld::isThereDirt(int x, int y)   // GET DIRTY
{
    if (m_dirt[x][y] == nullptr)
        return false;
    return true;
}

void StudentWorld::deleteDirt()    // delete dirt based on FrackMan's current position
{
    for (int i = m_FrackMan->getX(); i < m_FrackMan->getX() + 4; i++) // if there are dirt objects in FrackMan's new position
    {                                                                 // delete them
        for (int j = m_FrackMan->getY(); j < m_FrackMan->getY() + 4; j++)
        {
            if (isThereDirt(i, j))
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = nullptr;
                playSound(SOUND_DIG);
            }
        }
    }
}