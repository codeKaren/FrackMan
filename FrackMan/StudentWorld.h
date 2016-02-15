#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);

    virtual int init();

    virtual int move();

    virtual void cleanUp();
    
    ~StudentWorld(); // frees dynamically allocated memory (FrackMan and Dirt)
    
    bool isThereDirt(int x, int y);   // returns true if there is dirt in the specified spot on the screen

private:
    Dirt* m_dirt[64][64];
    FrackMan* m_FrackMan;
};

#endif // STUDENTWORLD_H_
