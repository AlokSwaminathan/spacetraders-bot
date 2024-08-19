#ifndef SPACETRADERS_PARSER_OTHER
#define SPACETRADERS_PARSER_OTHER

#include "json.h"
#include "../models.h"

bool parse_trait(struct JsonNode *root, struct Trait *trait);

void free_trait(struct Trait *trait);

#endif