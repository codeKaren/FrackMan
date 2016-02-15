#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// PART 1.1: BASE CLASS FOR ALL GAME OBJECTS
class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;
    bool isStillAlive() const;
    StudentWorld* whereAmI() const;
    
private:
    bool m_stillAlive;
    StudentWorld* m_studentWorld;
};

class Dirt: public Actor
{
public:
    Dirt(int startX, int startY, StudentWorld* world);
    virtual ~Dirt();
    virtual void doSomething();
    
private:
    
};

class Person: public Actor
{
public:
    Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, int HP, StudentWorld* world);
    virtual ~Person();
    
private:
    int m_healthPoints;
};

class FrackMan: public Person
{
public:
    FrackMan(StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    
private:
    int m_numSquirts;
    int m_numSonars;
    int m_numNuggets;
};



#endif // ACTOR_H_
