#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// PART 1.1: BASE CLASS FOR ALL GAME OBJECTS
class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth);  // need params???
    virtual ~Actor();
    virtual void doSomething() = 0;
    bool isStillAlive();
    // must call setVisible() sometime?? but aren't barrels of oil not visible in the beginning??
    
private:
    bool m_stillAlive;
};

class Dirt: public Actor
{
public:
    Dirt(int startX, int startY);
    virtual ~Dirt();
    virtual void doSomething();
    
private:
    
};

class Person: public Actor
{
public:
    Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, int HP);
    virtual ~Person();
    
private:
    int m_healthPoints;
};

class FrackMan: public Person
{
public:
    FrackMan();
    virtual ~FrackMan();
    virtual void doSomething();
    
private:
    int m_numSquirts;
    int m_numSonars;
    int m_numNuggets;
};



#endif // ACTOR_H_
