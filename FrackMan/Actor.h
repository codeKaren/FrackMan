#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

// BASE CLASS FOR ALL GAME OBJECTS
class Actor: public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, StudentWorld* world);
    virtual ~Actor();
    virtual void doSomething() = 0;
    bool isStillAlive() const;
    void makeDead();
    bool isVisible() const;   // returns true if visible by FrackMan
    void makeVisible();  // sets m_visible to true
    virtual bool isObstacle() const;    // returns true if object is an obstacle (currently only boulders)
    StudentWorld* whereAmI() const;  // returns m_studentWorld
    bool canMove(Direction direction) const;  // returns true if the actor can move in the specified direction
    bool tryToMove(Direction direction);   // used inside doSomething()
    virtual bool canGetAnnoyed() const;  // returns true if it is a Person class object
    
private:
    bool m_stillAlive;
    bool m_visible;
    StudentWorld* m_studentWorld;
};

// ALL OBJECT CLASSES DERIVED DIRECTLY FROM THE ACTOR BASE CLASS
class Dirt: public Actor
{
public:
    Dirt(int startX, int startY, StudentWorld* world);
    virtual ~Dirt();
    virtual void doSomething();
};

class Boulder: public Actor
{
public:
    Boulder(int startX, int startY, StudentWorld* world);
    virtual ~Boulder();
    virtual void doSomething();
    virtual bool isObstacle() const;
private:
    bool m_stableState;  // true if Boulder is in a stable state, false if Boulder is in a waiting state
    int numTicksWaiting;        // counts how many ticks a Boulder has spent in a waiting state
};

class Squirt: public Actor
{
public:
    Squirt(int x, int y, Direction direction, StudentWorld* world);
    virtual ~Squirt();
    virtual void doSomething();
    
private:
    void decreaseDistance();
    int m_travelDistance;
};

// OIL BARRELS, NUGGETS, SONAR KIT, AND WATER POOL CLASSES WHICH ARE DERIVED FROM A GOODIES CLASS
class Goodies: public Actor
{
public:
    Goodies(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, bool visible, int numTicks, StudentWorld* world);
    virtual ~Goodies();
    int howManyTicksLeft() const;   // returns how many ticks the goodie has left before it must disappear
    void decreaseNumTicks();  // decreases the number of ticks by one
private:
    int m_numTicks;
};

class OilBarrel: public Goodies    // *** ALL DONE *** //
{
public:
    OilBarrel(int startX, int startY, StudentWorld* world);
    virtual ~OilBarrel();
    virtual void doSomething();
    
private:
    bool m_visible;
};

class Nugget: public Goodies   // GOODIE CHAR 'n'
{
public:
    Nugget(int startX, int startY, bool visible, bool pickupState, bool permanentState, StudentWorld* world);
    virtual ~Nugget();
    virtual void doSomething();

private:
    bool m_pickupableByFrackMan;
    bool m_permanentState;
    int m_numTicks;
};

class SonarKit: public Goodies   // GOODIE CHAR 's'
{
public:
    SonarKit(int numTicks, StudentWorld* world);
    virtual ~SonarKit();
    virtual void doSomething();
    
private:
    
};

class WaterPool: public Goodies   // GOODIE CHAR 'w'
{
public:
    WaterPool(int startX, int startY, int numTicks, StudentWorld* world);
    virtual ~WaterPool();
    virtual void doSomething();
    
private:
};

// FRACKMAN AND PROTESTOR CLASSES WHICH ARE DERIVED FROM A PERSON CLASS
class Person: public Actor
{
public:
    Person(int imageID, int startX, int startY, Direction startDirection, float size, unsigned int depth, int HP, StudentWorld* world);
    virtual ~Person();
    virtual bool canGetAnnoyed() const; // returns true for objects derived from this class
    virtual void getAnnoyed(int decreaseHP) = 0;
    void decreaseHealthPoints(int howMany);  // decreases m_healthPoints by the number specified in the parameter
    int getHealthPoints() const;  // returns m_healthPoints
    
private:
    int m_healthPoints;
};

class FrackMan: public Person
{
public:
    FrackMan(StudentWorld* world);
    virtual ~FrackMan();
    virtual void doSomething();
    virtual void getAnnoyed(int decreaseHP);
    void addToInventory(Goodies* goodie, char label);
    // accessors
    int getNumSquirts() const;  // return m_numSquirts
    int getNumSonars() const;  // return m_numSonars
    int getNumNuggets() const;  // return m_numNuggets
    
private:
    int m_numSquirts;
    int m_numSonars;
    int m_numNuggets;
    void moveOrDig(Direction direction, int addToX, int addToY);  // let FrackMan move or dig, used in doSomething()
};

class Protester: public Person
{
public:
    Protester(StudentWorld* world, int imageID = IID_PROTESTER, int HP = 5);
    virtual ~Protester();
    virtual void doSomething();
    virtual void getAnnoyed(int decreaseHP);
    // Accessor functions
    int getNumTicksTotal() const; // returns m_numTicksWaiting
    int getNumTicksLeft() const; // returns m_numTicksLeft
    bool isLeaveOilFieldState() const; // returns m_leaveOilFieldState
    int getNumTicksSinceShout() const;  // return m_numTicksSinceShout
    int getNumSquaresInCurrDir() const;  // get m_numSquaresToMoveInCurrentDirection
    int getNumTicksSinceTurned() const;  // get m_numTicksSinceTurned
    virtual bool isHardcore() const; // returns false for a regular protester
    // Mutator functions
    void timePasses(); // decrements m_numTicksLeft
    void setNumTicksLeft(int numTicksLeft);  // sets m_numTicksLeft to whatever you want it to be
    void setLeaveOilField(); // changes m_leaveOilFieldState to true
    void setNumTicksShout(int numTicksShout);  // change the value of m_numTicksSinceShout
    void setNumSquaresToMoveInCurrDir(int squares);  // change the value of m_numSquaresToMoveInCurrentDirection
    void setNumTicksSinceTurned(int numTicksSinceTurned); // change the value of m_numTicksSinceTunred
    
private:
    int m_numSquaresToMoveInCurrentDirection;
    int m_numTicksTotal;
    int m_numTicksLeft;
    int m_numTicksSinceShout;
    int m_numTicksSinceTurned;
    bool m_leaveOilFieldState;
    int howManySquaresInCurrentDir();
    virtual bool doDifferentiatedStuff();  // add one different behavior to doSomething for the hardcore protester
};

class HardcoreProtester: public Protester
{
public:
    HardcoreProtester(StudentWorld* world);
    virtual ~HardcoreProtester();
    virtual void getAnnoyed(int decreaseHP);
    virtual bool isHardcore() const; // returns true for a hardcore protester
    
private:
    virtual bool doDifferentiatedStuff();  // add a different behavior to the protester class's doSomething() function

};

#endif // ACTOR_H_
