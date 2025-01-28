#include <ctime> //for random seed
#include <cmath> 
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) : x(0), y(0), direction(NORTH), homeX(0), homeY(0), isCleaning(true)  { 
  //supplying your own seed may help debugging, same seed will cause 
  //same random number sequence 
  if (random_seed==0) std::srand( static_cast<unsigned>(std::time(0))); // random seed from time
  else                std::srand( random_seed ); // random seed from user

  pathStack.push({x, y}); // Start at home
  visited.insert({x, y}); // Mark home as visited
} 

Action Agent::GetAction(Percept p) 
{
    if (p.dirt) 
    {
        return SUCK;
    }

    if (p.bump) 
    {
        // If there's a wall in front, turn to explore another direction
        onHitWall();
        turnLeft();
        return LEFT;
    }

    if (hasUnexploredNeighbor()) 
    {
        // Move to an unexplored neighbor
        printf("Exploring unexplored neighbor.\n");
        return moveToUnexplored();
    } 
    else if(!hasWallInfront())
    {
        return moveForwardAvoidingWalls();
    } 
    else
    {
        // If no unexplored neighbors exist, start backtracking
        printf("No unexplored neighbors. Switching to backtracking mode.\n");
        return backtrackToHome();
    }
}
void Agent::moveForward() {
    // Only update position if there is no bump
    std::cout<< "After: x: " << x << "y: "<<y <<std::endl;
    // Record the current position before moving
    pathStack.push({x, y});

    // Move forward based on the current direction
    switch (direction) {
        case NORTH: y++; break;
        case EAST:  x++; break;
        case SOUTH: y--; break;
        case WEST:  x--; break;
    }

    // Mark the new position as visited
    visited.insert({x, y});
    std::cout<< "After: x: " << x << "y: "<<y <<std::endl;
}

void Agent::onHitWall()
{
    wallPos.insert({x, y});
    switch (direction) {
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
    // If the agent is back at home, shut off
    if (x == homeX && y == homeY) 
    {
        printf("Returning home. Shutting off.\n");
        return SHUTOFF;
    }

    if (!pathStack.empty()) 
    {
        // Get the next target position from the stack
        Position target = pathStack.top();
        printf("pathStack.top().%i %i\n",target.x,target.y);

        // Determine the direction to face toward the target
        if (x < target.x) { // Target is to the EAST
            if (direction != EAST) {
                turnRight();
                return RIGHT;
            } else {
                pathStack.pop(); // Move toward the target, then pop it
                x++;
                return FORWARD;
            }
        } else if (x > target.x) { // Target is to the WEST
            if (direction != WEST) {
                turnRight();
                return RIGHT;
            } else {
                pathStack.pop();
                x--;
                return FORWARD;
            }
        } else if (y < target.y) { // Target is to the NORTH
            if (direction != NORTH) {
                turnRight();
                return RIGHT;
            } else {
                pathStack.pop();
                y++;
                return FORWARD;
            }
        } else if (y > target.y) { // Target is to the SOUTH
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

    turnRight();
    return RIGHT;
}


void  Agent::turnTo(Heading targetDirection) {
  while (direction != targetDirection) {
    direction = static_cast<Heading>((direction + 1) % 4); // Turn right until aligned
  }
}

bool Agent::hasUnexploredNeighbor() {
    // Check for unexplored cells in all four directions
    return (!visited.count({x, y - 1}) || !visited.count({x + 1, y}) ||
            !visited.count({x, y + 1}) || !visited.count({x - 1, y}));
}

Action Agent::moveToUnexplored() {
    std::cout<<"direction: " << direction <<std::endl;
    // Check and move to an unexplored direction
    if (!visited.count({x, y - 1}) && direction == SOUTH) {

        moveForward(); // No bump in this case
        return FORWARD;
    } else if (!visited.count({x + 1, y}) && direction == EAST) {
        moveForward();
        return FORWARD;
    } else if (!visited.count({x, y + 1}) && direction == NORTH) {
        moveForward();
        return FORWARD;
    } else if (!visited.count({x - 1, y}) && direction == WEST) {
        moveForward();
        return FORWARD;
    } else {
        // to find an unexplored direction
        turnLeft();
        return LEFT;
    }
}

bool Agent::hasWallInfront()
{
    Position nextPosition;
    switch (direction) {
        case NORTH: nextPosition = {x, y + 1}; break;
        case EAST:  nextPosition = {x + 1, y}; break;
        case SOUTH: nextPosition = {x, y - 1}; break;
        case WEST:  nextPosition = {x - 1, y}; break;
    }

    return wallPos.count(nextPosition);
}

Action Agent::moveForwardAvoidingWalls() {
    // If the next position is not a wall, move forward
    if (!hasWallInfront()) {
        // Record the current position before moving
        pathStack.push({x, y});

        // Move forward based on the current direction
        switch (direction) {
            case NORTH: y++; break;
            case EAST:  x++; break;
            case SOUTH: y--; break;
            case WEST:  x--; break;
        }

        // Mark the new position as visited
        visited.insert({x, y});
        std::cout << "Moved to x: " << x << ", y: " << y << std::endl;
        return FORWARD;
    } else {
        // If there's a wall, turn to explore another direction
        std::cout << "Hit a wall, turning left.\n";
        turnLeft();
        return LEFT;
    }
}

Action Agent::navigateToHome() {
    // Check if already at home
    if (x == homeX && y == homeY) {
        printf("Already at home. Shutting off.\n");
        return SHUTOFF;
    }

    // Move towards home while avoiding walls
    Position nextPosition;

    if (x < homeX && !wallPos.count({x + 1, y})) { // Move EAST
        while (direction != EAST) turnRight();
        nextPosition = {x + 1, y};
    } else if (x > homeX && !wallPos.count({x - 1, y})) { // Move WEST
        while (direction != WEST) turnRight();
        nextPosition = {x - 1, y};
    } else if (y < homeY && !wallPos.count({x, y + 1})) { // Move NORTH
        while (direction != NORTH) turnRight();
        nextPosition = {x, y + 1};
    } else if (y > homeY && !wallPos.count({x, y - 1})) { // Move SOUTH
        while (direction != SOUTH) turnRight();
        nextPosition = {x, y - 1};
    } else {
        // If no clear path, turn left to explore
        turnLeft();
        printf("No clear path, turning left to explore.\n");
        return LEFT;
    }

    // Move to the next position and mark as visited
    x = nextPosition.x;
    y = nextPosition.y;
    visited.insert(nextPosition);
    printf("Moved to x: %d, y: %d\n", x, y);
    return FORWARD;
}
