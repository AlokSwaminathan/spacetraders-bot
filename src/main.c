#include <curl/curl.h>
#include <signal.h>

#include "cli.h"
#include "constants.h"
#include "curl.h"
#include "json.h"
#include "models.h"
#include "parsers.h"
#include "requests.h"
#include "status.h"
#include "util.h"

CURL* curl_hnd = NULL;
struct CurlResponse curl_response;

struct CliArgs args;

char g_error_msg[ERROR_MSG_LEN];

const char* g_error_json_name = DEFAULT_JSON_NAME;

int main(int argc, char** argv) {
  signal(SIGINT, handle_exit);

  struct curl_slist* headers = curl_init(&curl_hnd);

  parse_args(argc, argv, &args);

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
    curl_add_auth(curl_hnd, headers, args.token);
    free(token);
  } else {
    curl_add_auth(curl_hnd, headers, args.token);
    char* symbol = get_agent_data();
    strcpy(args.symbol, symbol);
    free(symbol);
  }

  printf("Agent symbol: %s\n", args.symbol);
  printf("Token: %s\n", args.token);

  handle_exit(0);
}