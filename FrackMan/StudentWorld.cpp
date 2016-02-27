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
            if (i >= 30 && i < 34 && j >= 4 && j <= 59)   // keep empty for mine shaft
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
        int x = generateRandomNumber(0, 60);
        int y = generateRandomNumber(20, 56);
        
        while (x >= 27 && x <= 33)
        {
            x = generateRandomNumber(0, 60);
        }
        
        for (int j = 0; j < m_allActors.size(); j++)
        {
            if (Pythagoras(x, y, m_allActors[j]->getX(), m_allActors[j]->getY()) <= 6.00 || (x >= 27 && x <= 33))  // too close or in mineshaft
            {
                // pick a new random set of coordinates for the new Boulder
                x = generateRandomNumber(0, 60);
                y = generateRandomNumber(20, 56);
                j = 0;
            }
        }
        
        new Boulder(x, y, this);
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
    
    for (int i = 0; i < 64; i++)
    {
        for (int j = 0; j < 64; j++)
            m_maze[i][j] = 999;  // garbage values for nothing is there
    }
    
    // REMOVE
    new HardcoreProtester(this);
    
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
    
    updateMaze();  // change the maze with directions for the protester to leave the maze
    
    updateFrackManFinder();  // change the maze for directions for Hardcore Protester
    
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

bool StudentWorld::radiusCloseToBoulder(int x, int y, Actor* actor) const  // actor must be less than 3.00 from boulder
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

void StudentWorld::boulderSmash(Boulder* smasher)  // boulder hits a FrackMan or a Protester
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

void StudentWorld::annoyFrackMan(int decreaseHP)
{
    m_FrackMan->decreaseHealthPoints(decreaseHP);
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
        
        if ((Pythagoras(x, y, m_allActors[j]->getX(), m_allActors[j]->getY()) <= 6.00) || (numSquaresNoDirt > 0))  // too close or in mineshaft
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

void StudentWorld::whichDirectionToGoIn(Protester* protester, Actor::Direction& first, Actor::Direction& second, Actor::Direction& third, Actor::Direction& fourth) // returns the direction that the protester should go in to reach the exit
{
    int x = protester->getX();
    int y = protester->getY();
    
    // a[0] = left, a[1] = right, a[2] = up, a[3] = down
    int stepsNeeded[4] = {m_maze[x-1][y], m_maze[x+1][y], m_maze[x][y+1], m_maze[x][y-1]};
    int posMin = 0;
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            if (stepsNeeded[i] < stepsNeeded[posMin])
                posMin = i;
        }
        
        stepsNeeded[posMin] = 1000;   // so you ignore that value when comparing in the future iterations
        
        switch (j)   // set the values for the best direction to go in
        {
            case 0:
                switch (posMin)
            {
                case 0: first = Actor::left; break;
                case 1: first = Actor::right; break;
                case 2: first = Actor::up; break;
                case 3: first = Actor::down; break;
            }
                break;
            case 1:
                switch (posMin)
            {
                case 0: second = Actor::left; break;
                case 1: second = Actor::right; break;
                case 2: second = Actor::up; break;
                case 3: second = Actor::down; break;
            }
                break;
            case 2:
                switch (posMin)
            {
                case 0: third = Actor::left; break;
                case 1: third = Actor::right; break;
                case 2: third = Actor::up; break;
                case 3: third = Actor::down; break;
            }
                break;
            case 3:
                switch (posMin)
            {
                case 0: fourth = Actor::left; break;
                case 1: fourth = Actor::right; break;
                case 2: fourth = Actor::up; break;
                case 3: fourth = Actor::down; break;
            }
                break;
        }
    }
}

