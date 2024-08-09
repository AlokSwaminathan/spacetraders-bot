#ifndef SPACETRADERS_RESPONSES_FLEETS
#define SPACETRADERS_RESPONSES_FLEETS

#include "responses.h"

typedef enum ShipStatus{
  IN_TRANSIT,
  IN_ORBIT,
  DOCKED,
} ShipStatus;

typedef enum ShipFlightMode{
  CRUISE,
  DRIFT,
  STEALTH,
  CRUISE,
  BURN,
} ShipFlightMode;

typedef enum ShipCrewRotation{
  STRICT,
  RELAXED
} ShipCrewRotation;

typedef struct ShipCrew{
  int current;
  int required;
  int capacity;
  ShipCrewRotation rotation;
  int morale;
  int wages;
} ShipCrew;

typedef struct ShipInstallReqs{
  int power;
  int crew;
  int slots;
} ShipInstallReqs;

typedef struct ShipFrame{
  char* frame_symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int mounting_points;
  int fuel_capacity;
  ShipInstallReqs reqs;
} ShipFrame;

typedef struct ShipReactor{
  char* symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int power_output;
  ShipInstallReqs reqs;
} ShipReactor;

typedef struct ShipEngine{
  char* symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int speed;
  ShipInstallReqs reqs;
} ShipEngine;

typedef struct ShipCoolDown{
  char* ship_symbol;
  int total_sec;
  int remaining_sec;
  time_t expiration;
} ShipCoolDown;

typedef struct ShipModule{
  char* symbol;
  int capacity;
  int range;
  char* name;
  char* desc;
  ShipInstallReqs reqs;
} ShipModule;

typedef struct ShipMount{
  char* symbol;
  char* name;
  char* desc;
  int strength;
  char** deposits;
  size_t deposits_len;
  ShipInstallReqs reqs;
} ShipMount;

typedef struct ShipCargoItem{
  char* symbol;
  char* name;
  char* desc;
  int units;
} ShipCargoItem;

typedef struct ShipCargo{
  int capacity;
  int units;
  ARRAY_STRUCT(ShipCargoItem,inventory);
} ShipCargo;

typedef struct ShipFuel{
  int curr;
  int capacity;
  bool consumed;
  int consumed_amt;
  time_t consumption_time;
} ShipFuel;

typedef struct Ship{
  char* symbol;
  ShipRegistration registration;
  ShipNavigation nav; 
  ShipCoolDown cooldown;
  ARRAY_STRUCT(ShipModule,modules);
  ARRAY_STRUCT(ShipMount,mounts);
  ShipCargo cargo;
  ShipFuel fuel;
} Ship;

typedef struct ShipExtractionYield{
  char* symbol;
  int units;
} ShipExtractionYield;

typedef struct ShipExtraction{
  char* ship_symbol;
  ShipExtractionYield yield;
} ShipExtraction;

typedef struct ShipRegistration{
  char* name;
  char* faction_symbol;
  char* role;
} ShipRegistration;

typedef struct ShipNavRouteWaypoint{
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
} ShipNavRouteWaypoint;

typedef struct ShipNavRoute{
  ShipNavRouteWaypoint origin;
  ShipNavRouteWaypoint destination;
  time_t departure;
  time_t arrival;
} ShipNavRoute;

typedef struct ShipNavigation{
  char* system_symbol;
  char* waypoint_symbol;

  ShipStatus status;
  ShipFlightMode flight_mode;
} ShipNavigation;

typedef struct ScannerShip{
  char* symbol;
  ShipRegistration registration;
  ShipNavigation nav;
  char* frame_symbol;
  char* reactor_symbol;
  char* engine_symbol;
  char** mounts;
  size_t mounts_len;
} ScannerShip;

typedef enum ShipComponent{
  FRAME,
  REACTOR,
  ENGINE
} ShipComponent;

typedef struct ShipConditionEvent{
  char* symbol;
  ShipComponent component;
  char* name;
  char* desc;
} ShipConditionEvent;

typedef struct ShipModificationTransaction{
  char* waypoint_symbol;
  char* ship_symbol;
  char* trade_symbol;
  int total_price;
  time_t timestamp;
} ShipModificationTransaction;

typedef struct ShipyardTransacation{
  char* waypoint_symbol;
  char* ship_symbol;
  char* ship_type;
  int price;
  char* agent_symbol;
  time_t timestamp;
} ShipyardTransacation;

typedef struct ShipyardShip{
  char* type;
  char* name;
  char* desc;
  SupplyLevel supply;
  ActivityLevel activity;
  int purchase_price;
  ShipFrame frame;
  ShipReactor reactor;
  ShipEngine engine;
  ARRAY_STRUCT(ShipModule,modules);
  ARRAY_STRUCT(ShipMount,mounts);
  ShipCrew crew;
} ShipyardShip;

typedef struct Shipyard{
  char* symbol;
  char** ship_types;
  size_t ship_types_len;
  ARRAY_STRUCT(ShipyardTransacation,transactions);
  ARRAY_STRUCT(ShipyardShip,ships);
  int modifications_fee;
} Shipyard;

typedef struct Siphon{
  char* ship_symbol;
  char* yield;
  int units;
} Siphon;

#endif