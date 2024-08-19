#include "agents.h"

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "ships.h"
#include "contracts.h"
#include "factions.h"

bool parse_agent_details(struct JsonNode* root, struct Agent* agent) {
  JSON_OBJECT_GET_SET_NOT_REQ(root, "accountId", agent->account_id, JSON_TYPE_STRING,NULL);

  JSON_OBJECT_GET_SET(root, "symbol", agent->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "headquarters", agent->headquarters, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "credits", agent->credits, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "startingFaction", agent->starting_faction, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "shipCount", agent->ship_count, JSON_TYPE_LONG_LONG);

  return true;
}

void free_agent_details(struct Agent* agent) {
  free(agent->account_id);
  free(agent->symbol);
  free(agent->headquarters);
  free(agent->starting_faction);
}

bool parse_new_agent(struct JsonNode *root, struct NewAgentData *agent){

  JSON_OBJECT_GET_SET(root,"token",agent->token,JSON_TYPE_STRING);

  struct JsonNode *agent_details;
  JSON_OBJECT_GET_SET(root,"agent",agent_details,JSON_TYPE_OBJECT);
  if (!parse_agent_details(agent_details,&(agent->agent))) return false;

  struct JsonNode *contract;
  JSON_OBJECT_GET_SET(root,"contract",contract,JSON_TYPE_OBJECT);
  if (!parse_contract(contract,&(agent->starting_contract))) return false;

  struct JsonNode *faction;
  JSON_OBJECT_GET_SET(root,"faction",faction,JSON_TYPE_OBJECT);
  if (!parse_faction(faction,&(agent->faction))) return false;

  struct JsonNode *ship;
  JSON_OBJECT_GET_SET(root,"ship",ship,JSON_TYPE_OBJECT);
  if (!parse_ship(ship,&(agent->ship))) return false;

  return true;
}

void free_new_agent(struct NewAgentData *agent){
  free(agent->token);
  free_contract(&(agent->starting_contract));
  free_faction(&(agent->faction));
  free_agent_details(&(agent->agent));
  free_ship(&(agent->ship));
}