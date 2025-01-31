#include <ctime> //for random seed
#include <cmath> 
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : x(0), y(0), direction(NORTH), homeX(0), homeY(0)  { 
  //supplying your own seed may help debugging, same seed will cause 
  //same random number sequence 
  if (random_seed==0) std::srand( static_cast<unsigned>(std::time(0))); // random seed from time
  else                std::srand( random_seed ); // random seed from user

  startFakeBacktracking = false;
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
        //if there's a wall in front, turn to explore another direction
        onHitWall();
        turnLeft();
        return LEFT;
    }

    if (shouldBacktrackToHome()) {
        printf("All paths explored. Backtracking to home.");
        return backtrackToHome();
    }

    if(startFakeBacktracking)
    {
        printf("startFakeBacktracking x:");
        std::cout<<revisitPosition.x<<" y:"<<revisitPosition.y<<std::endl;
        return fakeBacktrack();
    }

    if (hasUnexploredNeighbor()) 
    {
        // Move to an unexplored neighbor
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
        // If no unexplored neighbors exist, start backtracking
        printf("No unexplored neighbors. Switching to backtracking mode.\n");
        return backtrackToHome();
    }
}

bool Agent::shouldBacktrackToHome() 
{
    return visited.size() > 1 && !hasUnexploredNeighbor();
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
    // Fake backtracking: If revisiting a position we've been to, we simulate backtracking
    if ((visited.count(target) && !(target.x == 0 && target.y == 0))) 
    {
        printf("Revisiting position: (%d, %d). Simulating backtracking.\n", target.x, target.y);
        startFakeBacktracking = true;
        revisitPosition = target;
        pathStack.pop();
    }

    //mark the new position as visited
    visited.insert({x, y});
    std::cout<< "After: x: " << x << "y: "<<y <<std::endl;
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

Action Agent::fakeBacktrack()
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
        startFakeBacktracking = false;
        return NOOP;
    }

    printf("pathStack.top(). Empty");
    turnRight();
    return RIGHT;
}

bool Agent::hasUnexploredNeighbor() {
    // Check for unexplored cells in all four directions
    return (!visited.count({x, y - 1}) || !visited.count({x + 1, y}) ||
            !visited.count({x, y + 1}) || !visited.count({x - 1, y}));
}

Action Agent::moveToUnexplored() {
    std::cout<<"direction: " << direction <<std::endl;
    // Check and move to an unexplored direction
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

        // Fake backtracking: If revisiting a position we've been to, we simulate backtracking
        if ((visited.count(target) && !(target.x == 0 && target.y == 0))) 
        {
            printf("Revisiting position: (%d, %d). Simulating backtracking.\n", target.x, target.y);
            startFakeBacktracking = true;
            revisitPosition = target;
            pathStack.pop();
        }

        //mark the new position as visited
        visited.insert({x, y});
        std::cout << "Moved to x: " << x << ", y: " << y << std::endl;
        return FORWARD;
    } 
    else
    {
        // If there's a wall, turn to explore another direction
        std::cout << "Hit a wall, turning left.\n";
        turnLeft();
        return LEFT;
    }
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

