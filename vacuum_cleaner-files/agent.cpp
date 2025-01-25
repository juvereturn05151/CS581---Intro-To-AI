#include <ctime> //for random seed
#include <cmath>
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : facing(NORTH), currentX(0), currentY(0), isReturningToBase(false), stuckOnSameSpotCount(0)
,startTurning180degree(false), turning90degreeCounter(0), hasTurn180degree(false), firstMove(true)
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

  /*if(firstMove)
  {
    firstMove = false;
    std::cout<<"first move"<<std::endl;
    return NOOP;
  }*/

  std::cout<<"agent pos: "<< currentX<<" , "<<currentY<<std::endl;
  std::cout<<"current direction: "<< (Heading)facing<<std::endl;

  if (p.bump)
  {
     std::cout<<"bump "<<std::endl;
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
   Position prevPos = agentPositionHistory.back();
   if(prevPos.x == currentX && prevPos.y == currentY)
   {
    agentPositionHistory.pop_back();
    return NOOP;
   }
   switch (facing)
    {
    case NORTH:
      if(prevPos.x == currentX && prevPos.y == currentY + 1)
      {
        agentPositionHistory.pop_back();
        currentX = prevPos.x;
        currentY = prevPos.y;
        return FORWARD;
      }else
      {
        return TurnRightWithoutHistory();
      }
      
      break;
    case EAST:
      if(prevPos.x == currentX + 1&& prevPos.y == currentY)
      {
        agentPositionHistory.pop_back();
        currentX = prevPos.x;
        currentY = prevPos.y;
        return FORWARD;
      }else
      {
        return TurnRightWithoutHistory();
      }
      break;
    case SOUTH:
      if(prevPos.x == currentX && prevPos.y == currentY - 1)
      {
        agentPositionHistory.pop_back();
        currentX = prevPos.x;
        currentY = prevPos.y;
        return FORWARD;
      }else
      {
        return TurnRightWithoutHistory();
      }
      break;
    case WEST:
      if(prevPos.x == currentX - 1 && prevPos.y == currentY )
      {
        agentPositionHistory.pop_back();
        currentX = prevPos.x;
        currentY = prevPos.y;
        return FORWARD;
      }else
      {
        return TurnRightWithoutHistory();
      }
      break;
    }
    return NOOP;
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
  printHistoryDebug();
    if (IsVisisted(currentX - 1, currentY))
    {
      return TurnRight();
    }
    currentX--;
    break;
  }
         std::cout<<"move forward"<<std::endl;
  agentPositionHistory.push_back({currentX, currentY});
  actionHistory.push(FORWARD);
  return FORWARD;
}

void Agent::printHistoryDebug()
{
  for (auto it = agentPositionHistory.begin(); it != agentPositionHistory.end(); ++it)
  {
    std::cout<< "x = " << it->x <<"y = " <<it->y<<std::endl;
  }
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

Action Agent::TurnRightWithoutHistory()
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

  return RIGHT;
}

Action Agent::TurnRightOnHitWall()
{
  int wallPosX = 0;
  int wallPosY = 0;
  int tempAgentPosX = currentX;
  int tempAgentPosY = currentY;
  agentPositionHistory.pop_back();
  stuckOnSameSpotCount++;
  switch (facing)
  {
  case NORTH:
    wallPosY = ++tempAgentPosY;
    currentY--;
    break;
  case EAST:

    wallPosX = ++tempAgentPosX;
    currentX--;
    break;
  case SOUTH:
    wallPosY = --tempAgentPosY;
        currentY++;
    break;
  case WEST:
    wallPosX = --tempAgentPosX;
        currentX++;
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
      std::cout<<"stuckOnSameSpotCount: "<< stuckOnSameSpotCount<<std::endl;
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