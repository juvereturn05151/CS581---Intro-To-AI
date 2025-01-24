#include <ctime> //for random seed
#include <cmath>
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : facing(NORTH), currentX(0), currentY(0), isReturningToBase(false), stuckOnSameSpotCount(0)
,startTurning180degree(false), turning90degreeCounter(0), hasTurn180degree(false)
{
  // supplying your own seed may help debugging, same seed will cause
  // same random number sequence
  if (random_seed == 0)
    std::srand(static_cast<unsigned>(std::time(0))); // random seed from time
  else
    std::srand(random_seed); // random seed from user

  agentPositionHistory.push_back({currentX, currentY});
}

Action Agent::GetAction(Percept p)
{
  // straight-forward beahavior
  if (p.dirt)
  {
    return SUCK;
  }

  if (p.bump)
  {
    return TurnRightOnHitWall();
  }

  if (p.home)
  {
    if (isReturningToBase)
    {
      return SHUTOFF;
    }
  }

  return Move();
}

Action Agent::Move()
{
  if (isReturningToBase)
  {
    return Backtrack();
  }
  else
  {
    return MoveForward();
  }
}

Action Agent::Backtrack()
{
  Action action = Action::NOOP;
  if (!actionHistory.empty()) {
        action = actionHistory.top();
        std::cout<<"pop "<<action<<std::endl;
    }
    
    if (action == RIGHT)
    {
      actionHistory.pop();
      return LEFT;
    }
    else if(action == FORWARD) 
    {
      if(hasTurn180degree)
      {
        actionHistory.pop();
        hasTurn180degree = false;
        return FORWARD;
      }else
      {
      startTurning180degree = true;
      }

    }

    if(startTurning180degree)
    {
      turning90degreeCounter++;
      std::cout<<"turning90degreeCounter "<<turning90degreeCounter<<std::endl;
      if(turning90degreeCounter == 2)
      {
        startTurning180degree = false;
         hasTurn180degree = true;
         turning90degreeCounter = 0;
      }
      return LEFT;
    }
    
    return action;
 
}

Action Agent::MoveForward()
{
  switch (facing)
  {
  case NORTH:
    if (IsVisisted(currentX, currentY + 1))
    {
      return TurnRight();
    }
    currentY++;
    break;
  case EAST:
    if (IsVisisted(currentX + 1, currentY))
    {
      return TurnRight();
    }
    currentX++;
    break;
  case SOUTH:
    if (IsVisisted(currentX, currentY - 1))
    {
      return TurnRight();
    }
    currentY--;
    break;
  case WEST:
    if (IsVisisted(currentX - 1, currentY))
    {
      return TurnRight();
    }
    currentX--;
    break;
  }

  agentPositionHistory.push_back({currentX, currentY});
  actionHistory.push(FORWARD);
  return FORWARD;
}

Action Agent::TurnRight()
{
  switch (facing)
  {
  case NORTH:
    facing = EAST;
    break;
  case EAST:
    facing = SOUTH;
    break;
  case SOUTH:
    facing = WEST;
    break;
  case WEST:
    facing = NORTH;
    break;
  }

  actionHistory.push(RIGHT);
  return RIGHT;
}

Action Agent::TurnRightOnHitWall()
{
  int wallPosX = 0;
  int wallPosY = 0;
  int tempAgentPosX = currentX;
  int tempAgentPosY = currentY;

  switch (facing)
  {
  case NORTH:
    wallPosY = ++tempAgentPosY;
    break;
  case EAST:
    wallPosX = ++tempAgentPosX;
    break;
  case SOUTH:
    wallPosY = --tempAgentPosY;
    break;
  case WEST:
    wallPosX = --tempAgentPosX;
    break;
  }

  wallPositions.push_back({wallPosX, wallPosY});
  return TurnRight();
}

bool Agent::IsVisisted(int x, int y)
{
  for (auto it = agentPositionHistory.begin(); it != agentPositionHistory.end(); ++it)
  {
    if (it->x == x && it->y == y)
    {
      stuckOnSameSpotCount++;
      if (stuckOnSameSpotCount >= 4)
      {
        isReturningToBase = true;
      }
      return true;
    }
  }
  stuckOnSameSpotCount = 0;
  return false;
}