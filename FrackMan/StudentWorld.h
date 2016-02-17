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
    
    bool deleteDirt(Actor* actor);   // deletes dirt in position currently occupied by an actor
    
    void addActor(Actor* actor);  // add a new actor to StudentWorld
    
    double getRadiusBetween(Actor* first, Actor* second) const;   // finds the Euclidian distance between two actors

private:
    Dirt* m_dirt[64][64];
    FrackMan* m_FrackMan;
    std::vector<Actor*> m_allActors;
};

#endif // STUDENTWORLD_H_
