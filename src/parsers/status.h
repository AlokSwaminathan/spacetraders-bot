#ifndef SPACETRADERS_PARSERS_STATUS
#define SPACETRADERS_PARSERS_STATUS

#include <stdbool.h>
#include "../models.h"

// Returns true if successful, false if failed
bool parse_game_status(JsonNode *root, GameStatus *status);

#endif