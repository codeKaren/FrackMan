#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// PART 1.1: BASE CLASS FOR ALL GAME OBJECTS
class Actor: public GraphObject
{
public:
    // simple constructor
    ~Actor(); // destructor
    virtual void doSomething() {}
    // must call setVisible()
    
    
private:
    bool stillAlive;
};

class Dirt: public Actor
{
    // simple constructor
    ~Dirt();
    
};

class Person: public Actor
{
    
};

class FrackMan: public Person
{
    // simple constructor
    ~FrackMan();
    virtual void doSomething();
    
};



#endif // ACTOR_H_
