#include "status.h"

#include <curl/curl.h>

#include "../parsers.h"
#include "json.h"
#include "util.h"

bool get_game_status(struct GameStatus* status) {
  struct JsonNode* status_json = curl_get_valid_json("GET", "https://api.spacetraders.io/v2/", NULL);

  g_error_json_name = "game_status";
  if (!parse_game_status(status_json, status)) return false;
  free_json(status_json);

  return true;
}