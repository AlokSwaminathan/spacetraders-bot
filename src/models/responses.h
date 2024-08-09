#ifndef SPACETRADERS_RESPONSES
#define SPACETRADERS_RESPONSES

#include <time.h>

#include "agents.h"
#include "contracts.h"
#include "factions.h"
#include "ships.h"
#include "market.h"
#include "locations.h"
#include "other.h"

#include "../util.h"

typedef struct NewAgentData{
  Agent agent;
  Contract starting_contract;
  Faction faction;
  Ship ship;
  char* token;
} NewAgentData;

#endif