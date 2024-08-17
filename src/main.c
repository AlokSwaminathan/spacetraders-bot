#include "main.h"

#include <curl/curl.h>

static void parse_args(int argc, char** argv);
static struct curl_slist* curl_init();
static void curl_add_auth(struct curl_slist* curl_headers, char* token);

CURL* curl_hnd = NULL;
struct CurlResponse curl_response;

struct CliArgs args;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  struct curl_slist* headers = curl_init();

  parse_args(argc, argv);

  struct GameStatus status;
  if (!get_game_status(&status)) {
    printf("Failed to get game status\n");
  } else {
    print_game_status(&status);
    printf("\n");
  }
  free_game_status(&status);

  if (args.token[0] == '\0') {
    char* token = register_agent(args.symbol, args.faction, args.email);
    strcpy(args.token, token);
    curl_add_auth(headers, args.token);
    free(token);
  } else {
    curl_add_auth(headers, args.token);
    char* symbol = get_agent_data();
    strcpy(args.symbol, symbol);
    free(symbol);
  }

  printf("Agent symbol: %s\n", args.symbol);
  printf("Token: %s\n", args.token);

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
        memcpy(args.token, argv[2], strlen(argv[2]));
      } else {
        PRINT_STANDARD_ERROR();
        exit(1);
      }
      break;
    case 5: {
      char* symbol = NULL;
      char* faction = NULL;

      for (int i = 1; i < 5; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) symbol = argv[i + 1];
        if (IS_FACTION_FLAG(argv[i])) faction = argv[i + 1];
      }

      if (symbol == NULL || faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      memcpy(args.symbol, symbol, strlen(symbol));
      memcpy(args.faction, faction, strlen(faction));

      break;
    }
    case 7: {
      char* symbol = NULL;
      char* faction = NULL;
      char* email = NULL;

      for (int i = 1; i < 7; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) symbol = argv[i + 1];
        if (IS_FACTION_FLAG(argv[i])) faction = argv[i + 1];
        if (IS_EMAIL_FLAG(argv[i])) email = argv[i + 1];
      }
      if (email == NULL || symbol == NULL || faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      memcpy(args.symbol, symbol, strlen(symbol));
      memcpy(args.faction, faction, strlen(faction));
      memcpy(args.email, email, strlen(email));

      break;
    }
    default:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
  }
}

static struct curl_slist* curl_init() {
  curl_hnd = curl_easy_init();
  if (curl_hnd == NULL) {
    printf("Curl didn't init right\n");
    exit(1);
  }

  struct curl_slist* curl_headers = NULL;

  curl_headers = curl_slist_append(curl_headers, "Content-Type: application/json");
  curl_headers = curl_slist_append(curl_headers, "Accept: application/json");

  curl_easy_setopt(curl_hnd, CURLOPT_HTTPHEADER, curl_headers);

  curl_easy_setopt(curl_hnd, CURLOPT_WRITEFUNCTION, curl_response_to_json);
  curl_easy_setopt(curl_hnd, CURLOPT_WRITEDATA, (void*)&curl_response);

  return curl_headers;
}

static void curl_add_auth(struct curl_slist* curl_headers, char* token) {
  // Add bearer auth token
  char header_token[BEARER_TOKEN_START_LEN + strlen(token) + 1];
  memcpy(header_token, BEARER_TOKEN_START, BEARER_TOKEN_START_LEN);
  memcpy(header_token + BEARER_TOKEN_START_LEN, token, strlen(token) + 1);
  curl_headers = curl_slist_append(curl_headers, (const char*)header_token);

  curl_easy_setopt(curl_hnd, CURLOPT_HTTPHEADER, curl_headers);
}