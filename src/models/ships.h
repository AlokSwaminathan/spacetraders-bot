#ifndef SPACETRADERS_MODELS_SHIPS
#define SPACETRADERS_MODELS_SHIPS

#include "other.h"
#include "util.h"

enum ShipStatus {
  SHIP_STATUS_IN_TRANSIT,
  SHIP_STATUS_IN_ORBIT,
  SHIP_STATUS_DOCKED,
};

enum ShipFlightMode {
  SHIP_FLIGHT_MODE_CRUISE,
  SHIP_FLIGHT_MODE_DRIFT,
  SHIP_FLIGHT_MODE_STEALTH,
  SHIP_FLIGHT_MODE_BURN,
};

enum ShipCrewRotation {
  SHIP_CREW_ROTATION_STRICT,
  SHIP_CREW_ROTATION_RELAXED
};

struct ShipCrew {
  int current;
  int required;
  int capacity;
  enum ShipCrewRotation rotation;
  int morale;
  int wages;
};

struct ShipInstallReqs {
  int power;
  int crew;
  int slots;
};

struct ShipFrame {
  char* frame_symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int mounting_points;
  int fuel_capacity;
  struct ShipInstallReqs reqs;
};

struct ShipReactor {
  char* symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int power_output;
  struct ShipInstallReqs reqs;
};

struct ShipEngine {
  char* symbol;
  char* name;
  char* desc;
  double condition;
  double integrity;
  int speed;
  struct ShipInstallReqs reqs;
};

struct ShipCoolDown {
  char* ship_symbol;
  int total_sec;
  int remaining_sec;
  time_t expiration;
};

struct ShipModule {
  char* symbol;
  int capacity;
  int range;
  char* name;
  char* desc;
  struct ShipInstallReqs reqs;
};

struct ShipMount {
  char* symbol;
  char* name;
  char* desc;
  int strength;
  char** deposits;
  size_t deposits_len;
  struct ShipInstallReqs reqs;
};

struct ShipCargoItem {
  char* symbol;
  char* name;
  char* desc;
  int units;
};

struct ShipCargo {
  int capacity;
  int units;
  ARRAY_STRUCT(ShipCargoItem, inventory);
};

struct ShipFuel {
  int curr;
  int capacity;
  bool consumed;
  int consumed_amt;
  time_t consumption_time;
};

struct ShipExtractionYield {
  char* symbol;
  int units;
};

struct ShipExtraction {
  char* ship_symbol;
  struct ShipExtractionYield yield;
};

struct ShipRegistration {
  char* name;
  char* faction_symbol;
  char* role;
};

struct ShipNavRouteWaypoint {
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
};

struct ShipNavRoute {
  struct ShipNavRouteWaypoint origin;
  struct ShipNavRouteWaypoint destination;
  time_t departure;
  time_t arrival;
};

struct ShipNavigation {
  char* system_symbol;
  char* waypoint_symbol;
  struct ShipNavRoute route;
  enum ShipStatus status;
  enum ShipFlightMode flight_mode;
};

struct Ship {
  char* symbol;
  struct ShipRegistration registration;
  struct ShipNavigation nav;
  struct ShipCoolDown cooldown;
  ARRAY_STRUCT(ShipModule, modules);
  ARRAY_STRUCT(ShipMount, mounts);
  struct ShipCargo cargo;
  struct ShipFuel fuel;
};

struct ScannerShip {
  char* symbol;
  struct ShipRegistration registration;
  struct ShipNavigation nav;
  char* frame_symbol;
  char* reactor_symbol;
  char* engine_symbol;
  char** mounts;
  size_t mounts_len;
};

enum ShipComponent {
  SHIP_COMPONENT_FRAME,
  SHIP_COMPONENT_REACTOR,
  SHIP_COMPONENT_ENGINE
};

struct ShipConditionEvent {
  char* symbol;
  enum ShipComponent component;
  char* name;
  char* desc;
};

struct ShipModificationTransaction {
  char* waypoint_symbol;
  char* ship_symbol;
  char* trade_symbol;
  int total_price;
  time_t timestamp;
};

struct ShipyardTransacation {
  char* waypoint_symbol;
  char* ship_symbol;
  char* ship_type;
  int price;
  char* agent_symbol;
  time_t timestamp;
};

struct ShipyardShip {
  char* type;
  char* name;
  char* desc;
  enum SupplyLevel supply;
  enum ActivityLevel activity;
  int purchase_price;
  struct ShipFrame frame;
  struct ShipReactor reactor;
  struct ShipEngine engine;
  ARRAY_STRUCT(ShipModule, modules);
  ARRAY_STRUCT(ShipMount, mounts);
  struct ShipCrew crew;
};

struct Shipyard {
  char* symbol;
  char** ship_types;
  size_t ship_types_len;
  ARRAY_STRUCT(ShipyardTransacation, transactions);
  ARRAY_STRUCT(ShipyardShip, ships);
  int modifications_fee;
};

struct Siphon {
  char* ship_symbol;
  char* yield;
  int units;
};

#endif