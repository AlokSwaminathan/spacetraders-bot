#include "ships.h"

#include "../util.h"
#include "util.h"

bool parse_ship_cargo_item(struct JsonNode *root, struct ShipCargoItem *item) {
  JSON_OBJECT_GET_SET(root, "symbol", item->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", item->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", item->desc, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "units", item->units, JSON_TYPE_LONG_LONG);

  return true;
}

void free_ship_cargo_item(struct ShipCargoItem *item) {
  free(item->symbol);
  free(item->name);
  free(item->desc);
}

bool parse_ship_fuel(struct JsonNode *root, struct ShipFuel *fuel) {
  JSON_OBJECT_GET_SET(root, "current", fuel->curr, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "capacity", fuel->capacity, JSON_TYPE_LONG_LONG);

  struct JsonNode *consumed;
  JSON_OBJECT_GET_SET_NOT_REQ(root, "consumed", consumed, JSON_TYPE_OBJECT, NULL);
  if (consumed != NULL) {
    fuel->consumed = true;

    JSON_OBJECT_GET_SET(consumed, "amount", fuel->consumed_amt, JSON_TYPE_LONG_LONG);

    char *timestamp_str;
    JSON_OBJECT_GET_SET(consumed, "timestamp", timestamp_str, JSON_TYPE_STRING);

    fuel->consumption_time = parse_date_time_str(timestamp_str);
    CHECK_TIME_T(fuel->consumption_time);
  } else {
    fuel->consumed = false;
  }

  return true;
}

void free_ship_fuel(struct ShipFuel *fuel __attribute__((unused))) {}

bool parse_ship_cargo(struct JsonNode *root, struct ShipCargo *cargo) {
  JSON_OBJECT_GET_SET(root, "capacity", cargo->capacity, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "units", cargo->units, JSON_TYPE_LONG_LONG);

  struct JsonNode *inventory;
  JSON_OBJECT_GET_SET(root, "inventory", inventory, JSON_TYPE_ARRAY);
  JSON_ARRAY_MAP(inventory, cargo->inventory, {
    if (!parse_ship_cargo_item(curr, &(cargo->inventory.start[i]))) return false;
  });

  return true;
}

void free_ship_cargo(struct ShipCargo *cargo) {
  ARRAY_STRUCT_FREE(cargo->inventory, {
    free_ship_cargo_item(&(cargo->inventory.start[i]));
  });
}

bool parse_ship_install_req(struct JsonNode *root, struct ShipInstallReq *req) {
  JSON_OBJECT_GET_SET_NOT_REQ(root, "power", req->power, JSON_TYPE_LONG_LONG, 0);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "crew", req->crew, JSON_TYPE_LONG_LONG, 0);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "slots", req->slots, JSON_TYPE_LONG_LONG, 0);

  return true;
}

void free_ship_install_req(struct ShipInstallReq *req __attribute__((unused))) {}

bool parse_ship_module(struct JsonNode *root, struct ShipModule *module) {
  JSON_OBJECT_GET_SET(root, "symbol", module->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "capacity", module->capacity, JSON_TYPE_LONG_LONG, -1);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "range", module->range, JSON_TYPE_LONG_LONG, -1);

  JSON_OBJECT_GET_SET(root, "name", module->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", module->desc, JSON_TYPE_STRING);

  struct JsonNode *reqs;
  JSON_OBJECT_GET_SET(root, "requirements", reqs, JSON_TYPE_OBJECT);
  if (!parse_ship_install_req(reqs, &(module->reqs))) return false;

  return true;
}

void free_ship_module(struct ShipModule *module) {
  free(module->symbol);
  free(module->name);
  free(module->desc);
  free_ship_install_req(&(module->reqs));
}

