#include <stdio.h>

#include "constants.h"
#include "json.h"

void parse_args(int argc, char** argv);

char* USERNAME;
char* TOKEN;

int main(int argc, char** argv) {
  parse_args(argc,argv);
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
        TOKEN = argv[1];
        // TODO - Add username fetching
      } else if (IS_USERNAME_FLAG(argv[1])) {
        USERNAME = argv[1];
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
