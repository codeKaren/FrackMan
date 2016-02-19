#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);

    virtual int init();

    virtual int move();

    virtual void cleanUp();
    
    ~StudentWorld(); // frees dynamically allocated memory (FrackMan and Dirt)
    
    bool isThereDirt(int x, int y) const;   // returns true if there is dirt in the specified spot on the screen
    
    bool isThereObstacle(int x, int y) const;  // returns true if there is an obstacle in the specifed spot (boulder)
    
    bool radiusCloseToBoulder(int x, int y, Actor* actor) const;  // returns true if FrackMan is going to dig into a boulder
    
    void boulderSmash(Boulder* smasher);  // boulder hits FrackMan or protester 
    
    bool deleteDirt(Actor* actor);   // deletes dirt in position currently occupied by an actor
    
    void addActor(Actor* actor);  // add a new actor to StudentWorld
    
    void decreaseBarrelNum();    // decrease the number of barrels in the level by one
    
    void pickedUpByFrackMan(Goodies* goodie, char label);  // goodie can tell FrackMan it just received something
    
    bool closeToFrackMan(Actor* goodie, double howClose) const; // returns true if a goodie is close enough for FrackMan to pick it up, discover it, etc.
    
    void sonarFunction();  // makes all game objects within a radius of 12 visible
    
    double getRadiusBetween(Actor* first, Actor* second) const;   // finds the Euclidian distance between two actors

    int max(int a, int b);  // returns the bigger number
    
    int min(int a, int b);  // returns the smaller number 
    
private:
    Dirt* m_dirt[64][64];
    FrackMan* m_FrackMan;
    std::vector<Actor*> m_allActors;
    int m_numBarrels;
};

#endif // STUDENTWORLD_H_
