#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <iostream> // JUST FOR TESTING
#include <math.h>
#include <queue>
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
    
    // generate boulders, nuggets, and oil barrels so that none are closer than 6 squares to each other
    
    int B = min(getLevel()/2 + 2, 6);  // number of boulders to be generated for each level
    int G = max(5-getLevel()/2, 2);   // number of nuggets
    int L = min(2+getLevel(), 20);  // number of barrels of oil
    
    for (int i = 0; i < B; i++)   // add all of the boulders into the oil field (must be between (0,20) and (60, 56)
    {
        if (m_allActors.size() == 0)
        {
            int x = generateRandomNumber(0, 60);
            int y = generateRandomNumber(20, 56);
            
            // making the first object, so don't need to check if it's close, but need to check if being generated into the mineshaft
            
            int numSquaresNoDirt = 0;            // check so that the boulder doesn't get generated in the mineshaft
            bool inMineShaft = true;
            while (inMineShaft)
            {
                for (int i = x; i < x + 4; i++)
                {
                    for (int j = y; j < y + 4; j++)
                    {
                        if (!isThereDirt(i, j))
                            numSquaresNoDirt++;
                    }
                }
                if (numSquaresNoDirt == 0)
                    inMineShaft = false;
                else
                {
                    x = generateRandomNumber(0, 60);
                    y = generateRandomNumber(20, 56);
                }
            }
            new Boulder(x, y, this);  // make the first boulder
        }
        else
        {
            int newX = 0;  // temp values before they get changed
            int newY = 0;
            getNewPosition(0, 20, 60, 56, newX, newY);
            new Boulder(newX, newY, this);   // boulder isn't too close to anything else, so place it down
        }
    }
    
    for (int k = 0; k < G; k++)  // add all of the gold nuggets to the field (must be between (0,0) and (60, 56))
    {
        int newX = 0;
        int newY = 0;
        getNewPosition(0, 0, 60, 56, newX, newY);
        new Nugget(newX, newY, false, true, true, this); // nugget can be placed down
    }
    
    for (int m = 0; m < L; m++)  // add all of the barrels of oil to the field (must be between (0,0) and (60, 56))
    {
        int newX = 0;
        int newY = 0;
        getNewPosition(0, 0, 60, 56, newX, newY);
        new OilBarrel(newX, newY, this);
    }
    
    m_numBarrels = L;   // all of the barrels have been made
    
    m_numTicksSinceAddedProtester = max(25, 200-getLevel());  // add a protester in the very first tick
    
    m_numProtesters = 0;   // no protesters in the field yet
    
    // BEGIN TESTING CODE
    /*
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
    */
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
    
    // possible add new actors during the tick
    
    // ADDING PROTESTERS
    int T = max(25, 200-getLevel());   // how many ticks to wait until allowed to add another protester
    int P = min(15, 2 + getLevel()*1.5);  // target number of protesters in the field
    int probabilityOfHardcore = min(90, getLevel()*10 + 30);  // probability of generating a hardcore protester

    if (m_numProtesters < P && m_numTicksSinceAddedProtester >= T)
    {
        int random = generateRandomNumber(1, probabilityOfHardcore);
        if (random == 1)   // generate a hardcore protester since it fulfilled the probability
        {
            new HardcoreProtester(this);
        }
        else   // didn't fulfill the probability so just make a regular protester
        {
            new Protester(this);
        }
        // since made new protester, reset the number of ticks to wait (to -1 because gonna increment right after)
        m_numTicksSinceAddedProtester = -1;
        m_numProtesters++;
    }
    m_numTicksSinceAddedProtester++;
    
    // ADDING WATERPOOL OR SONARKIT
    int G = getLevel()*25 + 300;  // 1 in G chance of adding new WaterPool or SonarKit during a tick
    int gimmeGoods = generateRandomNumber(1, G);  // check if a WaterPool or SonarKit should be generated
    if (gimmeGoods == 1)  // new goodie should be added
    {
        // 1 in 5 chance of adding SonarKit, 4 in 5 chance of adding WaterPool
        int gimmeSonar = generateRandomNumber(1,5);
        if (gimmeSonar == 1)   // get a SonarKit
        {
            new SonarKit((max(100, 300-10*getLevel())), this);
        }
        else   // get a WaterPool
        {
            int x = 0;  // temp values
            int y = 0;
            do {
                x = generateRandomNumber(0, 60);
                y = generateRandomNumber(0, 60);
            } while (!isValidWaterPoolSpot(x, y));
            new WaterPool(x, y, (max(100, 300-10*getLevel())), this);  // found valid spot so generate a WaterPool
        }
    }
    
    upDateMaze();
    
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
    return Pythagoras(first->getX(), first->getY(), second->getX(), second->getY());
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

