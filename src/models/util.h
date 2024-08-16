#ifndef SPACETRADERS_MODELS_UTILS
#define SPACETRADERS_MODELS_UTILS

#include <stdbool.h>

#define ARRAY_STRUCT(struct_def, name) \
  struct {                             \
    struct struct_def* start;          \
    size_t len;                        \
  } name

#endif