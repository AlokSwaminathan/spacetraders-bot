#ifndef SPACETRADERS_PARSERS_STATUS
#define SPACETRADERS_PARSERS_STATUS

#include <stdbool.h>

#include "../models.h"

// Returns true if successful, false if failed
bool parse_game_status(struct JsonNode *root, struct GameStatus *status);

void free_game_status(struct GameStatus *status);

#endif