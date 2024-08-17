#include "main.h"

#include <curl/curl.h>

static struct CliArgs parse_args(int argc, char** argv);
static struct curl_slist* curl_init();
static void curl_add_auth(struct curl_slist* curl_headers, char* token);

CURL* curl_hnd = NULL;
struct CurlResponse curl_response;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  struct curl_slist *headers = curl_init();

  struct CliArgs args = parse_args(argc, argv);

  if (args.token == NULL) {
    args.token = register_agent(args.symbol, args.faction, args.email);
    curl_add_auth(headers,args.token);
  } else {
    curl_add_auth(headers,args.token);
    args.symbol = get_agent_data();
  }

  if (args.token == NULL || args.symbol == NULL) {
    fprintf(stderr, "Failed to malloc memory for token or agent symbol\n");
  }

  struct GameStatus status;
  if (!get_game_status(&status)) {
    printf("Failed to get game status\n");
  } else {
    print_game_status(&status);
  }

  printf("\n");
  printf("Agent symbol: %s\n", args.symbol);
  printf("Token: %s\n", args.token);

  handle_exit(0);
}

static struct CliArgs parse_args(int argc, char** argv) {
  struct CliArgs ret = {
      .email = NULL,
      .faction = NULL,
      .symbol = NULL,
      .token = NULL,
  };
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
        ret.token = argv[2];
      } else {
        PRINT_STANDARD_ERROR();
        exit(1);
      }
      break;
    case 5: {
      for (int i = 1; i < 5; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) ret.symbol = argv[i + 1];
        if (IS_FACTION_FLAG(argv[i])) ret.faction = argv[i + 1];
      }

      if (ret.symbol == NULL || ret.faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      break;
    }
    case 7: {
      for (int i = 1; i < 7; i += 2) {
        if (IS_AGENT_SYMBOL_FLAG(argv[i])) ret.symbol = argv[i + 1];
        if (IS_FACTION_FLAG(argv[i])) ret.faction = argv[i + 1];
        if (IS_EMAIL_FLAG(argv[i])) ret.email = argv[i + 1];
      }
      if (ret.email == NULL || ret.symbol == NULL || ret.faction == NULL) {
        PRINT_STANDARD_ERROR();
        exit(1);
      }

      break;
    }
    default:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
  }
  return ret;
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