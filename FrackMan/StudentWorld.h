#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Coord    // use in the maze searching algorithm
{
public:
    Coord(int rr, int cc) : m_r(rr), m_c(cc) {}
    int r() const { return m_r; }
    int c() const { return m_c; }
private:
    int m_r;
    int m_c;
};

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);

    virtual int init();

    virtual int move();

    virtual void cleanUp();
    
    ~StudentWorld(); // frees dynamically allocated memory (FrackMan and Dirt)
    
    void updateDisplayText();  // updates the text at the top of the screen during each move() call
    
    bool isThereDirt(int x, int y) const;   // returns true if there is dirt in the specified spot on the screen
    
    bool isThereObstacle(int x, int y) const;  // returns true if there is an obstacle in the specifed spot (boulder)
    
    bool radiusCloseToBoulder(int x, int y, Actor* actor) const;  // returns true if FrackMan is going to dig into a boulder
    
    void boulderSmash(Boulder* smasher);  // boulder hits FrackMan or protester
    
    void waterGun(Squirt* squirt);  // squirt hits protester
    
    bool deleteDirt(Actor* actor);   // deletes dirt in position currently occupied by an actor
    
    void addActor(Actor* actor);  // add a new actor to StudentWorld
    
    void decreaseBarrelNum();    // decrease the number of barrels in the level by one
    
    void pickedUpByFrackMan(Goodies* goodie, char label);  // goodie can tell FrackMan it just received something
    
    bool pickedUpByProtester(Nugget* nugget);  // check to see if nugget is picked up by the protester
    
    bool closeToFrackMan(Actor* actor, double howClose) const; // returns true if a goodie is close enough for FrackMan to pick it up, discover it, etc.
    
    bool isFacingFrackMan(Protester* protester) const; // returns true if a protester is facing FrackMan
    
    bool isInLineOfSight(Protester* protester) const; // returns true if a protester is in direct line of sight to FrackMan
    
    bool canMoveToFrackMan(Protester* protester);  // returns true if the protester can continue moving in the current direction to reach FrackMan; makes the protester face FrackMan if it returns true
    
    void sonarFunction();  // makes all game objects within a radius of 12 visible
    
    void annoyFrackMan(int decreaseHP);  // call the getAnnoyed() function in FrackMan
    
    // MATH LIKE FUNCTIONS BELOW
    
    double getRadiusBetween(Actor* first, Actor* second) const;   // finds the Euclidian distance between two actors

    int max(int a, int b);  // returns the bigger number
    
    int min(int a, int b);  // returns the smaller number
    
    int generateRandomNumber(int startNum, int endNum);  // returns a random number between startNum and endNum
    
    Actor::Direction generateRandomDirection();  // returns a random direction
    
    Actor::Direction whichDirectionToGoIn(Protester* protester);  // returns the direction that the protester should go in to reach the exit
    
    void updateMaze();  // updates the array for the protesters to use to find what direction they should move in 
    
    
private:
    Dirt* m_dirt[64][64];
    FrackMan* m_FrackMan;
    std::vector<Actor*> m_allActors;
    int m_numBarrels;
    int m_numTicksSinceAddedProtester;
    int m_numProtesters;
    
    // maze searching array
    Actor::Direction m_maze[64][64];
    
    double Pythagoras(int x1, int y1, int x2, int y2) const;  // finds the Euclidean distance between two points
    
    void getNewPosition(int xBound1, int yBound1, int xBound2, int yBound2, int& newX, int& newY);  // get a position for a new object to be generated based on existing x and y boundaries and place the position in (newX, newY)
    
    bool isValidWaterPoolSpot(int x, int y) const;   // returns true if it is a valid spot for the WaterPool to spawn
    
    bool mazeHasBoulder(int x, int y) const;  // returns true if there is a boulder in a 3.00 radius from (x,y)
};

#endif // STUDENTWORLD_H_
