#ifndef SPACETRADERS_RESPONSES_LOCATIONS
#define SPACETRADERS_RESPONSES_LOCATIONS

#include "responses.h"

typedef struct Chart {
  char* waypoint_symbol;
  char* submitted_by;
  time_t submitted_on;
} Chart;

typedef struct ConnectedSystem {
  char* symbol;
  char* sector_symbol;
  char* type;
  char* faction_symbol;
  int x;
  int y;
  int dist;
} ConnectedSystem;


typedef struct WaypointConstruction{
  char* symbol;
  ARRAY_STRUCT(ConstructionMaterials, materials);
  bool is_complete;
} WaypointConstruction;

typedef struct JumpGate{
  char* waypoint_symbol;
  char** connections;
  size_t connections_len;
} JumpGate;

typedef struct ScannedSystem{
  char* symbol;
  char* sector_symbol;
  char* type;
  int x;
  int y;
  int dist;
} ScannedSystem;

typedef struct ScannedWaypoint{
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
  char** orbitals;
  size_t orbital_len;
  char* faction_symbol;
  ARRAY_STRUCT(Trait,traits);
  Chart chart;
} ScannedWaypoint;

typedef enum DepositSize{
  SMALL,
  MODERATE,
  LARGE,
} DepositSize;

typedef struct Survey{
  char* signature;
  char* symbol;
  char** deposits;
  size_t deposits_len;
  time_t expiration;
  DepositSize size;
} Survey;

typedef struct SystemWaypoint{
  char* symbol;
  char* type;
  int x;
  int y;
  char** orbitals;
  size_t orbitals_len;
  char* orbits;
} SystemWaypoint;

typedef struct System{
  char* symbol;
  char* sector_symbol;
  char* type;
  int x;
  int y;
  ARRAY_STRUCT(SystemWaypoint,waypoints);
  char** factions;
  size_t factions_len;
} System;


#endif