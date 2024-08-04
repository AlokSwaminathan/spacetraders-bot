#ifndef JSON_H
#define JSON_H

// This only supports JSON objects that are surrounded by curly braces
// Does not support utf-8 or \u
// Does not support numbers that cannot fit in a double or a long long

#include <stdbool.h>
#include <stdint.h>

#define JSON_RC_BRACKET '}'
#define JSON_LC_BRACKET '{'

#define JSON_R_BRACKET ']'
#define JSON_L_BRACKET '['

#define JSON_D_QUOTE '\"'
#define JSON_DECIMAL '.'
#define JSON_COMMA ','
#define JSON_COLON ':'

// Special characters
#define JSON_SPECIAL_START '\\'
#define JSON_SPECIAL_NEWLINE 'n'
#define JSON_SPECIAL_QUOTE '\"'
#define JSON_SPECIAL_BACKSLASH '\\'
#define JSON_SPECIAL_FSLASH '/'
#define JSON_SPECIAL_FORMFEED 'f'
#define JSON_SPECIAL_CARRIAGE_RET 'r'
#define JSON_SPECIAL_TAB 't'
#define JSON_SPECIAL_BACKSPACE 'b'

#define JSON_IS_WHITESPACE(c) (c == ' ' || c == '\n' || c == '\t' || c == '\r')
#define JSON_IS_NUM_START(c) ((c >= '0' && c <= '9') || c == '.' || c == '-')
#define JSON_IS_NUM(c) (JSON_IS_NUM_START(c) && c != '-')
#define JSON_IS_STRING_START(c) (c == '\"')
#define JSON_IS_OBJECT_START(c) (c == JSON_LC_BRACKET)
#define JSON_IS_ARR_START(c) (c == JSON_L_BRACKET)
#define JSON_IS_NULL_START(c) (c == 'n')
#define JSON_IS_TRUE_START(c) (c == 't')
#define JSON_IS_FALSE_START(c) (c == 'f')
#define JSON_IS_LITERAL_START(c) (JSON_IS_FALSE_START(c) || JSON_IS_TRUE_START(c) || JSON_IS_NULL_START(c))
#define JSON_READ_WHITESPACE(str) \
  while (JSON_IS_WHITESPACE(*str) && *str != '\0') str++

typedef enum JsonDataType {
  JSON_TYPE_LONG_LONG,  // 0
  JSON_TYPE_DOUBLE,     // 1
  JSON_TYPE_STRING,     // 2
  JSON_TYPE_OBJECT,     // 3
  JSON_TYPE_ARRAY,      // 4
  JSON_TYPE_ROOT,       // 5
  JSON_TYPE_NULL,       // 6
  JSON_TYPE_BOOL,       // 7
} JsonDataType;

typedef struct JsonNode JsonNode;
typedef struct JsonNode {
  JsonNode *parent;
  JsonNode *prev;
  JsonNode *next;
  JsonDataType type;
  uint32_t value_len;
  bool is_array_ele;
  char *key;
  union {
    JsonNode *child;
    bool value_bool;
    long long value_ll;
    double value_double;
    char *value_str;
    void *value_void;
  };
} JsonNode;

// Takes the ROOT node and frees the object
// Does nothing if node is not of type ROOT
void free_json(JsonNode *json_root);

// Takes in a full JSON string (null terminated)
// Returns a pointer to the ROOT JsonNode
// Returns NULL if any errors occurred
JsonNode *parse_json(char *json_string);

// Checks if a null terminated string is valid json or not
bool validate_json(char *json_string);

#endif