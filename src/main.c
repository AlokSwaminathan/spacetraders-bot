#include "main.h"

#include <curl/curl.h>

static void parse_args(int argc, char** argv);
static void curl_init();
static void curl_add_auth();

char* agent_symbol;
char* token;

CURL* curl_hnd = NULL;
struct curl_slist* curl_headers = NULL;
struct CurlResponse curl_response;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  curl_init();

  parse_args(argc, argv);

  curl_add_auth();

  struct GameStatus status;
  if (!get_game_status(&status)) {
    printf("Failed to get game status\n");
  } else {
    print_game_status(&status);
  }

  printf("\n");
  printf("Agent symbol: %s", agent_symbol);
  printf("Token: %s", token);

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
        token = strdup(argv[2]);
        get_agent_data(token);
      } else {
        PRINT_STANDARD_ERROR();
        exit(1);
      }
      break;
    case 5: {
      char* symbol = NULL;
      char* faction = NULL;

      for (int i = 1; i < 5; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) symbol = argv[i+1];
        if (IS_FACTION_FLAG(argv[i])) faction = argv[i+1];
      }

      if (symbol == NULL || faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      token = register_agent(symbol,faction,NULL);
      agent_symbol = strdup(symbol);
      break;
    }
    case 7: {
      char* email = NULL;
      char* symbol = NULL;
      char* faction = NULL;
      for (int i = 1; i < 7; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) symbol = argv[i+1];
        if (IS_FACTION_FLAG(argv[i])) faction = argv[i+1];
        if (IS_EMAIL_FLAG(argv[i])) email = argv[i+1];
      }
      if (email == NULL || symbol == NULL || faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      token = register_agent(symbol, faction, email);
      agent_symbol = strdup(symbol);
      break;
    }
    default:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
  }
  if (!token || !agent_symbol) {
    fprintf(stderr, "Failed to malloc memory for token or agent symbol\n");
  }
}

static void curl_init() {
  curl_hnd = curl_easy_init();
  if (curl_hnd == NULL) {
    printf("Curl didn't init right\n");
    exit(1);
  }

  curl_headers = curl_slist_append(curl_headers, "Content-Type: application/json");
  curl_headers = curl_slist_append(curl_headers, "Accept: application/json");

  curl_easy_setopt(curl_hnd, CURLOPT_HTTPHEADER, curl_headers);

  curl_easy_setopt(curl_hnd, CURLOPT_WRITEFUNCTION, curl_response_to_json);
  curl_easy_setopt(curl_hnd, CURLOPT_WRITEDATA, (void*)&curl_response);
}

static void curl_add_auth() {
  // Add bearer auth token
  char header_token[BEARER_TOKEN_START_LEN + strlen(token) + 1];
  memcpy(header_token, BEARER_TOKEN_START, BEARER_TOKEN_START_LEN);
  memcpy(header_token + BEARER_TOKEN_START_LEN, token, strlen(token) + 1);
  curl_headers = curl_slist_append(curl_headers, (const char*)header_token);

  curl_easy_setopt(curl_hnd, CURLOPT_HTTPHEADER, curl_headers);
}