#include "util.h"

#include <curl/curl.h>
#include <stdlib.h>

extern CURL* curl_hnd;

void handle_exit(int signum __attribute__((unused))) {
  printf("\nExiting program...\n");
  curl_easy_cleanup(curl_hnd);
  exit(0);
}