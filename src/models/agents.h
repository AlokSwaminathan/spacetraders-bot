#ifndef SPACETRADERS_MODELS_AGENTS
#define SPACETRADERS_MODELS_AGENTS

#include "models.h"

typedef struct Agent {
  char* account_id;
  char* symbol;
  char* headquarters;
  long long credits;
  char* starting_faction;
  int ship_count;
} Agent;

typedef struct NewAgentData{
  Agent agent;
  Contract starting_contract;
  Faction faction;
  Ship ship;
  char* token;
} NewAgentData;

#endif