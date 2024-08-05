#include "json.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char *json_dump_node(JsonNode *json_node, char *buf);
char *json_dump_node_pretty(JsonNode *json_node, int indent, char *buf);

char JSON_NULL_STR[] = {'n', 'u', 'l', 'l'};
char JSON_TRUE_STR[] = {'t', 'r', 'u', 'e'};
char JSON_FALSE_STR[] = {'f', 'a', 'l', 's', 'e'};

void free_json(JsonNode *json_node) {
  if (json_node == NULL) return;
  free(json_node->key);
  if (json_node->type == JSON_TYPE_STRING) {
    free(json_node->value_str);
  }
  if (json_node->type == JSON_TYPE_OBJECT || json_node->type == JSON_TYPE_ARRAY) {
    JsonNode *curr = json_node->child;
    JsonNode *next;
    for (uint32_t i = 0; i < json_node->ele_count; i++) {
      next = curr->next;
      free_json(curr);
      curr = next;
    }
  }
  free(json_node);
}

JsonNode *parse_json(char *json_string) {
  if (json_string == NULL) return NULL;

  JsonNode *root = json_node_default();
  if (root == NULL) return NULL;

  JSON_READ_WHITESPACE(json_string);

  json_string = json_parse_symbol(json_string, root);
  if (json_string == NULL) {
    free(root);
    return NULL;
  }

  JSON_READ_WHITESPACE(json_string);

  if (*json_string != '\0') {
    free_json(root);
    return NULL;
  }
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
  json_node->ele_count = 0;

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

    if (ret == NULL) {
      free(curr);
      return NULL;
    }
    json_string = ret;

    curr->prev = prev;
    if (prev != NULL)
      prev->next = curr;
    else
      json_node->child = curr;
    prev = curr;

    json_node->ele_count++;

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
  JSON_READ_WHITESPACE(json_string);
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
  json_node->ele_count = 0;
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
    if (ret == NULL) {
      free(curr);
      return NULL;
    }

    curr->prev = prev;
    if (prev != NULL)
      prev->next = curr;
    else
      json_node->child = curr;
    prev = curr;

    json_node->ele_count++;
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
  json_node->val_strlen = len;
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
      d += digit * pow(10, -digits);
    } else {
      l *= 10;
      l += digit;
    }
  }
  if (digits == 0) return NULL;

  if (!is_double) d = l;

  if (d < 1)
    json_node->val_strlen = 1;
  else
    json_node->val_strlen = log10(d) + 1;

  if (is_negative && d != 0.0) {
    l *= -1;
    d *= -1;
    json_node->val_strlen++;
  }

  if (is_double) {
    json_node->val_strlen += digits;
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
    json_node->value_void = NULL;
  } else {
    return NULL;
  }
  for (int i = 0; i < sz; i++) {
    if (*(json_string + i) != literal_str[i]) return NULL;
  }
  json_node->val_strlen = sz;
  return json_string + sz;
}

JsonNode *json_node_default(void) {
  JsonNode *node = (JsonNode *)malloc(sizeof(JsonNode));
  if (node == NULL) return NULL;
  node->parent = NULL;
  node->prev = NULL;
  node->next = NULL;
  node->value_void = NULL;
  node->ele_count = 0;
  node->is_array_ele = false;
  node->key = NULL;
  return node;
}

bool json_dump(JsonNode *root, char *buf, int buf_size) {
  int str_size = json_node_str_len(root);
  if (buf_size < str_size + 1) return false;
  char *end = json_dump_node(root, buf);
  *end = '\0';
  return true;
}

char *json_dump_node(JsonNode *json_node, char *buf) {
  if (json_node->key != NULL) {
    int key_len = strlen(json_node->key);
    buf[0] = JSON_D_QUOTE;
    memcpy(buf + 1, json_node->key, key_len);
    buf[key_len + 1] = JSON_D_QUOTE;
    buf[key_len + 2] = JSON_COLON;
    buf += key_len + 3;
  }
  switch (json_node->type) {
    case JSON_TYPE_OBJECT:
    case JSON_TYPE_ARRAY: {
      char start = JSON_L_BRACKET;
      char end = JSON_R_BRACKET;
      if (json_node->type == JSON_TYPE_OBJECT) {
        start = JSON_LC_BRACKET;
        end = JSON_RC_BRACKET;
      }
      buf[0] = start;
      JsonNode *curr = json_node->child;
      while (curr != NULL) {
        buf = json_dump_node(curr, buf + 1);
        buf[0] = JSON_COMMA;
        curr = curr->next;
      }
      buf[0] = end;
      return buf+1;
      break;
    }
    case JSON_TYPE_STRING:
      buf[0] = JSON_D_QUOTE;
      memcpy(buf + 1, json_node->value_str, json_node->val_strlen);
      buf[json_node->val_strlen + 1] = JSON_D_QUOTE;
      return buf + json_node->val_strlen + 2;
      break;
    case JSON_TYPE_LONG_LONG:
      snprintf(buf, json_node->val_strlen + 1, "%lld", json_node->value_ll);
      return buf + json_node->val_strlen;
      break;
    case JSON_TYPE_DOUBLE: {
      int after_dec = json_node->val_strlen - 1;
      double d = json_node->value_double;
      if (d < 0) {
        d *= -1;
        after_dec--;
      }
      if (d >= 1) after_dec -= (int)log10(d)+1;
      else after_dec--;
      snprintf(buf, json_node->val_strlen + 1, "%.*f", after_dec, json_node->value_double);
      return buf + json_node->val_strlen;
      break;
    }
    case JSON_TYPE_NULL:
      memcpy(buf, JSON_NULL_STR, sizeof(JSON_NULL_STR));
      return buf + sizeof(JSON_TRUE_STR);
      break;
    case JSON_TYPE_BOOL:
      if (json_node->value_bool) {
        memcpy(buf, JSON_TRUE_STR, sizeof(JSON_TRUE_STR));
        return buf + sizeof(JSON_TRUE_STR);
      } else {
        memcpy(buf, JSON_FALSE_STR, sizeof(JSON_FALSE_STR));
        return buf + sizeof(JSON_FALSE_STR);
      }
      break;
  }
  return NULL;
}

bool json_dump_pretty(JsonNode *root, int indent, char *buf, int buf_size) {
  int str_size = json_node_pretty_str_len(root, indent);
  if (buf_size < str_size) return false;
  return true;
}

int json_node_str_len(JsonNode *json_node) {
  if (json_node == NULL) return 0;
  int len = 0;
  switch (json_node->type) {
    case JSON_TYPE_OBJECT:
      len += (json_node->ele_count);
    case JSON_TYPE_ARRAY:
      len += (json_node->ele_count - 1);
      len += 2;
      JsonNode *curr = json_node->child;
      while (curr != NULL) {
        len += json_node_str_len(curr);
        curr = curr->next;
      }
      break;
    case JSON_TYPE_STRING:
      len += 2;
    case JSON_TYPE_LONG_LONG:
    case JSON_TYPE_DOUBLE:
    case JSON_TYPE_BOOL:
    case JSON_TYPE_NULL:
      len = json_node->val_strlen;
      break;
  }
  if (json_node->key != NULL) {
    len += strlen(json_node->key) + 2;
  }
  return len;
}

int json_node_pretty_str_len(JsonNode *json_node, int indent) {
  if (json_node == NULL) return 0;
}