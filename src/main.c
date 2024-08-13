#include "main.h"

#include <curl/curl.h>

static void parse_args(int argc, char** argv);
static void handle_exit(int signum);
static void curl_init();

char* username;
char* token;

CURL* curl_hnd = NULL;
struct curl_slist* curl_headers = NULL;
struct CurlResponse curl_response;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  curl_init();

  parse_args(argc, argv);

  GameStatus status;
  if (!get_game_status(&status)) {
    printf("Failed to get game status\n");
  } else {
    // print_game_status(&status);
  }

  handle_exit(0);
}

static void parse_args(int argc, char** argv) {
  switch (argc) {
    case 0:
    case 1:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
    case 2:
      if (IS_HELP_FLAG(argv[1])) {
        PRINT_HELP_MESSAGE();
        exit(0);
      } else {
        PRINT_STANDARD_ERROR();
        exit(1);
      }
      break;
    case 3:
      if (IS_TOKEN_FLAG(argv[1])) {
        token = argv[1];
        // TODO - Add username fetching
      } else if (IS_USERNAME_FLAG(argv[1])) {
        username = argv[1];
        // TODO - Add automatic token generation
      } else {
        PRINT_STANDARD_ERROR();
        exit(1);
      }
      break;
    default:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
  }
}

static void curl_init() {
  curl_hnd = curl_easy_init();
  if (curl_hnd == NULL) {
    printf("Curl didn't init right\n");
    exit(1);
  }

  curl_headers = curl_slist_append(curl_headers, "Accept: application/json");
  curl_easy_setopt(curl_hnd, CURLOPT_HTTPHEADER, curl_headers);

  curl_easy_setopt(curl_hnd, CURLOPT_WRITEFUNCTION, curl_response_to_json);
  curl_easy_setopt(curl_hnd, CURLOPT_WRITEDATA, (void*)&curl_response);
}

static void handle_exit(int signum __attribute__((unused))) {
  printf("Exiting program...\n");
  curl_easy_cleanup(curl_hnd);
  exit(0);
}