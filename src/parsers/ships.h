#ifndef SPACETRADERS_PARSER_SHIPS
#define SPACETRADERS_PARSER_SHIPS

#include "json.h"
#include "../models.h"

bool parse_ship(struct JsonNode *root, struct Ship *ship);

void free_ship(struct Ship *ship);

#endif