#include "agents.h"

#include <stdio.h>
#include <string.h>

#include "util.h"

char* parse_token_new_agent(struct JsonNode* root) {
  struct JsonNode* data;
  JSON_OBJECT_GET_SET(root, "data", data, JSON_TYPE_OBJECT);

  char* token;
  JSON_OBJECT_GET_SET(data, "token", token, JSON_TYPE_STRING);

  if (token == NULL) {
    fprintf(stderr, "Failed to get token from new agent's data\n");
  }

  return strdup(token);
}

bool parse_agent_details(struct JsonNode* root, struct Agent* agent) {
  JSON_OBJECT_GET_SET(root, "data", root, JSON_TYPE_OBJECT);

  JSON_OBJECT_GET_SET(root, "accountId", agent->account_id, JSON_TYPE_STRING);

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