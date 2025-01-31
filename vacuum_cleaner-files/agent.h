/*
Author: Ju-ve Chankasemporn
E-mail: juvereturn@gmail.com
Brief: Vacuum cleaner agent that stores path as information
move through the maps until it no longer finds the unexplored path, then return
*/

#ifndef AGENT_H
#define AGENT_H
#include "definitions.h"
#include <iostream>
#include <set>
#include <stack>

//For storing position
struct Position
{
    int x, y;
};

class Agent 
{
  public:
    Agent(int random_seed);
    Action GetAction(Percept p);
  private: 
    int x, y;   
    Heading direction;                        
    int homeX, homeY;               
    bool startFakeBacktracking;
    Position revisitPosition;                       
    std::set<Position> visited;      
    std::set<Position> wallPos;      
    std::stack<Position> pathStack;  
    
    void moveForward();
    void turnLeft();
    void turnRight();
    void onHitWall();
    void debugPathStack();

    Action moveToUnexplored();
    Action moveForwardAvoidingWalls();
    Action backtrackToHome();
    Action fakeBacktrack();

    bool hasUnexploredNeighbor();
    bool hasWallInfront();
    bool shouldBacktrackToHome();

};

#endif
