#ifndef JSON_H
#define JSON_H

// This only supports JSON objects that are surrounded by curly braces
// Does not support utf-8 or \u
// Does not support numbers that cannot fit in a double or a long long

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define JSON_RC_BRACKET '}'
#define JSON_LC_BRACKET '{'

#define JSON_R_BRACKET ']'
#define JSON_L_BRACKET '['

#define JSON_D_QUOTE '\"'
#define JSON_DECIMAL '.'
#define JSON_COMMA ','
#define JSON_COLON ':'

#define JSON_NEWLINE '\n'
#define JSON_BACKSLASH '\\'
#define JSON_TAB '\t'
#define JSON_FSLASH '/'
#define JSON_CARRIAGE_RET '\r'
#define JSON_BACKSPACE '\b'
#define JSON_FORMFEED '\f'

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

#define JSON_IS_SPECIAL_CHAR(c) (c == JSON_BACKSLASH || c == JSON_NEWLINE || c == JSON_D_QUOTE || c == JSON_FSLASH || c == JSON_FORMFEED || c == JSON_CARRIAGE_RET || c == JSON_TAB || c == JSON_BACKSPACE)

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
#define JSON_STRING_READ_WHITESPACE(str) \
  while (JSON_IS_WHITESPACE(*str) && *str != '\0') str++

#define JSON_STRING_PAD_WHITESPACE(str, n) \
  for (int _i = 0; _i < n; _i++, str++) *str = ' '

#define JSON_STRING_ADD_NEWLINE(str) *(str++) = '\n';

#define JSON_DOUBLE_MAX_PRECISION 308

typedef enum JsonDataType {
  JSON_TYPE_LONG_LONG,  // 0
  JSON_TYPE_DOUBLE,     // 1
  JSON_TYPE_STRING,     // 2
  JSON_TYPE_OBJECT,     // 3
  JSON_TYPE_ARRAY,      // 4
  JSON_TYPE_NULL,       // 5
  JSON_TYPE_BOOL,       // 6
} JsonDataType;

typedef struct JsonNode JsonNode;
typedef struct JsonNode {
  JsonNode *parent;
  JsonNode *prev;
  JsonNode *next;
  JsonDataType type;
  // If array or object then their respective number of children
  // If its a string then it is the length of the string
  // Otherwise it is just 1
  uint32_t ele_count;
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

// Checks if NULL
// Frees node and all of its children and data (if there are any)
void free_json(JsonNode *root);

// Takes in a full JSON string (null terminated)
// Returns a pointer to the root JsonNode
// Returns NULL if any errors occurred
JsonNode *parse_json(char *json_string);

// Checks if a null terminated string is valid json or not
bool validate_json(char *json_string);

// Takes in a buffer and tries to dump the json to that buffer, will add a null terminator
// Returns true if the dump was successful
// If the buffer is too small it will return false
// Returns false on other failures
bool json_dump(JsonNode *root, char *buf, int buf_size);

// Takes in a buffer and tries to dump the json to that buffer with pretty printing, will add a null terminator
// Returns true if the dump was successful
// If the buffer is too small it will return false
// Returns false on other failures
bool json_pretty_print(JsonNode *root, int indent, char *buf, int buf_size);

// Gets the length of the string from dumping a json node
int json_dump_str_len(JsonNode *json_node);

// Gets the length of the string from dumping a json node with indentation and pretty printing
// Newline between all elements in objects and arrays, along with spaces after colons in k,v pairs
// No trailing new line
int json_pretty_dump_str_len(JsonNode *json_node, int indent);

// Returns node or NULL if key not found
JsonNode *json_object_get(JsonNode *object, char *key);

// Returns node or NULL if node out of bound
JsonNode *json_array_get(JsonNode *array, size_t index);

// Sets node at index to value, doesn't shift other elements
// Returns true if set was successful
// Returns false if index out of bounds
// Value should be on the heap
bool json_array_set(JsonNode *array, size_t index, JsonNode *val);

// Sets node at index to value, shifts rest of array to the right
// Returns true if set was successful
// Returns false if index out of bounds (will work if index = len(arr))
// Value should be on the heap
bool json_array_insert(JsonNode *array, size_t index, JsonNode *val);

// Appends value to the end of a json array
// Value should be on the heap
void json_array_append(JsonNode *array, size_t index, JsonNode *val);

// Sets a key, val pair in an object
// Key and value should be on the heap
void json_object_set_key_val_pair(JsonNode *object, char *key, JsonNode *val);

// Changes k,v pair in object to have a new key
// Returns changed node if original key existed
// New key should be on the heap
// Returns NULL if original key didn't exist
JsonNode *json_object_change_key(JsonNode *object, char *original, char *new);

// Changes node to have new value and type
// If value is double, long long, or bool, doesn't need to be on heap
// Otherwise value should be on the heap
void json_node_set_value(JsonNode *json_node, JsonDataType type, void *value);

// Returns new json node with provided type and value
// If value is double, long long, or bool, doesn't need to be on heap
// Otherwise value should be on the heap
// Returns null if malloc fails
JsonNode *json_new_node(JsonDataType type, void *value);

#endif