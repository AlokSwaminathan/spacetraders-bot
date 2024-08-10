#ifndef SPACETRADERS_RESPONSES_AGENTS
#define SPACETRADERS_RESPONSES_AGENTS

#include "models.h"

typedef struct Agent{
  char* account_id;
  char* symbol;
  char* headquarters;
  long long credits;
  char* starting_faction;
  int ship_count;
} Agent;

#endif