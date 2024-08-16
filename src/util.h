#ifndef SPACETRADERS_UTIL
#define SPACETRADERS_UTIL

#include <stdio.h>

#define MALLOC_ERROR()                                              \
  {                                                                 \
    fprintf(stderr, "Failed to malloc somewhere somethings wrong"); \
    exit(1);                                                        \
  }

#define MALLOC_CHECK(ptr) \
  {                       \
    if (ptr == NULL) {    \
      MALLOC_ERROR();     \
    }                     \
  };

void handle_exit(int signum);

#endif