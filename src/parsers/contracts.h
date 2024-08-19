#ifndef SPACETRADERS_PARSER_CONTRACTS
#define SPACETRADERS_PARSER_CONTRACTS

#include "json.h"
#include "../models.h"

bool parse_contract(struct JsonNode *root, struct Contract *contract);

void free_contract(struct Contract *contract);

#endif