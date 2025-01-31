#include <ctime> //for random seed
#include <cmath> 
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : x(0), y(0), direction(NORTH), homeX(0), homeY(0)  { 
  //supplying your own seed may help debugging, same seed will cause 
  //same random number sequence 
  if (random_seed==0) std::srand( static_cast<unsigned>(std::time(0))); // random seed from time
  else                std::srand( random_seed ); // random seed from user

  isPreBacktracking = false;
  visited.insert({x, y});
} 

Action Agent::GetAction(Percept p) 
{
    debugPathStack();

    if (p.dirt) 
    {
        return SUCK;
    }

    if (p.bump) 
    {
        onHitWall();
        turnLeft();
        return LEFT;
    }

    if (shouldBacktrackToHome()) 
    {
        printf("All paths explored. Backtracking to home.");
        return backtrackToHome();
    }

    if(isPreBacktracking)
    {
        printf("isPreBacktracking x:");
        std::cout<<revisitPosition.x<<" y:"<<revisitPosition.y<<std::endl;
        return preBacktrack();
    }

    if (hasUnexploredNeighbor()) 
    {
        printf("Exploring unexplored neighbor.\n");
        return moveToUnexplored();
    } 
    else if(!hasWallInfront())
    {
        printf("moveForwardAvoidingWalls\n");
        return moveForwardAvoidingWalls();
    } 
    else
    {
        printf("No unexplored neighbors. Switching to backtracking mode.\n");
        return backtrackToHome();
    }
}

void Agent::moveForward() 
{
    //record the position before moving
    pathStack.push({x, y});

    //move forward based on the facing direction
    switch (direction) 
    {
        case NORTH: y++; break;
        case EAST:  x++; break;
        case SOUTH: y--; break;
        case WEST:  x--; break;
    }

    Position target = {x,y};
    //if visit the same position, set pre-backtrack
    if (isVisitTheSamePosition(target)) 
    {
        setPrebacktrack(target);
    }

    //mark the new position as visited
    visited.insert({x, y});
}

void Agent::onHitWall()
{
    wallPos.insert({x, y});
    pathStack.pop();
    switch (direction) 
    {
        case NORTH: y--; break;
        case EAST:  x--; break;
        case SOUTH: y++; break;
        case WEST:  x++; break;
    }
}

void Agent::turnLeft() 
{
    direction = static_cast<Heading>((direction + 3) % 4);
}

void Agent::turnRight() 
{
    direction = static_cast<Heading>((direction + 1) % 4);
}

void Agent::setPrebacktrack(Position target)
{
    isPreBacktracking = true;
    revisitPosition = target;
    pathStack.pop();
}

void Agent::debugPathStack() 
{
    std::stack<Position> tempStack = pathStack;
    std::cout << "Current Path Stack: ";
    while (!tempStack.empty()) 
    {
        Position pos = tempStack.top();
        tempStack.pop();
        std::cout << "(" << pos.x << ", " << pos.y << ") ";
    }
    std::cout << "\n";
}

Action Agent::moveToUnexplored() 
{
    if (!visited.count({x, y - 1}) && direction == SOUTH) 
    {
        moveForward(); 
        return FORWARD;
    } 
    else if (!visited.count({x + 1, y}) && direction == EAST) 
    {
        moveForward();
        return FORWARD;
    } 
    else if (!visited.count({x, y + 1}) && direction == NORTH) 
    {
        moveForward();
        return FORWARD;
    } 
    else if (!visited.count({x - 1, y}) && direction == WEST) 
    {
        moveForward();
        return FORWARD;
    } 
    else 
    {
        turnLeft();
        return LEFT;
    }
}

Action Agent::moveForwardAvoidingWalls() 
{
    if (!hasWallInfront()) 
    {
        //record the current position before moving
        pathStack.push({x, y});

        //move forward based on the facing direction
        switch (direction) 
        {
            case NORTH: y++; break;
            case EAST:  x++; break;
            case SOUTH: y--; break;
            case WEST:  x--; break;
        }

        Position target = {x,y};

        //if visit the same position, set pre-backtrack
        if (isVisitTheSamePosition(target)) 
        {
            setPrebacktrack(target);
        }

        //mark the new position as visited
        visited.insert({x, y});
        std::cout << "Moved to x: " << x << ", y: " << y << std::endl;
        return FORWARD;
    } 
    else
    {
        //if there is a wall, turn to explore another direction
        std::cout << "Hit a wall, turning left.\n";
        turnLeft();
        return LEFT;
    }
}

