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
#include<tuple>

//For storing position
struct Position
{
    int x, y;

    bool operator<(const Position& other) const 
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    bool operator==(const Position& other) const 
    {
        return x == other.x && y == other.y;
    }
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
                   
    bool isPreBacktracking;
    Position revisitPosition;

    std::set<Position> visited;      
    std::set<Position> wallPos;      
    std::stack<Position> pathStack;  
    
    void moveForward();
    void turnLeft();
    void turnRight();
    void onHitWall();
    void debugPathStack();
    //prebacktrack is special backtrack when the agent visits the same position
    //the purpose is to delete duplicated paths in the pathStack
    //if not doing this, it can cause infinite loop
    void setPrebacktrack(Position target);

    Action moveToUnexplored();
    Action moveForwardAvoidingWalls();
    Action backtrackToHome();
    Action preBacktrack();

    bool hasUnexploredNeighbor();
    bool hasWallInfront();
    bool shouldBacktrackToHome();
    bool isVisitTheSamePosition(Position target);

};

#endif
