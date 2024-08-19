#ifndef SPACETRADERS_UTIL
#define SPACETRADERS_UTIL

#include <stdio.h>

#define RELATIVE_FILE_PATH(file) (strstr(file, "src/") ? strstr(file, "src/") : file)

#define MALLOC_ERROR()                                                                                                                              \
  {                                                                                                                                                 \
    fprintf(stderr, "Failed to malloc at line %d in file %s in function %s, exiting program...", __LINE__, RELATIVE_FILE_PATH(__FILE__), __func__); \
    exit(1);                                                                                                                                        \
  }

#define MALLOC_CHECK(ptr) \
  {                       \
    void* _ptr = ptr;     \
    if (_ptr == NULL) {   \
      MALLOC_ERROR();     \
    }                     \
  };

void handle_exit(int signum);

#endif