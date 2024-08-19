#ifndef SPACETRADERS_PARSERS_UTIL
#define SPACETRADERS_PARSERS_UTIL

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "../constants.h"
#include "../util.h"
#include "json.h"

extern char g_error_msg[ERROR_MSG_LEN];
extern const char* g_error_json_name;

time_t parse_date_time_str(char* str);

// Takes in array or object and the child element and writes to global error buffer the error and where the error happened in the JSON object
void write_parsing_error(struct JsonNode* parent, struct JsonNode* child, void* accessor, enum JsonDataType expected_type);

#define DEFAULT_JSON_NAME "json_data"
#define ERROR_LEFTOVER_MSG_LEN 150

// If Json data is in a "data" field this returns that field
// Returns NULL if there is no data field
#define parse_data(node) json_object_get(node, "data")

#define JSON_SET_VALUE(node, value, json_type)                                    \
  do {                                                                            \
    if ((json_type) == JSON_TYPE_DOUBLE && (node)->type == JSON_TYPE_LONG_LONG) { \
      *(double*)&(value) = (double)((node)->value_ll);                            \
      break;                                                                      \
    }                                                                             \
    switch ((tmp_node)->type) {                                                   \
      case JSON_TYPE_OBJECT:                                                      \
      case JSON_TYPE_ARRAY:                                                       \
        *(struct JsonNode**)&(value) = node;                                      \
        break;                                                                    \
      case JSON_TYPE_STRING: {                                                    \
        char* tmp_char_ptr = strdup(node->value_str);                             \
        MALLOC_CHECK(tmp_char_ptr);                                               \
        *(char**)&(value) = tmp_char_ptr;                                         \
        break;                                                                    \
      }                                                                           \
      case JSON_TYPE_BOOL:                                                        \
        *(bool*)&(value) = node->value_bool;                                      \
        break;                                                                    \
      case JSON_TYPE_DOUBLE:                                                      \
        *(double*)&(value) = node->value_double;                                  \
        break;                                                                    \
      case JSON_TYPE_LONG_LONG:                                                   \
        *(long long*)&(value) = node->value_ll;                                   \
        break;                                                                    \
      case JSON_TYPE_NULL:                                                        \
        *(void**)&(value) = NULL;                                                 \
        break;                                                                    \
    };                                                                            \
  } while (0);

#define JSON_WEAK_TYPE_VERIFY(node, json_type) (!(node->type != (json_type) && !((json_type) == JSON_TYPE_DOUBLE && tmp_node->type == JSON_TYPE_LONG_LONG)))

#define JSON_OBJECT_GET_SET(object, key, value, json_type)                 \
  {                                                                        \
    struct JsonNode* tmp_node = json_object_get(object, key);              \
    if (tmp_node == NULL || !JSON_WEAK_TYPE_VERIFY(tmp_node, json_type)) { \
      write_parsing_error((object), (tmp_node), (key), (json_type));       \
      return false;                                                        \
    }                                                                      \
    JSON_SET_VALUE(tmp_node, value, json_type);                            \
  }

#define JSON_OBJECT_GET_SET_NOT_REQ(object, key, value, json_type, default) \
  {                                                                         \
    struct JsonNode* tmp_node = json_object_get(object, key);               \
    if (tmp_node == NULL) {                                                 \
      value = default;                                                      \
    } else if (!JSON_WEAK_TYPE_VERIFY(tmp_node, json_type)) {               \
      write_parsing_error((object), (tmp_node), (key), (json_type));        \
      return false;                                                         \
    } else {                                                                \
      JSON_SET_VALUE(tmp_node, value, json_type);                           \
    }                                                                       \
  }

#define JSON_ARRAY_GET_SET(array, index, value, json_type)                      \
  {                                                                             \
    struct JsonNode* tmp_node = json_array_get(array, index);                   \
    if (tmp_node == NULL || !JSON_WEAK_TYPE_VERIFY(tmp_node, json_type)) {      \
      write_parsing_error((object), (tmp_node), &(size_t)(index), (json_type)); \
      return false;                                                             \
    }                                                                           \
    JSON_SET_VALUE(tmp_node, value, json_type);                                 \
  }

#define JSON_ARRAY_GET_SET_NOT_REQ(array, index, value, json_type, default)     \
  {                                                                             \
    struct JsonNode* tmp_node = json_array_get(array, index);                   \
    if (tmp_node == NULL) {                                                     \
      value = default;                                                          \
    } else if (!JSON_WEAK_TYPE_VERIFY(tmp_node, json_type)) {                   \
      write_parsing_error((object), (tmp_node), &(size_t)(index), (json_type)); \
      return false;                                                             \
    } else {                                                                    \
      JSON_SET_VALUE(tmp_node, value, json_type);                               \
    }                                                                           \
  }

// curr is the active node
// i is the index in the array
#define JSON_ARRAY_MAP(array, array_struct, code)                                          \
  {                                                                                        \
    (array_struct).len = (array)->ele_count;                                               \
    if ((array_struct).len > 0) {                                                          \
      (array_struct).start = malloc(sizeof(*((array_struct).start)) * (array_struct).len); \
    } else {                                                                               \
      (array_struct).start = NULL;                                                         \
    }                                                                                      \
    if ((array_struct).start == NULL && (array_struct).len > 0) MALLOC_ERROR();            \
    struct JsonNode* curr = (array)->child;                                                \
    for (int i = 0; curr != NULL && (array)->ele_count != 0; curr = curr->next, i++) {     \
      code;                                                                                \
    }                                                                                      \
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

#define CHECK_TIME_T(t) \
  if (t < 0) return false;

#endif