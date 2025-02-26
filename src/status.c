#include "status.h"

#include <stdio.h>

void print_game_status(struct GameStatus* status) {
  if (status == NULL) return;

  printf("Spacetraders Game Status\n");
  printf("Server Status: %s\n", status->status);
  printf("API Version: %s\n", status->ver);
  printf("Last Reset Date: %s\n", status->reset_date);
  printf("Next Reset Date: %s\n", status->next_reset);

  printf("Stats:\n");
  printf("  %d agents\n", status->stats.agents);
  printf("  %d ships\n", status->stats.ships);
  printf("  %d systems\n", status->stats.systems);
  printf("  %d waypoints\n", status->stats.waypoints);

  printf("\nMost Credits:\n");
  for (size_t i = 0; i < status->most_credits.len; i++) {
    printf("  %zu. Agent symbol: %s, Credits: %lld\n", i + 1, status->most_credits.start[i].agent_symbol, status->most_credits.start[i].credits);
  }

  printf("\nMost Charts Submitted:\n");
  for (size_t i = 0; i < status->most_charts.len; i++) {
    printf("  %zu. Agent symbol: %s, Charts: %d\n", i + 1, status->most_charts.start[i].agent_symbol, status->most_charts.start[i].chart_count);
  }

  printf("\nAnnouncements:\n");
  for (size_t i = 0; i < status->announcements.len; i++) {
    printf("  %zu. %s - %s\n", i + 1, status->announcements.start[i].title, status->announcements.start[i].body);
  }
}