#ifndef AGENT_H
#define AGENT_H
#include "definitions.h"
#include <utility>
#include <list>
#include <stack>

struct Position
{
  int x;
  int y;
};


class Agent {
  public:
    Agent(int random_seed);
    Action GetAction(Percept p);
  private: 
    Heading facing;
    int currentX;
    int currentY;
    bool isReturningToBase;
    int stuckOnSameSpotCount;
    bool startTurning180degree;
    int turning90degreeCounter;
    bool hasTurn180degree;
    std::list<Position> agentPositionHistory; 
    std::list<Position> wallPositions; 
    std::stack<Action> actionHistory; 

    Action Move();
    Action MoveForward();
    Action Backtrack();
    Action TurnRight();
    Action TurnRightOnHitWall();
    bool IsVisisted(int x, int y);
};

#endif
