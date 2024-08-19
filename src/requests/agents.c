#include "agents.h"

#include "../parsers.h"
#include "../util.h"
#include "json.h"
#include "string.h"
#include "util.h"

char* register_agent(char* agent_symbol, char* faction, char* email) {
  struct JsonNode* request_json = json_new_node(JSON_TYPE_OBJECT, NULL);

  json_object_set_kv(request_json, "symbol", json_new_node(JSON_TYPE_STRING, agent_symbol));

  json_object_set_kv(request_json, "faction", json_new_node(JSON_TYPE_STRING, faction));

  if (email != NULL) {
    json_object_set_kv(request_json, "email", json_new_node(JSON_TYPE_STRING, email));
  }

  char postfields[json_dump_str_len(request_json)];

  json_dump(request_json, postfields, sizeof(postfields));

  struct JsonNode* agent_info = curl_get_valid_json("POST", "https://api.spacetraders.io/v2/register", postfields);

  struct JsonNode* real_data = parse_data(agent_info);
  if (real_data == NULL){
    fprintf(stderr,"No data field in new agent data struct\n");
    exit(1);
  }

  struct NewAgentData agent;
  g_error_json_name = "new_agent_data";
  if (!parse_new_agent(real_data, &agent)) {
    fprintf(stderr, "Failed to parse new agent data\n");
    fprintf(stderr, "%s\n",g_error_msg);
    exit(1);
  }

  char* token = strdup(agent.token);
  MALLOC_CHECK(token);

  free_json(agent_info);
  free_json(request_json);
  free_new_agent(&agent);

  return token;
}

char* get_agent_data(void) {
  struct JsonNode* root = curl_get_valid_json("GET", "https://api.spacetraders.io/v2/my/agent", NULL);

  struct JsonNode* real_data = parse_data(root);
  if (real_data == NULL){
    fprintf(stderr,"Failed to get data field out of agent info");
    exit(1);
  }

  struct Agent agent;
  g_error_json_name = "agent_details";
  if (!parse_agent_details(real_data, &agent)) {
    fprintf(stderr, "Failed to parse agent data to get symbol\n");
    fprintf(stderr, "%s\n",g_error_msg);
    exit(1);
  }
  free_json(root);

  char* symbol = strdup(agent.symbol);
  MALLOC_CHECK(symbol);

  free_agent_details(&agent);

  return symbol;
}