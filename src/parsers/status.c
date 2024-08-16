#include "status.h"

#include "util.h"

static bool parse_game_status_stats(struct JsonNode *root, struct GameStatusStats *stats) {
  JSON_OBJECT_GET_SET(root, "agents", stats->agents, JSON_TYPE_LONG_LONG);
  JSON_OBJECT_GET_SET(root, "ships", stats->ships, JSON_TYPE_LONG_LONG);
  JSON_OBJECT_GET_SET(root, "systems", stats->systems, JSON_TYPE_LONG_LONG);
  JSON_OBJECT_GET_SET(root, "waypoints", stats->waypoints, JSON_TYPE_LONG_LONG);
  return true;
}

bool parse_game_status(struct JsonNode *root, struct GameStatus *status) {
  status->root = root;

  JSON_OBJECT_GET_SET(root, "status", status->status, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "version", status->ver, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "resetDate", status->reset_date, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", status->description, JSON_TYPE_STRING);

  struct JsonNode *resets;
  JSON_OBJECT_GET_SET(root, "serverResets", resets, JSON_TYPE_OBJECT);

  JSON_OBJECT_GET_SET(resets, "next", status->next_reset, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(resets, "frequency", status->reset_freq, JSON_TYPE_STRING);

  struct JsonNode *stats;
  JSON_OBJECT_GET_SET(root, "stats", stats, JSON_TYPE_OBJECT);
  if (!parse_game_status_stats(stats, &(status->stats))) return false;

  struct JsonNode *leaderboards;
  JSON_OBJECT_GET_SET(root, "leaderboards", leaderboards, JSON_TYPE_OBJECT);

  struct JsonNode *most_credits;
  JSON_OBJECT_GET_SET(leaderboards, "mostCredits", most_credits, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(most_credits, status->most_credits, {
    JSON_OBJECT_GET_SET(curr, "agentSymbol", status->most_credits.start[i].agent_symbol, JSON_TYPE_STRING);
    JSON_OBJECT_GET_SET(curr, "credits", status->most_credits.start[i].credits, JSON_TYPE_LONG_LONG);
  });

  struct JsonNode *most_charts;
  JSON_OBJECT_GET_SET(leaderboards, "mostSubmittedCharts", most_charts, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(most_charts, status->most_charts, {
    JSON_OBJECT_GET_SET(curr, "agentSymbol", status->most_charts.start[i].agent_symbol, JSON_TYPE_STRING);
    JSON_OBJECT_GET_SET(curr, "credits", status->most_charts.start[i].chart_count, JSON_TYPE_LONG_LONG);
  });

  struct JsonNode *announcements;
  JSON_OBJECT_GET_SET(root, "announcements", announcements, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(announcements, status->announcements, {
    JSON_OBJECT_GET_SET(curr, "title", status->announcements.start[i].title, JSON_TYPE_STRING);
    JSON_OBJECT_GET_SET(curr, "body", status->announcements.start[i].body, JSON_TYPE_STRING);
  });

  struct JsonNode *links;
  JSON_OBJECT_GET_SET(root, "links", links, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(links, status->links, {
    JSON_OBJECT_GET_SET(curr, "name", status->links.start[i].name, JSON_TYPE_STRING);
    JSON_OBJECT_GET_SET(curr, "url", status->links.start[i].url, JSON_TYPE_STRING);
  });

  return true;
}
