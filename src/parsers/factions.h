#ifndef SPACETRADERS_PARSER_FACTCIONS
#define SPACETRADERS_PARSER_FACTCIONS

#include "json.h"
#include "../models.h"

bool parse_faction(struct JsonNode *root, struct Faction *faction);

void free_faction(struct Faction *faction);

#endif