void StudentWorld::updateMaze()  // updates the array for the protesters to use to find what direction they should move in
{
    queue<Coord> coordQueue;    // create an empty queue
    
    // reinitialize the map
    for (int x = 0; x < 64; x++)   // set all spots in the array to 999 (999 means a spot hasn't been discovered yet)
    {
        for (int y = 0; y < 64; y++)
        {
            m_maze[x][y] = 999;
        }
    }
    
    Coord start(60, 60);
    
    coordQueue.push(start);
    
    m_maze[60][60] = 0; // already reached the end so the number of steps to move from there is 0
    
    while (!coordQueue.empty())
    {
        Coord current = coordQueue.front();    // get the value of the top item before popping
        coordQueue.pop();
        
        if (current.r() <= 60 && current.c()+1 <= 60 && current.r() >= 0 && current.c()+1 >= 0)   // can move NORTH
        {
            if (canMoveXY(current.r(), current.c(), Actor::up) && m_maze[current.r()][current.c()+1] == 999)
            {
                m_maze[current.r()][current.c()+1] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()+1));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::up) && m_maze[current.r()][current.c()] + 1 < m_maze[current.r()][current.c()+1])
            {
                m_maze[current.r()][current.c()+1] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()+1));
            }
        }
        
        if (current.r() <= 60 && current.c()-1 && current.r() >= 0 && current.c()-1 >= 0)   // can move SOUTH
        {
            if (canMoveXY(current.r(), current.c(), Actor::down) && (m_maze[current.r()][current.c()-1] == 999))
            {
                m_maze[current.r()][current.c()-1] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()-1));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::down) && m_maze[current.r()][current.c()] + 1 < m_maze[current.r()][current.c()-1])
            {
                m_maze[current.r()][current.c()-1] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()-1));
            }
        }
        
        if (current.r()+1 <= 60 && current.c() <= 60 && current.r()+1 >= 0 && current.c() >= 0)   // can move RIGHT
        {
            if (canMoveXY(current.r(), current.c(), Actor::right) && m_maze[current.r()+1][current.c()] == 999)
            {
                m_maze[current.r()+1][current.c()] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()+1, current.c()));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::right) && m_maze[current.r()][current.c()] + 1 < m_maze[current.r()+1][current.c()])
            {
                m_maze[current.r()][current.c()-1] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()+1, current.c()));
            }
        }
        
        if (current.r()-1 <= 60 && current.c() <= 60 && current.r()-1 >= 0 && current.c() >= 0)   // can move LEFT
        {
            if (canMoveXY(current.r(), current.c(), Actor::left) && m_maze[current.r()-1][current.c()] == 999)
            {
                m_maze[current.r()-1][current.c()] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()-1, current.c()));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::left) && m_maze[current.r()][current.c()] + 1 < m_maze[current.r()-1][current.c()])
            {
                m_maze[current.r()-1][current.c()] = m_maze[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()-1, current.c()));
            }
        }
    }
}

bool StudentWorld::mazeHasBoulder(int x, int y) const  // returns true if boulder in a 3.00 radius to (x,y)
{
    for (int i = 0; i < m_allActors.size(); i++)
    {
        if (m_allActors[i]->isObstacle() == true)   // loop through all actors to find an obstacle (boulder)
        {
            if (Pythagoras(x, y, m_allActors[i]->getX(), m_allActors[i]->getY()) <= 3.00)  // there is a boulder nearby
                return true;
        }
    }
    return false;
}

void StudentWorld::updateFrackManFinder()
{
    queue<Coord> coordQueue;    // create an empty queue
    
    // reinitialize the map
    
    for (int x = 0; x < 64; x++)   // set all spots in the array to 999 (999 means a spot hasn't been discovered yet)
    {
        for (int y = 0; y < 64; y++)
        {
            m_findFrackMan[x][y] = 999;
        }
    }
    
    Coord start(m_FrackMan->getX(), m_FrackMan->getY());
    
    coordQueue.push(start);
    
    m_findFrackMan[m_FrackMan->getX()][m_FrackMan->getY()] = 0; //already found FrackMan so the number of steps to move from there is 0
    
    while (!coordQueue.empty())
    {
        Coord current = coordQueue.front();    // get the value of the top item before popping
        coordQueue.pop();
        
        if (current.r() <= 60 && current.c()+1 <= 60 && current.r() >= 0 && current.c()+1 >= 0)   // can move NORTH
        {
            if (canMoveXY(current.r(), current.c(), Actor::up) && m_findFrackMan[current.r()][current.c()+1] == 999)
            {
                m_findFrackMan[current.r()][current.c()+1] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()+1));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::up) && m_findFrackMan[current.r()][current.c()] + 1 < m_findFrackMan[current.r()][current.c()+1])
            {
                m_findFrackMan[current.r()][current.c()+1] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()+1));
            }
        }
        
        if (current.r() <= 60 && current.c()-1 && current.r() >= 0 && current.c()-1 >= 0)   // can move SOUTH
        {
            if (canMoveXY(current.r(), current.c(), Actor::down) && m_findFrackMan[current.r()][current.c()-1] == 999)
            {
                m_findFrackMan[current.r()][current.c()-1] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()-1));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::down) && m_findFrackMan[current.r()][current.c()] + 1 < m_findFrackMan[current.r()][current.c()-1])
            {
                m_findFrackMan[current.r()][current.c()-1] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r(), current.c()-1));
            }
        }
        
        if (current.r()+1 <= 60 && current.c() <= 60 && current.r()+1 >= 0 && current.c() >= 0)   // can move RIGHT
        {
            if (canMoveXY(current.r(), current.c(), Actor::right) && m_findFrackMan[current.r()+1][current.c()] == 999)
            {
                m_findFrackMan[current.r()+1][current.c()] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()+1, current.c()));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::right) && m_findFrackMan[current.r()][current.c()] + 1 < m_findFrackMan[current.r()+1][current.c()])
            {
                m_findFrackMan[current.r()][current.c()-1] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()+1, current.c()));
            }
        }
        
        if (current.r()-1 <= 60 && current.c() <= 60 && current.r()-1 >= 0 && current.c() >= 0)   // can move LEFT
        {
            if (canMoveXY(current.r(), current.c(), Actor::left) && m_findFrackMan[current.r()-1][current.c()] == 999)
            {
                m_findFrackMan[current.r()-1][current.c()] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()-1, current.c()));
            }
            else if (canMoveXY(current.r(), current.c(), Actor::left) && m_findFrackMan[current.r()][current.c()] + 1 < m_findFrackMan[current.r()-1][current.c()])
            {
                m_findFrackMan[current.r()-1][current.c()] = m_findFrackMan[current.r()][current.c()] + 1;
                coordQueue.push(Coord(current.r()-1, current.c()));
            }
        }
    }
}

