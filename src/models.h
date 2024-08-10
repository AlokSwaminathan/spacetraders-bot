#ifndef SPACETRADERS_MODELS
#define SPACETRADERS_MODELS

#include <time.h>

#include "models/agents.h"
#include "models/contracts.h"
#include "models/factions.h"
#include "models/ships.h"
#include "models/market.h"
#include "models/locations.h"
#include "models/other.h"

#include "util.h"

typedef struct NewAgentData{
  Agent agent;
  Contract starting_contract;
  Faction faction;
  Ship ship;
  char* token;
} NewAgentData;

#endif