Action Agent::backtrackToHome() {
    printf("Backtracking...\n");
    std::cout<<"x: "<<x<<"y: "<<y<<std::endl;
    if (x == homeX && y == homeY) 
    {
        printf("Returning home. Shutting off.\n");
        return SHUTOFF;
    }

    if (!pathStack.empty()) 
    {
        //get the next target position from the stack
        Position target = pathStack.top();
        printf("pathStack.top().%i %i\n",target.x,target.y);

        //determine the direction to face toward the target
        if (x < target.x) 
        { 
            if (direction != EAST) 
            {
                turnRight();
                return RIGHT;
            } 
            else 
            {
                //move toward the target, then pop it
                pathStack.pop(); 
                x++;
                return FORWARD;
            }
        } 
        else if (x > target.x) 
        { 
            if (direction != WEST) {
                turnRight();
                return RIGHT;
            } else {
                pathStack.pop();
                x--;
                return FORWARD;
            }
        } 
        else if (y < target.y) 
        { 
            if (direction != NORTH) 
            {
                turnRight();
                return RIGHT;
            } 
            else 
            {
                pathStack.pop();
                y++;
                return FORWARD;
            }
        } 
        else if (y > target.y) 
        { 
            if (direction != SOUTH) 
            {
                turnRight();
                return RIGHT;
            } 
            else 
            {
                pathStack.pop();
                y--;
                return FORWARD;
            }
        }
    }
    printf("pathStack.top(). Empty");
    turnRight();
    return RIGHT;
}

Action Agent::preBacktrack()
{
    printf("Fake Backtracking...\n");
    std::cout<<"x: "<<x<<"y: "<<y<<std::endl;
    Position target = pathStack.top();
    if(target.x != revisitPosition.x && target.y != revisitPosition.y)
    {
        if (!pathStack.empty()) 
        {
            Position target = pathStack.top();
            printf("pathStack.top().%i %i\n",target.x,target.y);

            if (x < target.x) 
            { 
                if (direction != EAST) {
                    turnRight();
                    return RIGHT;
                } else {
                    pathStack.pop(); 
                    x++;
                    return FORWARD;
                }
            } 
            else if (x > target.x)
            {
                if (direction != WEST) {
                    turnRight();
                    return RIGHT;
                } else {
                    pathStack.pop();
                    x--;
                    return FORWARD;
                }
            } 
            else if (y < target.y) 
            {
                if (direction != NORTH) {
                    turnRight();
                    return RIGHT;
                } else {
                    pathStack.pop();
                    y++;
                    return FORWARD;
                }
            } 
            else if (y > target.y) 
            { 
                if (direction != SOUTH) {
                    turnRight();
                    return RIGHT;
                } else {
                    pathStack.pop();
                    y--;
                    return FORWARD;
                }
            }
        }
    }
    else
    {
        isPreBacktracking = false;
        return NOOP;
    }

    printf("pathStack.top(). Empty");
    turnRight();
    return RIGHT;
}

bool Agent::hasUnexploredNeighbor() {
    //check for unexplored cells in all four directions
    return (!visited.count({x, y - 1}) || !visited.count({x + 1, y}) ||
            !visited.count({x, y + 1}) || !visited.count({x - 1, y}));
}

bool Agent::hasWallInfront()
{
    Position nextPosition;
    switch (direction) 
    {
        case NORTH: nextPosition = {x, y + 1}; break;
        case EAST:  nextPosition = {x + 1, y}; break;
        case SOUTH: nextPosition = {x, y - 1}; break;
        case WEST:  nextPosition = {x - 1, y}; break;
    }

    return wallPos.count(nextPosition);
}

bool Agent::shouldBacktrackToHome() 
{
    return visited.size() > 1 && !hasUnexploredNeighbor();
}

bool Agent::isVisitTheSamePosition(Position target)
{
    return (visited.count(target) && !(target.x == 0 && target.y == 0));
}