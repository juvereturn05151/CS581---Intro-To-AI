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

Action Agent::GetAction(Percept p) {
    if (isCleaning) {
        if (p.dirt) {
            return SUCK;
        }

        if (p.bump) {
            // If there's a wall in front, turn to explore another direction
            turnLeft();
            return LEFT;
        }

        if (hasUnexploredNeighbor()) {
            // Move to an unexplored neighbor
            printf("Exploring unexplored neighbor.\n");
            return moveToUnexplored();
        } else {
            // If no unexplored neighbors exist, start backtracking
            printf("No unexplored neighbors. Switching to backtracking mode.\n");
            isCleaning = false; // Switch to backtracking mode
            return backtrackToHome();
        }
    } else {
        // Backtrack and check for unexplored neighbors
        if (hasUnexploredNeighbor()) {
            // Resume exploration if an unexplored neighbor is found
            printf("Found unexplored neighbor while backtracking. Resuming exploration.\n");
            isCleaning = true; // Switch back to cleaning mode
            return moveToUnexplored();
        } else {
            return backtrackToHome();
        }
    }
}
void Agent::moveForward(bool bump) {
    // Only update position if there is no bump
    if (!bump) {
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
    }
}

    void Agent::turnLeft() {
        direction = static_cast<Heading>((direction + 3) % 4);
    }

    void Agent::turnRight() {
        direction = static_cast<Heading>((direction + 1) % 4);
    }

Action Agent::backtrackToHome() {
    printf("Backtracking...\n");

    // If the agent is back at home, shut off
    if (x == homeX && y == homeY) {
        printf("Returning home. Shutting off.\n");
        return SHUTOFF;
    }

    if (!pathStack.empty()) {
        // Get the next target position from the stack
        Position target = pathStack.top();

        // Determine the direction to face toward the target
        if (x < target.x) { // Target is to the EAST
            if (direction != EAST) {
                turnTo(EAST);
                return RIGHT;
            } else {
                pathStack.pop(); // Move toward the target, then pop it
                x++;
                return FORWARD;
            }
        } else if (x > target.x) { // Target is to the WEST
            if (direction != WEST) {
                turnTo(WEST);
                return RIGHT;
            } else {
                pathStack.pop();
                x--;
                return FORWARD;
            }
        } else if (y < target.y) { // Target is to the NORTH
            if (direction != NORTH) {
                turnTo(NORTH);
                return RIGHT;
            } else {
                pathStack.pop();
                y++;
                return FORWARD;
            }
        } else if (y > target.y) { // Target is to the SOUTH
            if (direction != SOUTH) {
                turnTo(SOUTH);
                return RIGHT;
            } else {
                pathStack.pop();
                y--;
                return FORWARD;
            }
        }
    }

    // If for some reason the pathStack is empty, shut off as a fallback
    return SHUTOFF;
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
    // Check and move to an unexplored direction
    if (!visited.count({x, y - 1}) && direction == SOUTH) {
        moveForward(false); // No bump in this case
        return FORWARD;
    } else if (!visited.count({x + 1, y}) && direction == EAST) {
        moveForward(false);
        return FORWARD;
    } else if (!visited.count({x, y + 1}) && direction == NORTH) {
        moveForward(false);
        return FORWARD;
    } else if (!visited.count({x - 1, y}) && direction == WEST) {
        moveForward(false);
        return FORWARD;
    } else {
        turnLeft(); // Rotate to find an unexplored direction
        return LEFT;
    }
}