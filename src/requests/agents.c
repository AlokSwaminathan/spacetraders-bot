#include "agents.h"

#include "../parsers.h"
#include "../util.h"
#include "json.h"
#include "string.h"
#include "util.h"

char* register_agent(char* agent_symbol, char* faction, char* email) {
  struct JsonNode* root = json_new_node(JSON_TYPE_OBJECT, NULL);

  json_object_set_kv(root, "symbol", json_new_node(JSON_TYPE_STRING, agent_symbol));

  json_object_set_kv(root, "faction", json_new_node(JSON_TYPE_STRING, faction));

  if (email != NULL) {
    json_object_set_kv(root, "email", json_new_node(JSON_TYPE_STRING, email));
  }

  char postfields[json_dump_str_len(root)];

  json_dump(root, postfields, sizeof(postfields));

  struct JsonNode* agent_info = curl_get_valid_json("POST", "https://api.spacetraders.io/v2/register", postfields);

  char* token = parse_token_new_agent(agent_info);
  MALLOC_CHECK(token);

  free_json(agent_info);
  free_json(root);

  return token;
}

char* get_agent_data(void) {
  struct JsonNode* root = curl_get_valid_json("GET", "https://api.spacetraders.io/v2/my/agent", NULL);

  struct Agent agent;
  if (!parse_agent_details(root, &agent)) {
    fprintf(stderr, "Failed to parse agent data to get symbol\n");
    exit(1);
  }
  free_json(root);

  char* symbol = strdup(agent.symbol);
  MALLOC_CHECK(symbol);

  free_agent_details(&agent);

  return symbol;
}