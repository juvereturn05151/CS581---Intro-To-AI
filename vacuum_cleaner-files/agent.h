#ifndef AGENT_H
#define AGENT_H
#include "definitions.h"


class Agent {
  public:
    Agent(int random_seed);
    Action GetAction(Percept p);
  private: 
    bool isFirstMove;
    bool wasBumped;
    bool hasMoveForwardAfterBump;
    Action lastBumpCorner;

    Action handleDirt(const Percept& p) const;
    Action handleHome(const Percept& p);
    Action handleBump(const Percept& p);
    Action handleDefaultMove();
};

#endif
