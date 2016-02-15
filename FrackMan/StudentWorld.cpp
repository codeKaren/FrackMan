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
        for (int j = 0; j < 64; j++)     // keep the top four rows clear of dirt
        {
            if (i >= 30 && i <= 34 && j >= 4 && j <= 59)   // keep empty for mine shaft
                m_dirt[i][j] = nullptr;
            else if (j > 59)                          // keep the top of the screen empty
                m_dirt[i][j] = nullptr;
            else
                new Dirt(i, j, this);  // fill in the dirt
        }
    }
    // AND HIS NAME IS...FRAAAAAAAAACKMAN (DUM DE DUM DUM DUM DUMDUMDUM)
    m_FrackMan = new FrackMan(this);
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    m_FrackMan->doSomething();
    for (int i = m_FrackMan->getX(); i < m_FrackMan->getX() + 4; i++) // if there are dirt objects in FrackMan's new position
    {
        for (int j = m_FrackMan->getY(); j < m_FrackMan->getY() + 4; j++)
        {
            if (isThereDirt(i, j))
            {
                delete m_dirt[i][j];
                m_dirt[i][j] = nullptr;
            }
        }
    }


    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
}

StudentWorld::~StudentWorld()
{
    // FREE HIM (all dynamically allocated memory)
}

bool StudentWorld::isThereDirt(int x, int y)   // GET DIRTY
{
    if (m_dirt[x][y] == nullptr)
        return false;
    return true;
}