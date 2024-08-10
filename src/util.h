#ifndef SPACETRADERS_UTILS
#define SPACETRADERS_UTILS

#include <time.h>
#include <stdbool.h>

#define ARRAY_STRUCT(struct_def, name) \
  struct {                             \
    struct struct_def* start;          \
    size_t len;                        \
  } name

#endif