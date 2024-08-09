#ifndef SPACETRADERS_RESPONSES_FACTIONS
#define SPACETRADERS_RESPONSES_FACTIONS

#include "../util.h"

typedef struct Faction {
  char* symbol;
  char* name;
  char* desc;
  char* headquarters;
  ARRAY_STRUCT(Trait, traits);
  bool is_recruiting;
} Faction;

#endif