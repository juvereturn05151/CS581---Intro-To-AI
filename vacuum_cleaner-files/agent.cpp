#include <ctime> //for random seed
#include <cmath>
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : facing(NORTH), currentX(0), currentY(0), agentState(BlindlyMove), stuckOnSameSpotCount(0)
,startTurning180degree(false), turning90degreeCounter(0), hasTurn180degree(false), firstMove(true)
{
  // supplying your own seed may help debugging, same seed will cause
  // same random number sequence
  if (random_seed == 0)
    std::srand(static_cast<unsigned>(std::time(0))); // random seed from time
  else
    std::srand(random_seed); // random seed from user

  agentPositionHistory.push_back({currentX, currentY});
  notdeleteAgentPositionHistory.push_back({currentX, currentY});
  unexplored.clear();
}

Action Agent::GetAction(Percept p) {
    if (p.dirt) {
        return SUCK;
    }

    std::cout << "agent pos: " << currentX << " , " << currentY << std::endl;
    std::cout << "current direction: " << (Heading)facing << std::endl;
    std::cout<<"stuckOnSameSpotCount: "<< stuckOnSameSpotCount<<std::endl;
    std::cout << "agent state: " << (AgentState)agentState << std::endl;
    if (p.bump) {
        std::cout << "bump " << std::endl;
        return TurnRightOnHitWall();
    }

    if (p.home) {
        if (agentState == AgentState::ReturnToBase) {
            return SHUTOFF;
        }
    }



    return Move();
}

Action Agent::Move() {
    if (agentState == AgentState::GoToUnexplored) {
        return GoToUnexplored();
    }
    else if (agentState == AgentState::ReturnToBase) {
        return Backtrack();
    }
    else {
        return MoveForward();
    }
}

Action Agent::GoToUnexplored()
{
    printHistoryDebug();
    IsThereUnexploredCoordinates();
  return Backtrack();
    
}

Action Agent::Backtrack() {
    if(agentPositionHistory.size() <= 0)
    {
        return NOOP;
    }
    Position prevPos = agentPositionHistory.back();
    if (prevPos.x == currentX && prevPos.y == currentY) {
        agentPositionHistory.pop_back();
        return NOOP;
    }
    switch (facing) {
    case NORTH:
        if (prevPos.x == currentX && prevPos.y == currentY + 1) {
            agentPositionHistory.pop_back();
            currentX = prevPos.x;
            currentY = prevPos.y;
            return FORWARD;
        } else {
            return TurnRightWithoutHistory();
        }

    case EAST:
        if (prevPos.x == currentX + 1 && prevPos.y == currentY) {
            agentPositionHistory.pop_back();
            currentX = prevPos.x;
            currentY = prevPos.y;
            return FORWARD;
        } else {
            return TurnRightWithoutHistory();
        }
        break;

    case SOUTH:
        if (prevPos.x == currentX && prevPos.y == currentY - 1) {
            agentPositionHistory.pop_back();
            currentX = prevPos.x;
            currentY = prevPos.y;
            return FORWARD;
        } else {
            return TurnRightWithoutHistory();
        }
        break;

    case WEST:
        if (prevPos.x == currentX - 1 && prevPos.y == currentY) {
            agentPositionHistory.pop_back();
            currentX = prevPos.x;
            currentY = prevPos.y;
            return FORWARD;
        } else {
            return TurnRightWithoutHistory();
        }
        break;
    }
    return NOOP;
}

Action Agent::MoveForward() {
    switch (facing) {
    case NORTH:
        if (IsVisited(currentX, currentY + 1) || IsWall(currentX, currentY + 1)) {
            stuckOnSameSpotCount++;
            if (stuckOnSameSpotCount >= 4) {
                CheckWhatToDoNext();
            }
            return TurnRight();
        }
        currentY++;
        break;
    case EAST:
        if (IsVisited(currentX + 1, currentY)|| IsWall(currentX + 1, currentY)) {
          stuckOnSameSpotCount++;
            if (stuckOnSameSpotCount >= 4) {
                CheckWhatToDoNext();
            }
            return TurnRight();
        }
        currentX++;
        break;
    case SOUTH:
        if (IsVisited(currentX, currentY - 1)|| IsWall(currentX, currentY - 1)) {
          stuckOnSameSpotCount++;
            if (stuckOnSameSpotCount >= 4) {
                CheckWhatToDoNext();
            }
            return TurnRight();
        }
        currentY--;
        break;
    case WEST:
        if (IsVisited(currentX - 1, currentY)|| IsWall(currentX - 1, currentY)) {
          stuckOnSameSpotCount++;
            if (stuckOnSameSpotCount >= 4) {
                CheckWhatToDoNext();
            }
            return TurnRight();
        }
        currentX--;
        break;
    }
    stuckOnSameSpotCount = 0;
    agentPositionHistory.push_back({currentX, currentY});
      notdeleteAgentPositionHistory.push_back({currentX, currentY});
    actionHistory.push(FORWARD);
    return FORWARD;
}

