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
#define JSON_IS_NUM(c) (c >= '0' && c <= '9')
#define JSON_IS_NUM_START(c) (JSON_IS_NUM(c) || c == '-')
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
#define JSON_DOUBLE_MIN_NORMAL 1e-5
#define JSON_DOUBLE_MAX_NORMAL 1e5
#define JSON_DOUBLE_COMPARISON_TOLERANCE 1.1
#define JSON_IS_EXPONENT_START(c) (c == 'e' || c == 'E')

#define JSON_MALLOC_CHECK(ptr)                                \
  {                                                           \
    if (ptr == NULL) {                                        \
      fprintf(stderr, "Malloc failure, exitting program..."); \
      exit(1);                                                \
    }                                                         \
  }

enum JsonDataType {
  JSON_TYPE_LONG_LONG,  // 0
  JSON_TYPE_DOUBLE,     // 1
  JSON_TYPE_STRING,     // 2
  JSON_TYPE_OBJECT,     // 3
  JSON_TYPE_ARRAY,      // 4
  JSON_TYPE_NULL,       // 5
  JSON_TYPE_BOOL,       // 6
};

struct JsonNode {
  struct JsonNode *parent;
  struct JsonNode *prev;
  struct JsonNode *next;
  enum JsonDataType type;
  // If array or object then their respective number of children
  // If its a string then it is the length of the string
  // Otherwise it is just 1
  uint32_t ele_count;
  bool is_array_ele;
  char *key;
  union {
    struct JsonNode *child;
    bool value_bool;
    long long value_ll;
    double value_double;
    char *value_str;
    void *value_void;
  };
};

// Checks if NULL
// Frees node and all of its children and data (if there are any)
void free_json(struct JsonNode *root);

// Takes in a full JSON string (null terminated)
// Returns a pointer to the root JsonNode
// Returns NULL if the parsing failed (invalid json)
struct JsonNode *parse_json(char *json_string);

// Checks if a null terminated string is valid json or not
bool validate_json(char *json_string);

// Takes in a buffer and tries to dump the json to that buffer, will add a null terminator
// Returns true if the dump was successful
// If the buffer is too small it will return false
// Returns false on other failures
bool json_dump(struct JsonNode *root, char *buf, int buf_size);

// Takes in a buffer and tries to dump the json to that buffer with pretty printing, will add a null terminator
// Returns true if the dump was successful
// If the buffer is too small it will return false
// Returns false on other failures
bool json_pretty_print(struct JsonNode *root, int indent, char *buf, int buf_size);

// Gets the length of the string from dumping a json node
int json_dump_str_len(struct JsonNode *json_node);

// Gets the length of the string from dumping a json node with indentation and pretty printing
// Newline between all elements in objects and arrays, along with spaces after colons in k,v pairs
// No trailing new line
int json_pretty_dump_str_len(struct JsonNode *json_node, int indent);

// Returns node or NULL if key not found
struct JsonNode *json_object_get(struct JsonNode *object, char *key);

// Returns node or NULL if node out of bound
struct JsonNode *json_array_get(struct JsonNode *array, size_t index);

// Sets node at index to value, doesn't shift other elements
// Returns old JsonNode if successful
// Returns NULL if index out of bounds (index >= array->ele_count)
struct JsonNode *json_array_set(struct JsonNode *array, size_t index, struct JsonNode *val);

// Sets node at index to value, shifts rest of array to the right including arr[index]
// Returns true if set was successful
// Returns false if index out of bounds (will work if index = len(arr))
bool json_array_insert(struct JsonNode *array, size_t index, struct JsonNode *val);

// Appends value to the end of a json array
void json_array_append(struct JsonNode *array, struct JsonNode *val);

// Sets a key, val pair in an object
// Clones key
// Returns pointer to previous node with that key, or NULL if the key is new
struct JsonNode *json_object_set_kv(struct JsonNode *object, char *key, struct JsonNode *val);

// Changes k,v pair in object to have a new key
// Clones key
// Returns true if change worked
// Returns false if original key didn't exist or if the new key was already taken
bool json_object_change_key(struct JsonNode *object, char *original, char *new);

// Changes node to have new value and type
// Clones value if it's a string
// Double values should not be inf or nan
void json_node_set_value(struct JsonNode *json_node, enum JsonDataType type, void *value);

// Returns new json node with provided type and value
// This is the only way that JsonNodes should be made
// Double values should not be inf or nan
struct JsonNode *json_new_node(enum JsonDataType type, void *value);

#endif