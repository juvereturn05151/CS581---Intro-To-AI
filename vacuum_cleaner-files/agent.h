#ifndef AGENT_H
#define AGENT_H
#include "definitions.h"
#include <iostream>
#include <set>
#include <stack>
#include <tuple>

struct Position {
    int x, y;

    bool operator<(const Position& other) const {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

class Agent {
  public:
    Agent(int random_seed);
    Action GetAction(Percept p);
  private: 
    int x, y;                        
    int homeX, homeY;               
    Heading direction;             
    bool isCleaning;                 
    std::set<Position> visited;      
    std::set<Position> wallPos;      
    std::stack<Position> pathStack;  


    void moveForward();

    void turnLeft();

    void turnRight();

    Action backtrackToHome();

    void turnTo(Heading targetDirection);

    bool hasUnexploredNeighbor();

    bool hasWallInfront();

    Action moveToUnexplored();

    void onHitWall();

    Action moveForwardAvoidingWalls();

    Action navigateToHome();

};

#endif