bool parse_ship_mount(struct JsonNode *root, struct ShipMount *mount) {
  JSON_OBJECT_GET_SET(root, "symbol", mount->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", mount->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "description", mount->desc, JSON_TYPE_STRING, NULL);

  JSON_OBJECT_GET_SET_NOT_REQ(root, "strength", mount->strength, JSON_TYPE_LONG_LONG, -1);

  struct JsonNode *deposits;
  JSON_OBJECT_GET_SET_NOT_REQ(root, "deposits", deposits, JSON_TYPE_ARRAY, NULL);
  if (deposits == NULL) {
    mount->deposits.start = NULL;
    mount->deposits.len = 0;
  } else {
    JSON_ARRAY_MAP(deposits, mount->deposits, {
      if (curr->type != JSON_TYPE_STRING) return false;
      char *deposit = strdup(curr->value_str);
      MALLOC_CHECK(deposit);
      mount->deposits.start[i].good = deposit;
    });
  }

  struct JsonNode *reqs;
  JSON_OBJECT_GET_SET(root, "requirements", reqs, JSON_TYPE_OBJECT);
  if (!parse_ship_install_req(reqs, &(mount->reqs))) return false;

  return true;
}

void free_ship_mount(struct ShipMount *mount) {
  free(mount->symbol);
  free(mount->name);
  free(mount->desc);

  ARRAY_STRUCT_FREE(mount->deposits, {
    free(mount->deposits.start[i].good);
  });

  free_ship_install_req(&(mount->reqs));
}

bool parse_ship_cooldown(struct JsonNode *root, struct ShipCooldown *cooldown) {
  JSON_OBJECT_GET_SET(root, "shipSymbol", cooldown->ship_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "totalSeconds", cooldown->total_sec, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "remainingSeconds", cooldown->remaining_sec, JSON_TYPE_LONG_LONG);

  char *expiration_str = NULL;
  JSON_OBJECT_GET_SET_NOT_REQ(root, "expiration", expiration_str, JSON_TYPE_STRING, NULL);

  if (expiration_str == NULL) {
    time(&(cooldown->expiration));
    cooldown->expiration += cooldown->remaining_sec;
  } else {
    cooldown->expiration = parse_date_time_str(expiration_str);
    CHECK_TIME_T(cooldown->expiration);
  }

  return true;
}

void free_ship_cooldown(struct ShipCooldown *cooldown) {
  free(cooldown->ship_symbol);
}

bool parse_ship_engine(struct JsonNode *root, struct ShipEngine *engine) {
  JSON_OBJECT_GET_SET(root, "symbol", engine->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", engine->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", engine->desc, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "condition", engine->condition, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "integrity", engine->integrity, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "speed", engine->speed, JSON_TYPE_LONG_LONG);

  struct JsonNode *reqs;
  JSON_OBJECT_GET_SET(root, "requirements", reqs, JSON_TYPE_OBJECT);
  if (!parse_ship_install_req(reqs, &(engine->reqs))) return false;

  return true;
}

void free_ship_engine(struct ShipEngine *engine) {
  free(engine->symbol);
  free(engine->name);
  free(engine->desc);
  free_ship_install_req(&(engine->reqs));
}

bool parse_ship_reactor(struct JsonNode *root, struct ShipReactor *reactor) {
  JSON_OBJECT_GET_SET(root, "symbol", reactor->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", reactor->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", reactor->desc, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "condition", reactor->condition, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "integrity", reactor->integrity, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "powerOutput", reactor->power_output, JSON_TYPE_LONG_LONG);

  struct JsonNode *reqs;
  JSON_OBJECT_GET_SET(root, "requirements", reqs, JSON_TYPE_OBJECT);
  if (!parse_ship_install_req(reqs, &(reactor->reqs))) return false;

  return true;
}

void free_ship_reactor(struct ShipReactor *reactor) {
  free(reactor->symbol);
  free(reactor->name);
  free(reactor->desc);
  free_ship_install_req(&(reactor->reqs));
}

bool parse_ship_frame(struct JsonNode *root, struct ShipFrame *frame) {
  JSON_OBJECT_GET_SET(root, "symbol", frame->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", frame->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", frame->desc, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "condition", frame->condition, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "integrity", frame->integrity, JSON_TYPE_DOUBLE);

  JSON_OBJECT_GET_SET(root, "moduleSlots", frame->module_slots, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "mountingPoints", frame->mounting_points, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "fuelCapacity", frame->fuel_capacity, JSON_TYPE_LONG_LONG);

  struct JsonNode *reqs;
  JSON_OBJECT_GET_SET(root, "requirements", reqs, JSON_TYPE_OBJECT);
  if (!parse_ship_install_req(reqs, &(frame->reqs))) return false;

  return true;
}

void free_ship_frame(struct ShipFrame *frame) {
  free(frame->symbol);
  free(frame->name);
  free(frame->desc);
  free_ship_install_req(&(frame->reqs));
}

bool parse_ship_crew(struct JsonNode *root, struct ShipCrew *crew) {
  JSON_OBJECT_GET_SET(root, "current", crew->current, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "required", crew->required, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "capacity", crew->capacity, JSON_TYPE_LONG_LONG);

  char *rotation_str;
  JSON_OBJECT_GET_SET(root, "rotation", rotation_str, JSON_TYPE_STRING);

  if (strcmp(rotation_str, SHIP_CREW_ROTATION_RELAXED_STR)) {
    crew->rotation = SHIP_CREW_ROTATION_RELAXED;
  } else if (strcmp(rotation_str, SHIP_CREW_ROTATION_STRICT_STR)) {
    crew->rotation = SHIP_CREW_ROTATION_STRICT;
  }
  free(rotation_str);

  JSON_OBJECT_GET_SET(root, "morale", crew->morale, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "wages", crew->wages, JSON_TYPE_LONG_LONG);

  return true;
}

void free_ship_crew(struct ShipCrew *crew __attribute__((unused))) {}

bool parse_ship_nav_route_waypoint(struct JsonNode *root, struct ShipNavRouteWaypoint *waypoint) {
  JSON_OBJECT_GET_SET(root, "symbol", waypoint->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "type", waypoint->type, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "systemSymbol", waypoint->system_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "x", waypoint->x, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "y", waypoint->y, JSON_TYPE_LONG_LONG);

  return true;
}

void free_ship_nav_route_waypoint(struct ShipNavRouteWaypoint *waypoint) {
  free(waypoint->symbol);
  free(waypoint->type);
  free(waypoint->system_symbol);
}

bool parse_ship_nav_route(struct JsonNode *root, struct ShipNavRoute *route) {
  struct JsonNode *origin;
  JSON_OBJECT_GET_SET(root, "origin", origin, JSON_TYPE_OBJECT);
  if (!parse_ship_nav_route_waypoint(origin, &(route->origin))) return false;

  struct JsonNode *dest;
  JSON_OBJECT_GET_SET(root, "destination", dest, JSON_TYPE_OBJECT);
  if (!parse_ship_nav_route_waypoint(dest, &(route->destination))) return false;

  char *departure_str;
  JSON_OBJECT_GET_SET(root, "departureTime", departure_str, JSON_TYPE_STRING);
  route->departure = parse_date_time_str(departure_str);
  CHECK_TIME_T(route->departure);

  char *arrival;
  JSON_OBJECT_GET_SET(root, "arrival", arrival, JSON_TYPE_STRING);
  route->arrival = parse_date_time_str(arrival);
  CHECK_TIME_T(route->arrival);

  return true;
}

void free_ship_nav_route(struct ShipNavRoute *route) {
  free_ship_nav_route_waypoint(&(route->origin));
  free_ship_nav_route_waypoint(&(route->destination));
}

bool parse_ship_nav(struct JsonNode *root, struct ShipNavigation *nav) {
  JSON_OBJECT_GET_SET(root, "systemSymbol", nav->system_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "waypointSymbol", nav->waypoint_symbol, JSON_TYPE_STRING);

  struct JsonNode *route;
  JSON_OBJECT_GET_SET(root, "route", route, JSON_TYPE_OBJECT);
  if (!parse_ship_nav_route(route, &(nav->route))) return false;

  char *status_str;
  JSON_OBJECT_GET_SET(root, "status", status_str, JSON_TYPE_STRING);

  if (strcmp(status_str, SHIP_STATUS_IN_TRANSIT_STR)) {
    nav->status = SHIP_STATUS_IN_TRANSIT;
  } else if (strcmp(status_str, SHIP_STATUS_IN_ORBIT_STR)) {
    nav->status = SHIP_STATUS_IN_ORBIT;
  } else if (strcmp(status_str, SHIP_STATUS_DOCKED_STR)) {
    nav->status = SHIP_STATUS_DOCKED;
  } else {
    free(status_str);
    return false;
  }
  free(status_str);

  char *flight_mode_str;
  JSON_OBJECT_GET_SET(root, "flightMode", flight_mode_str, JSON_TYPE_STRING);

  if (strcmp(flight_mode_str, SHIP_FLIGHT_MODE_CRUISE_STR)) {
    nav->flight_mode = SHIP_FLIGHT_MODE_CRUISE;
  } else if (strcmp(flight_mode_str, SHIP_FLIGHT_MODE_BURN_STR)) {
    nav->flight_mode = SHIP_FLIGHT_MODE_BURN;
  } else if (strcmp(flight_mode_str, SHIP_FLIGHT_MODE_DRIFT_STR)) {
    nav->flight_mode = SHIP_FLIGHT_MODE_DRIFT;
  } else if (strcmp(flight_mode_str, SHIP_FLIGHT_MODE_STEALTH_STR)) {
    nav->flight_mode = SHIP_FLIGHT_MODE_STEALTH;
  } else {
    free(flight_mode_str);
    return false;
  }
  free(flight_mode_str);

  return true;
}

void free_ship_nav(struct ShipNavigation *nav) {
  free(nav->system_symbol);
  free(nav->waypoint_symbol);
  free_ship_nav_route(&(nav->route));
}

bool parse_ship_registration(struct JsonNode *root, struct ShipRegistration *registration) {
  JSON_OBJECT_GET_SET(root, "name", registration->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "factionSymbol", registration->faction_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "role", registration->role, JSON_TYPE_STRING);

  return true;
}

void free_ship_registration(struct ShipRegistration *reg) {
  free(reg->name);
  free(reg->faction_symbol);
  free(reg->role);
}

bool parse_ship(struct JsonNode *root, struct Ship *ship) {
  JSON_OBJECT_GET_SET(root, "symbol", ship->symbol, JSON_TYPE_STRING);

  struct JsonNode *registration;
  JSON_OBJECT_GET_SET(root, "registration", registration, JSON_TYPE_OBJECT);
  if (!parse_ship_registration(registration, &(ship->registration))) return false;

  struct JsonNode *nav;
  JSON_OBJECT_GET_SET(root, "nav", nav, JSON_TYPE_OBJECT);
  if (!parse_ship_nav(nav, &(ship->nav))) return false;

  struct JsonNode *crew;
  JSON_OBJECT_GET_SET(root, "crew", crew, JSON_TYPE_OBJECT);
  if (!parse_ship_crew(crew, &(ship->crew))) return false;

  struct JsonNode *frame;
  JSON_OBJECT_GET_SET(root, "frame", frame, JSON_TYPE_OBJECT);
  if (!parse_ship_frame(frame, &(ship->frame))) return false;

  struct JsonNode *reactor;
  JSON_OBJECT_GET_SET(root, "reactor", reactor, JSON_TYPE_OBJECT);
  if (!parse_ship_reactor(reactor, &(ship->reactor))) return false;

  struct JsonNode *engine;
  JSON_OBJECT_GET_SET(root, "engine", engine, JSON_TYPE_OBJECT);
  if (!parse_ship_engine(engine, &(ship->engine))) return false;

  struct JsonNode *cooldown;
  JSON_OBJECT_GET_SET(root, "cooldown", cooldown, JSON_TYPE_OBJECT);
  if (!parse_ship_cooldown(cooldown, &(ship->cooldown))) return false;

  struct JsonNode *modules;
  JSON_OBJECT_GET_SET(root, "modules", modules, JSON_TYPE_ARRAY);
  JSON_ARRAY_MAP(modules, ship->modules, {
    if (!parse_ship_module(curr, &(ship->modules.start[i]))) return false;
  });

  struct JsonNode *mounts;
  JSON_OBJECT_GET_SET(root, "mounts", mounts, JSON_TYPE_ARRAY);
  JSON_ARRAY_MAP(mounts, ship->mounts, {
    if (!parse_ship_mount(curr, &(ship->mounts.start[i]))) return false;
  });

  struct JsonNode *fuel;
  JSON_OBJECT_GET_SET(root, "fuel", fuel, JSON_TYPE_OBJECT);
  if (!parse_ship_fuel(fuel, &(ship->fuel))) return false;

  struct JsonNode *cargo;
  JSON_OBJECT_GET_SET(root, "cargo", cargo, JSON_TYPE_OBJECT);
  if (!parse_ship_cargo(cargo, &(ship->cargo))) return false;

  return true;
}

void free_ship(struct Ship *ship) {
  free(ship->symbol);
  free_ship_registration(&(ship->registration));
  free_ship_nav(&(ship->nav));
  free_ship_crew(&(ship->crew));
  free_ship_frame(&(ship->frame));
  free_ship_reactor(&(ship->reactor));
  free_ship_engine(&(ship->engine));
  free_ship_cooldown(&(ship->cooldown));

  ARRAY_STRUCT_FREE(ship->modules, {
    free_ship_module(&(ship->modules.start[i]));
  });

  ARRAY_STRUCT_FREE(ship->mounts, {
    free_ship_mount(&(ship->mounts.start[i]));
  });

  free_ship_cargo(&(ship->cargo));

  free_ship_fuel(&(ship->fuel));
}