#ifndef SPACETRADER_REQUEST_STATUS
#define SPACETRADER_REQUEST_STATUS

#include "../models.h"

GameStatus get_game_status(void);

void print_game_status(GameStatus status);

#endif