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
    virtual void doSomething();
    // must call setVisible()
    
private:
    bool stillAlive;
};

class Dirt: public Actor
{
public:
    Dirt(int startX, int startY);   // need params???
    virtual ~Dirt();
    
private:
    
};

class Person: public Actor
{
public:
    Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth);
    virtual ~Person();
    
private:
};

class FrackMan: public Person
{
public:
    FrackMan(); // need params??
    virtual ~FrackMan();
    virtual void doSomething();
    
private:
    
};



#endif // ACTOR_H_
