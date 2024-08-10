#include "main.h"

#include <curl/curl.h>

void parse_args(int argc, char** argv);
void handle_exit(int signum);

char* username;
char* token;
CURL* curl;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  curl = curl_easy_init();
  if (curl == NULL) {
    printf("Curl didn't init right\n");
    return 1;
  }
  parse_args(argc, argv);

  GameStatus status = get_game_status();
  print_game_status(status);

  handle_exit(0);
}

void parse_args(int argc, char** argv) {
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

void handle_exit(int signum) {
  printf("Exiting program...\n");
  curl_easy_cleanup(curl);
  exit(0);
}