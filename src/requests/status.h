#ifndef SPACETRADER_REQUEST_STATUS
#define SPACETRADER_REQUEST_STATUS

#include <stdbool.h>
#include "../models.h"

bool get_game_status(struct GameStatus *status);

void print_game_status(struct GameStatus *status);

#endif