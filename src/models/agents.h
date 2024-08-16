#ifndef SPACETRADERS_MODELS_AGENTS
#define SPACETRADERS_MODELS_AGENTS

#include "contracts.h"
#include "factions.h"
#include "ships.h"
#include "util.h"

struct Agent {
  char* account_id;
  char* symbol;
  char* headquarters;
  long long credits;
  char* starting_faction;
  int ship_count;
};

struct NewAgentData {
  struct Agent agent;
  struct Contract starting_contract;
  struct Faction faction;
  struct Ship ship;
  char* token;
};

#endif