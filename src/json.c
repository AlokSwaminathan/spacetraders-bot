#include "json.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// "private" functions
char *json_parse_object(char *json_string, JsonNode *json_node);
char *json_parse_kv_pair(char *json_string, JsonNode *json_node);
char *json_parse_key(char *json_string, JsonNode *json_node);
char *json_parse_string(char *json_string, JsonNode *json_node);
char *json_parse_num(char *json_string, JsonNode *json_node);
char *json_parse_array(char *json_string, JsonNode *json_node);
char *json_parse_literal(char *json_string, JsonNode *json_node);
char *json_parse_symbol(char *json_string, JsonNode *json_node);
JsonNode *json_node_default(void);

char JSON_NULL_STR[] = {'n', 'u', 'l', 'l'};
char JSON_TRUE_STR[] = {'t', 'r', 'u', 'e'};
char JSON_FALSE_STR[] = {'f', 'a', 'l', 's', 'e'};

void free_json(JsonNode *json_root) {
  if (json_root == NULL) return;

  if (json_root->type != JSON_TYPE_ROOT) {
    return;
  }

  for (JsonNode *curr = json_root->child; curr->next != NULL; curr = curr->next) {
    if (curr->type == JSON_TYPE_OBJECT) {
      curr->type = JSON_TYPE_ROOT;
      free_json(curr);
    } else {
      free(curr->value_void);
    }
  }
}

JsonNode *parse_json(char *json_string) {
  if (json_string == NULL) return NULL;

  JsonNode *root = json_node_default();
  if (root == NULL) return NULL;
  root->type = JSON_TYPE_ROOT;
  root->child = json_node_default();
  if (root->child == NULL) return NULL;
  root->child->parent = root;

  JSON_READ_WHITESPACE(json_string);

  json_string = json_parse_symbol(json_string, root->child);
  if (json_string == NULL) return NULL;

  JSON_READ_WHITESPACE(json_string);

  if (*json_string != '\0') return NULL;
  return root;
}

bool validate_json(char *json_string) {
  if (json_string == NULL) return false;

  // This is horrible inefficient and not good at all
  return parse_json(json_string) != NULL;
}

char *json_parse_symbol(char *json_string, JsonNode *json_node) {
  char c = *json_string;
  char *(*parse_func)(char *, JsonNode *);
  if (JSON_IS_NUM_START(c)) {
    parse_func = json_parse_num;
  } else if (JSON_IS_ARR_START(c)) {
    parse_func = json_parse_array;
  } else if (JSON_IS_LITERAL_START(c)) {
    parse_func = json_parse_literal;
  } else if (JSON_IS_OBJECT_START(c)) {
    parse_func = json_parse_object;
  } else if (JSON_IS_STRING_START(c)) {
    parse_func = json_parse_string;
  } else {
    return NULL;
  }
  return parse_func(json_string, json_node);
}

char *json_parse_object(char *json_string, JsonNode *json_node) {
  // Set up node
  json_node->type = JSON_TYPE_OBJECT;
  json_node->value_len = 0;

  json_string++;
  JsonNode *prev = NULL;
  JsonNode *curr = NULL;
  JSON_READ_WHITESPACE(json_string);
  if (*json_string == JSON_RC_BRACKET) return json_string + 1;
  while (true) {
    JSON_READ_WHITESPACE(json_string);
    if (*json_string != JSON_D_QUOTE) return NULL;

    curr = json_node_default();
    if (curr == NULL) {
      return NULL;
    }

    curr->parent = json_node;
    curr->is_array_ele = false;

    char *ret = json_parse_kv_pair(json_string, curr);

    if (ret == NULL) return NULL;
    json_string = ret;

    curr->prev = prev;
    if (prev != NULL)
      prev->next = curr;
    else
      json_node->child = curr;
    prev = curr;

    json_node->value_len++;

    JSON_READ_WHITESPACE(json_string);
    if (*json_string == JSON_RC_BRACKET) return json_string + 1;
    if (*json_string != JSON_COMMA) return NULL;

    json_string++;
  }
}

char *json_parse_kv_pair(char *json_string, JsonNode *json_node) {
  json_string = json_parse_key(json_string, json_node);
  if (json_string == NULL) return NULL;
  JSON_READ_WHITESPACE(json_string);
  if (*json_string != JSON_COLON) {
    return NULL;
  }
  json_string++;
  return json_parse_symbol(json_string, json_node);
}

char *json_parse_key(char *json_string, JsonNode *json_node) {
  json_string = json_parse_string(json_string, json_node);
  if (json_string == NULL) return NULL;
  json_node->key = json_node->value_str;
  json_node->value_str = NULL;
  return json_string;
}

char *json_parse_array(char *json_string, JsonNode *json_node) {
  // Set up node
  json_node->type = JSON_TYPE_ARRAY;
  json_node->value_len = 0;
  json_node->child = NULL;

  json_string++;
  JsonNode *prev = NULL;
  JsonNode *curr = NULL;
  while (true) {
    JSON_READ_WHITESPACE(json_string);
    if (*json_string == '\0') return NULL;

    curr = json_node_default();
    if (curr == NULL) {
      return NULL;
    }

    curr->is_array_ele = true;
    curr->key = NULL;
    curr->parent = json_node;

    char *ret = json_parse_symbol(json_string, curr);
    if (ret == NULL) return NULL;

    curr->prev = prev;
    if (prev != NULL)
      prev->next = curr;
    else
      json_node->child = curr;
    prev = curr;

    json_node->value_len++;
    json_string = ret;

    JSON_READ_WHITESPACE(json_string);
    if (*json_string == JSON_R_BRACKET) return json_string + 1;
    if (*json_string != JSON_COMMA) return NULL;

    json_string++;
  }
}

