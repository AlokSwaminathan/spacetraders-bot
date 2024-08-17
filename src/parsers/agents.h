#ifndef SPACETRADERS_PARSER_AGENTS
#define SPACETRADERS_PARSER_AGENTS

#include "json.h"
#include "../models.h"

char* parse_token_new_agent(struct JsonNode *root);

bool parse_agent_details(struct JsonNode *root, struct Agent *agent);

void free_agent_details(struct Agent *agent);

#endif