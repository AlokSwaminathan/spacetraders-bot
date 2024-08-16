#ifndef SPACETRADERS_MODELS_FACTIONS
#define SPACETRADERS_MODELS_FACTIONS

#include "util.h"

struct Faction {
  char* symbol;
  char* name;
  char* desc;
  char* headquarters;
  ARRAY_STRUCT(Trait, traits);
  bool is_recruiting;
};

#endif