#ifndef SPACETRADERS_MODELS_LOCATIONS
#define SPACETRADERS_MODELS_LOCATIONS

#include "util.h"

struct Chart {
  char* waypoint_symbol;
  char* submitted_by;
  time_t submitted_on;
};

struct ConnectedSystem {
  char* symbol;
  char* sector_symbol;
  char* type;
  char* faction_symbol;
  int x;
  int y;
  int dist;
};

struct WaypointConstruction {
  char* symbol;
  ARRAY_STRUCT(ConstructionMaterials, materials);
  bool is_complete;
};

struct JumpGate {
  char* waypoint_symbol;
  char** connections;
  size_t connections_len;
};

struct ScannedSystem {
  char* symbol;
  char* sector_symbol;
  char* type;
  int x;
  int y;
  int dist;
};

struct ScannedWaypoint {
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
  char** orbitals;
  size_t orbital_len;
  char* faction_symbol;
  ARRAY_STRUCT(Trait, traits);
  struct Chart chart;
};

enum DepositSize {
  DEP_SIZE_SMALL,
  DEP_SIZE_MODERATE,
  DEP_SIZE_LARGE,
};

struct Survey {
  char* signature;
  char* symbol;
  char** deposits;
  size_t deposits_len;
  time_t expiration;
  enum DepositSize size;
};

struct SystemWaypoint {
  char* symbol;
  char* type;
  int x;
  int y;
  char** orbitals;
  size_t orbitals_len;
  char* orbits;
};

struct System {
  char* symbol;
  char* sector_symbol;
  char* type;
  int x;
  int y;
  ARRAY_STRUCT(SystemWaypoint, waypoints);
  char** factions;
  size_t factions_len;
};

#endif