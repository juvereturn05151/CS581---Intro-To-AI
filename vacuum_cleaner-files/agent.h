#ifndef AGENT_H
#define AGENT_H
#include "definitions.h"
#include <utility>
#include <list>
#include <stack>
#include <set>

struct Position
{
  int x;
  int y;
};

enum AgentState
{
  BlindlyMove,
  GoToUnexplored,
  ReturnToBase
};


class Agent {
  public:
    Agent(int random_seed);
    Action GetAction(Percept p);
  private: 
    Heading facing;
    int currentX;
    int currentY;
    AgentState agentState;
    int stuckOnSameSpotCount;
    bool startTurning180degree;
    int turning90degreeCounter;
    bool hasTurn180degree;
    bool firstMove;
    std::list<Position> agentPositionHistory; 
        std::list<Position> notdeleteAgentPositionHistory; 
    std::list<Position> wallPositions; 
    std::stack<Action> actionHistory; 
    std::list<Position> unexplored;
    Action lastestAction;
    float xMinBoundary = 0;
    float xMaxBoundary = 0;
    float yMinBoundary = 0;
    float yMaxBoundary = 0;
    float xWallMinBoundary = 0;
    float xWallMaxBoundary = 0;
    float yWallMinBoundary = 0;
    float yWallMaxBoundary = 0;

    Action Move();
    Action MoveForward();
    Action Backtrack();
    Action TurnRight();
    Action TurnRightWithoutHistory();
    Action TurnRightOnHitWall();
    Action GoToUnexplored();
    bool IsVisited(int x, int y);
    bool IsWall(int x, int y);
    bool IsThereUnexploredCoordinates();
    void printHistoryDebug();
    void CalculateBoundary();
    void FindUnexploredCoordinates();
    void CheckWhatToDoNext();
};

#endif
