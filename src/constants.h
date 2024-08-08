#ifndef SPACETRADER_BOT_CONSTANTS
#define SPACETRADER_BOT_CONSTANTS

#include <string.h>

// CLI flag constants
#define CLI_HELP_MESSAGE                                                  \
  "%s --help\n"                                                           \
  "\n"                                                                    \
  "Usage: %s [options]\n"                                                 \
  "\n"                                                                    \
  "Options:\n"                                                            \
  "  -h|--help   Show help output\n"                                      \
  "  -u|--username Pass in a usernmae to claim to get the token to use\n" \
  "  -t|--token Pass in a token to use\n"                                 \
  "\n"                                                                    \
  "  Can only pass one of the 3 arguments\n"

#define CLI_STANDARD_ERROR_MSG "ERROR: Must pass in either --help, --username <username>, or --token <token>\n"

#define PRINT_STANDARD_ERROR() printf(CLI_STANDARD_ERROR_MSG CLI_HELP_MESSAGE, argv[0], argv[0]);
#define PRINT_HELP_MESSAGE() printf(CLI_HELP_MESSAGE, argv[0], argv[0])

#define HELP_FLAG_SHORT "-h"
#define HELP_FLAG_LONG "--help"
#define IS_HELP_FLAG(str) (strcmp(str, HELP_FLAG_LONG) == 0 || strcmp(str, HELP_FLAG_SHORT) == 0)

#define USERNAME_FLAG_SHORT "-u"
#define USERNAME_FLAG_LONG "-username"
#define IS_USERNAME_FLAG(str) (strcmp(str, USERNAME_FLAG_LONG) == 0 || strcmp(str, USERNAME_FLAG_SHORT) == 0)

#define TOKEN_FLAG_SHORT "-t"
#define TOKEN_FLAG_LONG "--token"
#define IS_TOKEN_FLAG(str) (strcmp(str, TOKEN_FLAG_LONG) == 0 || strcmp(str, TOKEN_FLAG_SHORT) == 0)

#endif