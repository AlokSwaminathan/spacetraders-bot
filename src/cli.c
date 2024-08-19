#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_args(int argc, char** argv, struct CliArgs* args) {
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
        memcpy(args->token, argv[2], strlen(argv[2]));
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

      memcpy(args->symbol, symbol, strlen(symbol));
      memcpy(args->faction, faction, strlen(faction));

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

      memcpy(args->symbol, symbol, strlen(symbol));
      memcpy(args->faction, faction, strlen(faction));
      memcpy(args->email, email, strlen(email));

      break;
    }
    default:
      PRINT_STANDARD_ERROR();
      exit(1);
      break;
  }
}