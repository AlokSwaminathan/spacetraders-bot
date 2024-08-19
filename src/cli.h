#ifndef SPACETRADERS_CLI
#define SPACETRADERS_CLI

struct CliArgs {
  char token[1024];
  char symbol[1024];
  char faction[1024];
  char email[1024];
};

void parse_args(int argc, char** argv, struct CliArgs* args);

#define CLI_HELP_MESSAGE                                               \
  "%s --help\n"                                                        \
  "\n"                                                                 \
  "Usage: %s [options]\n"                                              \
  "\n"                                                                 \
  "Options:\n"                                                         \
  "  -h|--help    Show help output\n"                                  \
  "  -t|--token    Pass in a token to use\n"                           \
  "  -s|--agent-symbol    Pass in a symbol to claim\n"                 \
  "  -f|--faction    Pass in a faction to claim\n"                     \
  "  -e|--email    Optionally pass in an email to claim symbol with\n" \
  "\n"                                                                 \
  "Examples:\n"                                                        \
  "  %s -h\n"                                                          \
  "  %s -t token123\n"                                                 \
  "  %s -s custom-symbol -f COSMIC\n"                                  \
  "  %s -s custom-symbol -f COSMIC -e e@mail.com\n"

#define CLI_STANDARD_ERROR_MSG "ERROR: Must pass in either --help, --agent-symbol <symbol> --faction <faction>, or --token <token>\n"

#define PRINT_STANDARD_ERROR() fprintf(stderr, CLI_STANDARD_ERROR_MSG CLI_HELP_MESSAGE, argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
#define PRINT_HELP_MESSAGE() printf(CLI_HELP_MESSAGE, argv[0], argv[0], argv[0], argv[0], argv[0], argv[0])

#define HELP_FLAG_SHORT "-h"
#define HELP_FLAG_LONG "--help"
#define IS_HELP_FLAG(str) (strcmp(str, HELP_FLAG_LONG) == 0 || strcmp(str, HELP_FLAG_SHORT) == 0)

#define AGENT_SYMBOL_FLAG_SHORT "-s"
#define AGENT_SYMBOL_FLAG_LONG "--agent-symbol"
#define IS_AGENT_SYMBOL_FLAG(str) (strcmp(str, AGENT_SYMBOL_FLAG_LONG) == 0 || strcmp(str, AGENT_SYMBOL_FLAG_SHORT) == 0)

#define TOKEN_FLAG_SHORT "-t"
#define TOKEN_FLAG_LONG "--token"
#define IS_TOKEN_FLAG(str) (strcmp(str, TOKEN_FLAG_LONG) == 0 || strcmp(str, TOKEN_FLAG_SHORT) == 0)

#define FACTION_FLAG_SHORT "-f"
#define FACTION_FLAG_LONG "--faction"
#define IS_FACTION_FLAG(str) (strcmp(str, FACTION_FLAG_SHORT) == 0 || strcmp(str, FACTION_FLAG_LONG) == 0)

#define EMAIL_FLAG_SHORT "-e"
#define EMAIL_FLAG_LONG "--email"
#define IS_EMAIL_FLAG(str) (strcmp(str, EMAIL_FLAG_SHORT) == 0 || strcmp(str, EMAIL_FLAG_LONG) == 0)

#endif
