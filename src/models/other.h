#ifndef SPACETRADERS_MODELS_OTHER
#define SPACETRADERS_MODELS_OTHER

#include "json.h"
#include "util.h"

typedef enum ActivityLevel {
  ACTIVITY_LEVEL_WEAK,
  ACTIVITY_LEVEL_GROWING,
  ACTIVITY_LEVEL_STRONG,
  ACTIVITY_LEVEL_RESTRICTED,
} ActivityLevel;

typedef enum SupplyLevel {
  SUPPLY_LEVEL_SCARCE,
  SUPPLY_LEVEL_LIMITED,
  SUPPLY_LEVEL_MODERATE,
  SUPPLY_LEVEL_HIGH,
  SUPPLY_LEVEL_ABUNDANT,
} SupplyLevel;

typedef struct ConstructionMaterials {
  char* trade_symbol;
  int required;
  int fulfilled;
} ConstructionMaterials;

typedef struct Trait {
  char* symbol;
  char* name;
  char* description;
} Trait;

typedef struct Pagination {
  int total;
  int page;
  int limit;
} Pagination;

typedef struct GameStatusStats {
  int agents;
  int ships;
  int systems;
  int waypoints;
} GameStatusStats;

typedef struct GameStatus {
  JsonNode* root;
  char* status;
  char* ver;
  char* reset_date;
  char* description;
  GameStatusStats stats;
  ARRAY_STRUCT({char* agent_symbol; long long credits; }, most_credits);
  ARRAY_STRUCT({char* agent_symbol; int chart_count; }, most_charts);
  char* next_reset;
  char* reset_freq;
  ARRAY_STRUCT({char* title; char* body; }, announcements);
  ARRAY_STRUCT({char* name; char* url; }, links);
} GameStatus;

#endif