double StudentWorld::Pythagoras(int x1, int y1, int x2, int y2) const 
{
    double xDistance = x1 - x2;
    double yDistance = y1 - y2;
    return sqrt(pow(xDistance,2) + pow(yDistance,2));
}

void StudentWorld::getNewPosition(int xBound1, int yBound1, int xBound2, int yBound2, int& newX, int& newY)
{
    int x = generateRandomNumber(xBound1, xBound2);
    int y = generateRandomNumber(yBound1, yBound2);
    
    for (int j = 0; j < m_allActors.size(); j++)
    {
        int numSquaresNoDirt = 0;            // check so that the boulder doesn't get generated in the mineshaft
        for (int i = x; i < x + 4; i++)
        {
            for (int j = y; j < y + 4; j++)
            {
                if (!isThereDirt(i, j))
                    numSquaresNoDirt++;
            }
        }
        
        if (Pythagoras(x, y, m_allActors[j]->getX(), m_allActors[j]->getY()) <= 6.00 || numSquaresNoDirt > 0)  // too close or in mineshaft
        {
            // pick a new random set of coordinates for the new Boulder
            x = generateRandomNumber(xBound1, xBound2);
            y = generateRandomNumber(yBound1, yBound2);
            j = 0;
        }
    }
    // position is valid so save it into (newX, newY)
    newX = x;
    newY = y;
}

bool StudentWorld::isValidWaterPoolSpot(int x, int y) const
{
    for (int i = x; i < x + 4; i++)
    {
        for (int j = y; j < y + 4; j++)
        {
            if (isThereDirt(i, j))
                return false;
        }
    }
    return true;   // no dirt so can spawn WaterPool there
}

Actor::Direction StudentWorld::whichDirectionToGoIn(Protester* protester) // returns the direction that the protester should go in to reach the exit
{
    int x = protester->getX();
    int y = protester->getY();
    return m_maze[x][y];
}

void StudentWorld::updateMaze()  // updates the array for the protesters to use to find what direction they should move in
{
    queue<Coord> coordQueue;    // create an empty queue
    char maze[64][64];   // create an array of chars to hold a model of how the algorithm has travelled through the maze
    for (int x = 0; x < 60; x++)   // actors can only travel between x = 0 and x = 60 and y = 0 and y = 60 throughout the game
    {
        for (int y = 0; y < 60; y++)
        {
            if (isThereDirt(x, y) || mazeHasBoulder(x, y))   // protester cannot move that way since blocked
                maze[x][y] = '#';
            else
                maze[x][y] = '.';
        }
    }
    
    Coord start(60, 60);
    
    coordQueue.push(start);
    maze[60][60] = '#';
    m_maze[60][60] = Actor::none;   // already reached the end so don't need to add a direction
    
    while (!coordQueue.empty())
    {
        Coord current = coordQueue.front();    // get the value of the top item before popping
        coordQueue.pop();
        
        if (maze[current.r()-1][current.c()] == '.')   // can move LEFT
        {
            coordQueue.push(Coord(current.r()-1, current.c()));
            maze[current.r()-1][current.c()] = '#';
            m_maze[current.r()-1][current.c()] = Actor::right;
        }
        
        if (maze[current.r()][current.c()+1] == '.')   // can move NORTH
        {
            coordQueue.push(Coord(current.r(), current.c()+1));
            maze[current.r()][current.c()+1] = '#';
            m_maze[current.r()][current.c()+1] = Actor::down;
        }
        
        if (maze[current.r()+1][current.c()] == '.')   // can move RIGHT
        {
            coordQueue.push(Coord(current.r()+1, current.c()));
            maze[current.r()+1][current.c()] = '#';
            m_maze[current.r()+1][current.c()] = Actor::left;
        }
        
        if (maze[current.r()][current.c()-1] == '.')   // can move SOUTH
        {
            coordQueue.push(Coord(current.r(), current.c()-1));
            maze[current.r()][current.c()-1] = '#';
            m_maze[current.r()][current.c()-1] = Actor::up;
        }
    }
}

bool StudentWorld::mazeHasBoulder(int x, int y) const  // returns true if boulder in a 3.00 radius to (x,y)
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->isObstacle() == true)   // loop through all actors to find an obstacle (boulder)
        {
            if (Pythagoras(x, y, m_allActors[i]->getX(), m_allActors[i]->getY()))  // there is a boulder nearby
                return true;
        }
    }
    return false;
}

