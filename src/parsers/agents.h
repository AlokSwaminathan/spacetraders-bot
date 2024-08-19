#ifndef SPACETRADERS_PARSER_AGENTS
#define SPACETRADERS_PARSER_AGENTS

#include "json.h"
#include "../models.h"

bool parse_agent_details(struct JsonNode *root, struct Agent *agent);

void free_agent_details(struct Agent *agent);

bool parse_new_agent(struct JsonNode *root, struct NewAgentData *agent);

void free_new_agent(struct NewAgentData *agent);

#endif