#ifndef SPACETRADERS_UTIL
#define SPACETRADERS_UTIL

#include <stdio.h>

#define MALLOC_ERROR()                                              \
  {                                                                 \
    fprintf(stderr, "Failed to malloc, exiting program..."); \
    exit(1);                                                        \
  }

#define MALLOC_CHECK(ptr) \
  {                       \
    void* _ptr = ptr;     \
    if (_ptr == NULL) {    \
      MALLOC_ERROR();     \
    }                     \
  };

void handle_exit(int signum);

#endif