#ifndef SPACETRADERS_MODELS_OTHER
#define SPACETRADERS_MODELS_OTHER

#include "models.h"

typedef enum ActivityLevel {
  WEAK,
  GROWING,
  STRONG,
  RESTRICTED,
} ActivityLevel;

typedef enum SupplyLevel {
  SCARCE,
  LIMITED,
  MODERATE,
  HIGH,
  ABUNDANT,
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
typedef struct GameStatus {
  char* status;
  char* ver;
  char* reset_date;
  char* description;
  struct {
    int agents;
    int ships;
    int systems;
    int waypoints;
  } stats;
  ARRAY_STRUCT({char* agent_symbol; long long credits; }, most_credits);
  ARRAY_STRUCT({char* agent_symbol; int chart_count; }, most_submitted_charts);
  char* next_reset;
  char* reset_freq;
  ARRAY_STRUCT({char* title; char* body; }, announcements);
  ARRAY_STRUCT({char* name; char* url; }, links);
} GameStatus;

#endif