void StudentWorld::whichDirToFrackMan(HardcoreProtester* protester, Actor::Direction& first, Actor::Direction& second, Actor::Direction& third, Actor::Direction& fourth) // returns the direction that the protester should go in to reach FrackMan
{
    int x = protester->getX();
    int y = protester->getY();
    
    // a[0] = left, a[1] = right, a[2] = up, a[3] = down
    int stepsNeeded[4] = {m_findFrackMan[x-1][y], m_findFrackMan[x+1][y], m_findFrackMan[x][y+1], m_findFrackMan[x][y-1]};
    int posMin = 0;
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            if (stepsNeeded[i] < stepsNeeded[posMin])
                posMin = i;
        }
        
        stepsNeeded[posMin] = 1000;   // so you ignore that value when comparing in the future iterations
        
        switch (j)   // set the values for the best direction to go in
        {
            case 0:
                switch (posMin)
            {
                case 0: first = Actor::left; break;
                case 1: first = Actor::right; break;
                case 2: first = Actor::up; break;
                case 3: first = Actor::down; break;
            }
                break;
            case 1:
                switch (posMin)
            {
                case 0: second = Actor::left; break;
                case 1: second = Actor::right; break;
                case 2: second = Actor::up; break;
                case 3: second = Actor::down; break;
            }
                break;
            case 2:
                switch (posMin)
            {
                case 0: third = Actor::left; break;
                case 1: third = Actor::right; break;
                case 2: third = Actor::up; break;
                case 3: third = Actor::down; break;
            }
                break;
            case 3:
                switch (posMin)
            {
                case 0: fourth = Actor::left; break;
                case 1: fourth = Actor::right; break;
                case 2: fourth = Actor::up; break;
                case 3: fourth = Actor::down; break;
            }
                break;
        }
    }
}

int StudentWorld::getSquaresFromFrackMan(HardcoreProtester* protester)
{
    return m_findFrackMan[protester->getX()][protester->getY()];
}

bool StudentWorld::canMoveXY(int x, int y, Actor::Direction direction) const
{
    int addToX = 0;     // modify x or y for each possible direction to move in
    int addToY = 0;
    int obstacleX = 0;
    int obstacleY = 0;
    switch (direction)
    {
        case Actor::left:
            addToX = -1; obstacleX = -1; break;
        case Actor::right:
            addToX = 1; obstacleX = 4; break;
        case Actor::down:
            addToY = -1; obstacleY = -1; break;
        case Actor::up:
            addToY = 1; obstacleY = 4; break;
        case Actor::none:
            break;
    }
    
    if (x+addToX < 0 || x+addToX > 60 || y+addToY < 0 || y+addToY > 60)  // gone out of bounds
    {
        return false;
    }
    else if (direction == Actor::left || direction == Actor::right)
    {
        for (int k = y; k < y + 4; k++)   // runs into obstacle or dirt
        {
            if (isThereDirt(x + obstacleX, k) || isThereObstacle(x+obstacleX, y+obstacleY))
            {
                return false;
            }
        }
    }
    else if (direction == Actor::down || direction == Actor::up)
    {
        for (int k = x; k < x + 4; k++)
        {
            if (isThereDirt(k, y + obstacleY) || isThereObstacle(x+obstacleX, y+obstacleY))
            {
                return false;
            }
        }
    }
    return true;  // no dirt or obstacles, so can move
}