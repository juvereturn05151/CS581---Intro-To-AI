#include <ctime> //for random seed
#include <cmath> 
#include "agent.h"
#include <iostream>

Agent::Agent(int random_seed) { 
  //supplying your own seed may help debugging, same seed will cause 
  //same random number sequence 
  if (random_seed==0) std::srand( static_cast<unsigned>(std::time(0))); // random seed from time
  else                std::srand( random_seed ); // random seed from user

  isFirstMove = true;
  lastBumpCorner = NOOP;
} 

Action Agent::GetAction(Percept p) 
{
  //std::cout << "lastBumpCorner: " << lastBumpCorner<< std::endl;
  std::cout << "wasBumped: " << wasBumped<< std::endl;
    std::cout << " p.bump: " <<  p.bump<< std::endl;
  //straight-forward beahavior
  if (p.dirt)
  {
    return SUCK;
  } 

  if(wasBumped && p.bump && !p.home)
  {
    return SHUTOFF;
  }

  if(hasMoveForwardAfterBump)
  {
    wasBumped = false;
    hasMoveForwardAfterBump = false;
    if(lastBumpCorner == RIGHT)
    {
      return RIGHT;
    }
    else if (lastBumpCorner == LEFT)
    {
      return LEFT;
    }
  }

  if (p.bump) 
  {
    wasBumped  = true;

    if(lastBumpCorner == LEFT)
    {
      lastBumpCorner = RIGHT;
      return RIGHT;
    }
    else if(lastBumpCorner == RIGHT)
    {
     lastBumpCorner = LEFT;
     return LEFT;
    }

    lastBumpCorner = RIGHT;
    return RIGHT;
  }

  if (p.home)
  {
    if(isFirstMove)
    {
      isFirstMove = false;
      return FORWARD;
    }
  }

  if(wasBumped)
  {
    hasMoveForwardAfterBump = true;
  }

  return FORWARD;
}

