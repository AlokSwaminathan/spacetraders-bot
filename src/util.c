#include "util.h"

#include <curl/curl.h>

extern char* agent_symbol;
extern char* token;
extern CURL* curl_hnd;

void handle_exit(int signum __attribute__((unused))) {
  printf("Exiting program...\n");
  curl_easy_cleanup(curl_hnd);
  free(agent_symbol);
  free(token);
  exit(0);
}