char *json_parse_string(char *json_string, JsonNode *json_node) {
  json_string++;
  // Get length of string
  int len = 0;
  // Store if return is due to finding end of string or just because of a null terminator
  bool valid_ret = false;
  for (char *start = json_string; *start != '\0'; start++, len++) {
    if (*start == JSON_SPECIAL_START) {
      start++;
      if (*start == '\0') return NULL;
      switch (*start) {
        case JSON_SPECIAL_BACKSLASH:
        case JSON_SPECIAL_CARRIAGE_RET:
        case JSON_SPECIAL_FORMFEED:
        case JSON_SPECIAL_FSLASH:
        case JSON_SPECIAL_NEWLINE:
        case JSON_SPECIAL_QUOTE:
        case JSON_SPECIAL_TAB:
        case JSON_SPECIAL_BACKSPACE:
          break;
        default:
          return NULL;
      }
      continue;
    }
    if (*start == JSON_D_QUOTE) {
      valid_ret = true;
      break;
    }
  }
  if (!valid_ret) return NULL;

  // Actually parse the string
  char *str = malloc(sizeof(char) * (len + 1));
  for (int i = 0; i < len; i++, json_string++) {
    char c = *json_string;
    if (c == JSON_SPECIAL_START) {
      json_string++;
      c = *json_string;
      switch (c) {
        case JSON_SPECIAL_BACKSLASH:
          str[i] = '\\';
          break;
        case JSON_SPECIAL_CARRIAGE_RET:
          str[i] = '\r';
          break;
        case JSON_SPECIAL_FORMFEED:
          str[i] = '\f';
          break;
        case JSON_SPECIAL_FSLASH:
          str[i] = '/';
          break;
        case JSON_SPECIAL_NEWLINE:
          str[i] = '\n';
          break;
        case JSON_SPECIAL_QUOTE:
          str[i] = '\"';
          break;
        case JSON_SPECIAL_TAB:
          str[i] = '\t';
          break;
        case JSON_SPECIAL_BACKSPACE:
          str[i] = '\b';
          break;
        default:
          return NULL;
      }
      continue;
    }
    str[i] = c;
  }
  str[len] = '\0';
  json_node->value_len = 1;
  json_node->type = JSON_TYPE_STRING;
  json_node->value_str = str;
  return json_string + 1;
}

char *json_parse_num(char *json_string, JsonNode *json_node) {
  long long l = 0;
  double d = 0.0;
  int digits = 0;
  bool is_double = false;
  bool is_negative = false;
  if (*json_string == '-') {
    is_negative = true;
    json_string++;
  }
  for (; JSON_IS_NUM(*json_string); json_string++, digits++) {
    if (*json_string == JSON_DECIMAL) {
      if (is_double) return NULL;
      is_double = true;
      d = l;
      digits = 0;
      continue;
    }
    uint8_t digit = *json_string - '0';
    if (is_double) {
      d += digit*pow(10, -digits);
    } else {
      l *= 10;
      l += digit;
    }
  }
  if (digits == 0) return NULL;
  if (is_negative) {
    l *= -1;
    d *= -1;
  }
  json_node->value_len = 1;
  if (is_double) {
    json_node->type = JSON_TYPE_DOUBLE;
    json_node->value_double = d;
  } else {
    json_node->type = JSON_TYPE_LONG_LONG;
    json_node->value_ll = l;
  }
  return json_string;
}

char *json_parse_literal(char *json_string, JsonNode *json_node) {
  char *literal_str;
  int sz;
  char c = *json_string;
  if (JSON_IS_TRUE_START(c)) {
    literal_str = JSON_TRUE_STR;
    sz = sizeof(JSON_TRUE_STR);
    json_node->type = JSON_TYPE_BOOL;
    json_node->value_bool = true;
  } else if (JSON_IS_FALSE_START(c)) {
    literal_str = JSON_FALSE_STR;
    sz = sizeof(JSON_FALSE_STR);
    json_node->type = JSON_TYPE_BOOL;
    json_node->value_bool = false;
  } else if (JSON_IS_NULL_START(c)) {
    literal_str = JSON_NULL_STR;
    sz = sizeof(JSON_NULL_STR);
    json_node->type = JSON_TYPE_NULL;
  } else {
    return NULL;
  }
  for (int i = 0; i < sz; i++) {
    if (*(json_string + i) != literal_str[i]) return NULL;
  }
  return json_string + sz;
}

JsonNode *json_node_default(void) {
  JsonNode *node = (JsonNode *)malloc(sizeof(JsonNode));
  if (node == NULL) return NULL;
  node->parent = NULL;
  node->prev = NULL;
  node->next = NULL;
  node->value_void = NULL;
  node->value_len = 1;
  node->is_array_ele = false;
  node->key = NULL;
  return node;
}