void Agent::printHistoryDebug() {
    for (auto it = agentPositionHistory.begin(); it != agentPositionHistory.end(); ++it) {
        std::cout << "x = " << it->x << ", y = " << it->y << std::endl;
    }
}

Action Agent::TurnRight() {
    switch (facing) {
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

Action Agent::TurnRightWithoutHistory() {
    switch (facing) {
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

Action Agent::TurnRightOnHitWall() {
    int wallPosX = 0;
    int wallPosY = 0;
    int tempAgentPosX = currentX;
    int tempAgentPosY = currentY;

    wallPositions.push_back({currentX, currentY});
    agentPositionHistory.pop_back();
    stuckOnSameSpotCount++;

    switch (facing) {
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

    return TurnRight();
}

bool Agent::IsVisited(int x, int y) {
    for (auto it = notdeleteAgentPositionHistory.begin(); it != notdeleteAgentPositionHistory.end(); ++it) {
        if (it->x == x && it->y == y) {
            return true;
        }
    }
    return false;
}

bool Agent::IsWall(int x, int y)
{
      for (auto it = wallPositions.begin(); it != wallPositions.end(); ++it) {
        if (it->x == x && it->y == y) {
            return true;
        }
    }

    return false;
}

void Agent::CalculateBoundary() {
    for (auto it = agentPositionHistory.begin(); it != agentPositionHistory.end(); ++it) {
        if (it->x <= xMinBoundary) {
            xMinBoundary = it->x;
        }
        if (it->x >= xMaxBoundary) {
            xMaxBoundary = it->x;
        }
        if (it->y <= yMinBoundary) {
            yMinBoundary = it->y;
        }
        if (it->y >= yMaxBoundary) {
            yMaxBoundary = it->y;
        }
    }

    for (auto it = wallPositions.begin(); it != wallPositions.end(); ++it) {
        if (it->x <= xWallMinBoundary) {
            xWallMinBoundary = it->x;
        }
        if (it->x >= xWallMaxBoundary) {
            xWallMaxBoundary = it->x;
        }
        if (it->y <= yWallMinBoundary) {
            yWallMinBoundary = it->y;
        }
        if (it->y >= yWallMaxBoundary) {
            yWallMaxBoundary = it->y;
        }
    }
}

void Agent::FindUnexploredCoordinates() {
unexplored.clear();

    // Iterate through all positions within the agent boundary
    for (int x = xMinBoundary; x <= xMaxBoundary; ++x) {
        for (int y = yMinBoundary; y <= yMaxBoundary; ++y) {
            Position current = {x, y};
            bool isVisited = false;
            bool isWall = false;

            if(notdeleteAgentPositionHistory.size() > 0 )
            {
                for (auto it = notdeleteAgentPositionHistory.begin(); it != notdeleteAgentPositionHistory.end(); ++it) {
                if (it->x == x && it->y == y) {
                    isVisited = true;
                }
                }
            }

            if(wallPositions.size() > 0 )
            {
                for (auto it = wallPositions.begin(); it != wallPositions.end(); ++it) {
                if (it->x == x && it->y == y) {
                    isWall = true;
                }
            }
            }


            if (!isVisited && !isWall) {
                unexplored.push_back(current);
            }
        }
    }

    std::cout << "Unexplored coordinates:\n";
    for (const auto& pos : unexplored) {
        std::cout << "(" << pos.x << ", " << pos.y << ")\n";
    }
}

bool Agent::IsThereUnexploredCoordinates()
{
  CalculateBoundary();
  FindUnexploredCoordinates();

  return unexplored.size()>0;
}

void Agent::CheckWhatToDoNext()
{
  if(IsThereUnexploredCoordinates())
  {
    agentState = AgentState::GoToUnexplored;
  }else 
  {
    agentState = AgentState::ReturnToBase;
  }
  
}