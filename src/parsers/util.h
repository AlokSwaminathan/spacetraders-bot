#ifndef SPACETRADERS_PARSERS_UTIL
#define SPACETRADERS_PARSERS_UTIL

#include <string.h>

#include "../util.h"
#include "json.h"

#define ASSERT_NODE(node, json_type) \
  if (node == NULL || (node)->type != json_type) return false;

#define JSON_SET_VALUE(node, value)                 \
  switch ((tmp_node)->type) {                       \
    case JSON_TYPE_OBJECT:                          \
    case JSON_TYPE_ARRAY:                           \
      *(struct JsonNode**)&(value) = node;          \
      break;                                        \
    case JSON_TYPE_STRING: {                        \
      char* tmp_char_ptr = strdup(node->value_str); \
      MALLOC_CHECK(tmp_char_ptr);                   \
      *(char**)&(value) = tmp_char_ptr;             \
      break;                                        \
    }                                               \
    case JSON_TYPE_BOOL:                            \
      *(bool*)&(value) = node->value_bool;          \
      break;                                        \
    case JSON_TYPE_DOUBLE:                          \
      *(double*)&(value) = node->value_double;      \
      break;                                        \
    case JSON_TYPE_LONG_LONG:                       \
      *(long long*)&(value) = node->value_ll;       \
      break;                                        \
    case JSON_TYPE_NULL:                            \
      *(void**)&(value) = NULL;                     \
      break;                                        \
  };

#define JSON_OBJECT_GET_SET(object, key, value, type)         \
  {                                                           \
    struct JsonNode* tmp_node = json_object_get(object, key); \
    ASSERT_NODE(tmp_node, type);                              \
    JSON_SET_VALUE(tmp_node, value);                          \
  }

#define JSON_ARRAY_GET_SET(array, index, value, type)         \
  {                                                           \
    struct JsonNode* tmp_node = json_array_get(array, index); \
    ASSERT_NODE(tmp_node, type);                              \
    JSON_SET_VALUE(tmp_node, value);                          \
  }

// curr is the active node
// i is the index in the array
#define JSON_ARRAY_MAP(array, array_struct, code)                                        \
  {                                                                                      \
    (array_struct).len = (array)->ele_count;                                             \
    (array_struct).start = malloc(sizeof(*((array_struct).start)) * (array_struct).len); \
    if ((array_struct).start == NULL && (array_struct).len != 0) return false;           \
    struct JsonNode* curr = (array)->child;                                              \
    for (int i = 0; curr != NULL && (array)->ele_count != 0; curr = curr->next, i++) {   \
      code;                                                                              \
    }                                                                                    \
  }

#define JSON_OBJECT_MAP(object, array_struct, code) JSON_ARRAY_MAP(object, array_struct, code);

// i is the current index
#define ARRAY_STRUCT_FREE(array_struct, code)       \
  {                                                 \
    for (size_t i = 0; i < array_struct.len; i++) { \
      code;                                         \
    }                                               \
    free(array_struct.start);                       \
  }

#endif