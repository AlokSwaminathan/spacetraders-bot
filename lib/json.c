#include "json.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Private functions
static char *json_parse_object(char *json_string, struct JsonNode *json_node);
static char *json_parse_kv_pair(char *json_string, struct JsonNode *json_node);
static char *json_parse_key(char *json_string, struct JsonNode *json_node);
static char *json_parse_string(char *json_string, struct JsonNode *json_node);
static char *json_parse_num(char *json_string, struct JsonNode *json_node);
static char *json_parse_array(char *json_string, struct JsonNode *json_node);
static char *json_parse_literal(char *json_string, struct JsonNode *json_node);
static char *json_parse_symbol(char *json_string, struct JsonNode *json_node);

static int json_str_escaped_len(char *str);
static int json_double_len_as_str(double d);
static int json_long_long_len_as_str(long long ll);

static struct JsonNode *json_node_default(void);

static char *json_dump_node(struct JsonNode *json_node, char *buf, int indent, int layer);

static int json_node_str_len(struct JsonNode *json_node, int indent, int layer);

static void *json_copy_data(void *data, size_t size);

static char JSON_NULL_STR[] = {'n', 'u', 'l', 'l'};
static char JSON_TRUE_STR[] = {'t', 'r', 'u', 'e'};
static char JSON_FALSE_STR[] = {'f', 'a', 'l', 's', 'e'};
static char JSON_DOUBLE_LENGTH_STORAGE[JSON_DOUBLE_MAX_PRECISION * 2 + 5];

void free_json(struct JsonNode *json_node) {
  if (json_node == NULL) return;
  free(json_node->key);
  if (json_node->type == JSON_TYPE_STRING) {
    free(json_node->value_str);
  }
  if (json_node->type == JSON_TYPE_OBJECT || json_node->type == JSON_TYPE_ARRAY) {
    struct JsonNode *curr = json_node->child;
    struct JsonNode *next;
    for (uint32_t i = 0; i < json_node->ele_count; i++) {
      next = curr->next;
      free_json(curr);
      curr = next;
    }
  }
  free(json_node);
}

struct JsonNode *parse_json(char *json_string) {
  if (json_string == NULL) return NULL;

  struct JsonNode *root = json_node_default();

  JSON_STRING_READ_WHITESPACE(json_string);

  json_string = json_parse_symbol(json_string, root);
  if (json_string == NULL) {
    free_json(root);
    return NULL;
  }

  JSON_STRING_READ_WHITESPACE(json_string);

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

static char *json_parse_symbol(char *json_string, struct JsonNode *json_node) {
  char c = *json_string;
  char *(*parse_func)(char *, struct JsonNode *);
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

static char *json_parse_object(char *json_string, struct JsonNode *json_node) {
  // Set up node
  json_node->type = JSON_TYPE_OBJECT;
  json_node->ele_count = 0;

  json_string++;
  struct JsonNode *prev = NULL;
  struct JsonNode *curr = NULL;
  JSON_STRING_READ_WHITESPACE(json_string);
  if (*json_string == JSON_RC_BRACKET) return json_string + 1;
  while (true) {
    JSON_STRING_READ_WHITESPACE(json_string);
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

    JSON_STRING_READ_WHITESPACE(json_string);
    if (*json_string == JSON_RC_BRACKET) break;
    if (*json_string != JSON_COMMA) return NULL;

    json_string++;
  }
  char *keys[json_node->ele_count];
  curr = json_node->child;
  for (int i = 0; curr != NULL; i++, curr = curr->next) {
    keys[i] = curr->key;
    for (int j = 0; j < i; j++) {
      if (strcmp(keys[j], keys[i]) == 0) {
        return NULL;
      }
    }
  }
  return json_string + 1;
}

static char *json_parse_kv_pair(char *json_string, struct JsonNode *json_node) {
  json_string = json_parse_key(json_string, json_node);
  if (json_string == NULL) return NULL;
  JSON_STRING_READ_WHITESPACE(json_string);
  if (*json_string != JSON_COLON) {
    return NULL;
  }
  json_string++;
  JSON_STRING_READ_WHITESPACE(json_string);
  return json_parse_symbol(json_string, json_node);
}

static char *json_parse_key(char *json_string, struct JsonNode *json_node) {
  json_string = json_parse_string(json_string, json_node);
  if (json_string == NULL) return NULL;
  json_node->key = json_node->value_str;
  json_node->value_str = NULL;
  return json_string;
}

static char *json_parse_array(char *json_string, struct JsonNode *json_node) {
  // Set up node
  json_node->type = JSON_TYPE_ARRAY;
  json_node->ele_count = 0;
  json_node->child = NULL;

  json_string++;

  JSON_STRING_READ_WHITESPACE(json_string);
  if (*json_string == JSON_R_BRACKET) return json_string + 1;

  struct JsonNode *prev = NULL;
  struct JsonNode *curr = NULL;
  while (true) {
    JSON_STRING_READ_WHITESPACE(json_string);
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

    JSON_STRING_READ_WHITESPACE(json_string);
    if (*json_string == JSON_R_BRACKET) return json_string + 1;
    if (*json_string != JSON_COMMA) return NULL;

    json_string++;
  }
}

static char *json_parse_string(char *json_string, struct JsonNode *json_node) {
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
  JSON_MALLOC_CHECK(str);
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
  json_node->ele_count = len;
  json_node->type = JSON_TYPE_STRING;
  json_node->value_str = str;
  return json_string + 1;
}

static char *json_parse_num(char *json_string, struct JsonNode *json_node) {
  long long l = 0;
  double d = 0.0;

  bool hit_decimal = false;
  bool is_double = false;
  bool is_negative = false;

  if (*json_string == '-') {
    is_negative = true;
    json_string++;
  }
  if (*json_string == JSON_DECIMAL) {
    return NULL;
  }

  size_t digits_whole = 0;
  size_t digits_decimal = 0;

  json_string--;
  while (json_string++) {
    char c = *json_string;
    if (JSON_IS_NUM(c)) {
      if (hit_decimal) {
        d += (c - '0') * pow(10, -(int)(digits_decimal + 1));
        digits_decimal++;
        continue;
      }
      digits_whole++;
      if (is_double) {
        d = (d * 10) + (c - '0');
      } else {
        if ((double)l * 10 + (c - '0') > (double)__LONG_LONG_MAX__ / JSON_DOUBLE_COMPARISON_TOLERANCE) {
          is_double = true;
          d = ((double)l * 10) + (c - '0');
        } else {
          l = (l * 10) + (c - '0');
        }
      }
    } else if (c == JSON_DECIMAL) {
      hit_decimal = true;
      if (!is_double) {
        is_double = true;
        d = l;
      }
    } else if (JSON_IS_EXPONENT_START(c)) {
      if (digits_decimal == 0) return NULL;
      if (l == 0 && d == 0.0) {
      } else if (is_double) {
        if (d < 1.0 || d >= 10.0) return NULL;
      } else {
        if (l < 1 || l >= 10) return NULL;
      }
      int sign;
      if (*(json_string + 1) == '+') {
        sign = 1;
        json_string++;
      } else if (*(json_string + 1) == '-') {
        sign = -1;
        json_string++;
      } else if (isdigit(*(json_string + 1))) {
        sign = 1;
      } else {
        return NULL;
      }
      int exponent = 0;
      for (json_string++; isdigit(*json_string); json_string++) {
        exponent = (exponent * 10) + (*json_string - '0');
      }
      if (is_double) {
        d = d * pow(10, sign * exponent);
      } else {
        d = l * pow(10, sign * exponent);
        if (d > __LONG_LONG_MAX__ / JSON_DOUBLE_COMPARISON_TOLERANCE) {
          is_double = true;
        } else {
          l = d;
        }
      }
      break;
    } else {
      break;
    }
  }
  if (hit_decimal && digits_decimal == 0) return NULL;
  if (digits_whole == 0) return NULL;

  if (is_negative) {
    l *= -1;
    d *= -1;
  }

  json_node->ele_count = 1;

  if (is_double) {
    json_node->type = JSON_TYPE_DOUBLE;
    json_node->value_double = d;
  } else {
    json_node->type = JSON_TYPE_LONG_LONG;
    json_node->value_ll = l;
  }
  return json_string;
}

static char *json_parse_literal(char *json_string, struct JsonNode *json_node) {
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
  json_node->ele_count = 1;
  return json_string + sz;
}

static struct JsonNode *json_node_default(void) {
  struct JsonNode *node = (struct JsonNode *)malloc(sizeof(struct JsonNode));
  JSON_MALLOC_CHECK(node);
  node->parent = NULL;
  node->prev = NULL;
  node->next = NULL;
  node->value_void = NULL;
  node->ele_count = 0;
  node->is_array_ele = false;
  node->key = NULL;
  return node;
}

bool json_dump(struct JsonNode *root, char *buf, int buf_size) {
  if (root == NULL) return false;
  int str_size = json_node_str_len(root, -1, 0);
  if (buf_size < str_size + 1) return false;
  char *end = json_dump_node(root, buf, -1, 0);
  *end = '\0';
  return true;
}

bool json_pretty_print(struct JsonNode *root, int indent, char *buf, int buf_size) {
  if (root == NULL) return false;
  int str_size = json_node_str_len(root, indent, 0);
  if (buf_size < str_size + 1) return false;
  char *end = json_dump_node(root, buf, indent, 0);
  *end = '\0';
  return true;
}

static char *json_dump_node(struct JsonNode *json_node, char *buf, int indent, int layer) {
  if (indent > -1) JSON_STRING_PAD_WHITESPACE(buf, layer * indent);
  if (json_node->key != NULL) {
    struct JsonNode key_node = *json_node;
    key_node.value_str = key_node.key;
    key_node.key = NULL;
    key_node.ele_count = strlen(key_node.value_str);
    key_node.type = JSON_TYPE_STRING;
    buf = json_dump_node(&key_node, buf, -1, 0);
    *buf = JSON_COLON;
    buf++;
    if (indent > -1) JSON_STRING_PAD_WHITESPACE(buf, 1);
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
      buf++;
      if (indent > -1) {
        JSON_STRING_ADD_NEWLINE(buf);
      }
      struct JsonNode *curr = json_node->child;
      while (curr != NULL) {
        buf = json_dump_node(curr, buf, indent, layer + 1);
        if (curr->next != NULL) {
          buf[0] = JSON_COMMA;
          buf++;
        }
        if (indent > -1) {
          JSON_STRING_ADD_NEWLINE(buf);
        }
        curr = curr->next;
      }
      if (indent > -1) JSON_STRING_PAD_WHITESPACE(buf, layer * indent);
      buf[0] = end;
      return buf + 1;
      break;
    }
    case JSON_TYPE_STRING:
      buf[0] = JSON_D_QUOTE;
      buf++;
      int len = json_node->ele_count;
      for (int i = 0; i < len; i++) {
        char c = json_node->value_str[i];
        if (JSON_IS_SPECIAL_CHAR(c)) {
          buf[0] = JSON_BACKSLASH;
          buf++;
          if (c == JSON_NEWLINE)
            buf[0] = JSON_SPECIAL_NEWLINE;
          else if (c == JSON_D_QUOTE)
            buf[0] = JSON_SPECIAL_QUOTE;
          else if (c == JSON_BACKSLASH)
            buf[0] = JSON_SPECIAL_BACKSLASH;
          else if (c == JSON_FSLASH)
            buf[0] = JSON_SPECIAL_FSLASH;
          else if (c == JSON_FORMFEED)
            buf[0] = JSON_SPECIAL_FORMFEED;
          else if (c == JSON_CARRIAGE_RET)
            buf[0] = JSON_SPECIAL_CARRIAGE_RET;
          else if (c == JSON_TAB)
            buf[0] = JSON_SPECIAL_TAB;
          else if (c == JSON_BACKSPACE)
            buf[0] = JSON_SPECIAL_BACKSPACE;
          buf++;
        } else {
          buf[0] = c;
          buf++;
        }
      }
      buf[0] = JSON_D_QUOTE;
      return buf + 1;
      break;
    case JSON_TYPE_LONG_LONG: {
      int len = json_long_long_len_as_str(json_node->value_ll);
      snprintf(buf, len + 1, "%lld", json_node->value_ll);
      return buf + len;
      break;
    }
    case JSON_TYPE_DOUBLE: {
      int len = json_double_len_as_str(json_node->value_double);
      double d_abs = fabs(json_node->value_double);
      if (d_abs > JSON_DOUBLE_MIN_NORMAL && d_abs < JSON_DOUBLE_MAX_NORMAL) {
        snprintf(buf, len + 1, "%.*f", JSON_DOUBLE_MAX_PRECISION, json_node->value_double);
      } else {
        snprintf(JSON_DOUBLE_LENGTH_STORAGE, sizeof(JSON_DOUBLE_LENGTH_STORAGE), "%.*e", __DECIMAL_DIG__, json_node->value_double);
        int full_len = strlen(JSON_DOUBLE_LENGTH_STORAGE);
        char *end = JSON_DOUBLE_LENGTH_STORAGE + full_len - 1;
        char *curr = end;
        for (; isdigit(*curr); curr--);
        char *exp = curr - 1;
        int exp_zeros = 0;
        int significand_zeros = 0;
        for (curr++; *curr == '0'; curr++, exp_zeros++);
        char *power = curr;
        for (curr = exp - 1; *curr == '0' || *curr == JSON_DECIMAL; curr--, significand_zeros++);
        int starting_amt = 1 + (curr - JSON_DOUBLE_LENGTH_STORAGE);
        memcpy(buf, JSON_DOUBLE_LENGTH_STORAGE, starting_amt);
        buf[starting_amt] = 'e';
        if (d_abs < 1) {
          buf[starting_amt + 1] = '-';
        } else {
          buf[starting_amt + 1] = '+';
        }
        memcpy(buf + starting_amt + 2, power, (end + 1) - power);
      }
      return buf + len;
      break;
    }
    case JSON_TYPE_NULL:
      memcpy(buf, JSON_NULL_STR, sizeof(JSON_NULL_STR));
      return buf + sizeof(JSON_NULL_STR);
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

static int json_node_str_len(struct JsonNode *json_node, int indent, int layer) {
  if (json_node == NULL) return 0;
  int len = 0;
  if (indent > -1) {
    len += layer * indent;
  }
  switch (json_node->type) {
    case JSON_TYPE_OBJECT:
      len += (json_node->ele_count);
    case JSON_TYPE_ARRAY:
      len += (json_node->ele_count - 1);
      len += 2;
      if (indent > -1) {
        len++;                  // Add new line after starting bracket
        len += layer * indent;  // Add indent before ending bracket
      }
      struct JsonNode *curr = json_node->child;
      while (curr != NULL) {
        len += json_node_str_len(curr, indent, layer + 1);
        curr = curr->next;
        if (indent > -1) len++;
      }
      break;
    case JSON_TYPE_STRING:
      len += 2;
      len += json_str_escaped_len(json_node->value_str);
      break;
    case JSON_TYPE_LONG_LONG:
      len += json_long_long_len_as_str(json_node->value_ll);
      break;
    case JSON_TYPE_DOUBLE:
      len += json_double_len_as_str(json_node->value_double);
      break;
    case JSON_TYPE_BOOL:
      if (json_node->value_bool) {
        len += sizeof(JSON_TRUE_STR);
      } else {
        len += sizeof(JSON_FALSE_STR);
      }
      break;
    case JSON_TYPE_NULL:
      len += sizeof(JSON_NULL_STR);
      break;
  }
  if (json_node->key != NULL) {
    len += 2;                // Quotes
    if (indent > -1) len++;  // Space after colon
    len += json_str_escaped_len(json_node->key);
  }
  return len;
}

static int json_double_len_as_str(double d) {
  if (d == 0.0) return 0;
  double d_abs = fabs(d);
  if (d_abs > JSON_DOUBLE_MIN_NORMAL && d_abs < JSON_DOUBLE_MAX_NORMAL) {
    snprintf(JSON_DOUBLE_LENGTH_STORAGE, sizeof(JSON_DOUBLE_LENGTH_STORAGE), "%.*f", JSON_DOUBLE_MAX_PRECISION, d);
    int len = strlen(JSON_DOUBLE_LENGTH_STORAGE);
    for (char *end = JSON_DOUBLE_LENGTH_STORAGE + len - 1; *end == '0'; end--, len--);
    if (*(JSON_DOUBLE_LENGTH_STORAGE + len - 1) == JSON_DECIMAL) len++;  // Make sure the last char isn't the decimal point
    return len;
  } else {
    snprintf(JSON_DOUBLE_LENGTH_STORAGE, sizeof(JSON_DOUBLE_LENGTH_STORAGE), "%.*e", __DECIMAL_DIG__, d);
    int len = strlen(JSON_DOUBLE_LENGTH_STORAGE);
    char *end = JSON_DOUBLE_LENGTH_STORAGE + len - 1;
    char *curr = end;
    for (; isdigit(*curr); curr--);
    char *exp = curr - 1;
    int exp_zeros = 0;
    int significand_zeros = 0;
    for (curr++; *curr == '0'; curr++, exp_zeros++);
    for (exp--; *exp == '0' || *exp == JSON_DECIMAL; exp--, significand_zeros++);
    len -= significand_zeros + exp_zeros;
    return len;
  }
}

static int json_long_long_len_as_str(long long ll) {
  if (ll == 0) return 1;

  int len = 0;

  if (ll < 0) {
    len++;
    ll *= -1;
  }
  while (ll > 0) {
    ll /= 10;
    len++;
  }
  return len;
}

static int json_str_escaped_len(char *str) {
  int len = 0;
  for (char *c = str; *c != '\0'; c++) {
    if (JSON_IS_SPECIAL_CHAR(*c)) len++;
    len++;
  }
  return len;
}

int json_dump_str_len(struct JsonNode *root) {
  return json_node_str_len(root, INT32_MIN, 0) + 1;
}

int json_pretty_print_str_len(struct JsonNode *root, int indent) {
  return json_node_str_len(root, indent, 0) + 1;
}

// Helper functions

struct JsonNode *json_object_get(struct JsonNode *object, char *key) {
  if (object == NULL || object->type != JSON_TYPE_OBJECT) return NULL;
  struct JsonNode *curr = object->child;
  while (curr != NULL) {
    if (strcmp(curr->key, key) == 0) {
      return curr;
    }
    curr = curr->next;
  }
  return NULL;
}

struct JsonNode *json_array_get(struct JsonNode *array, size_t index) {
  if (array == NULL || array->type != JSON_TYPE_ARRAY) return NULL;
  if (index >= array->ele_count) return NULL;
  struct JsonNode *curr = array->child;
  for (size_t i = 0; i < index; i++, curr = curr->next);
  return curr;
}

struct JsonNode *json_array_set(struct JsonNode *array, size_t index, struct JsonNode *val) {
  if (index >= array->ele_count) return NULL;
  struct JsonNode *curr = json_array_get(array, index);
  val->prev = curr->prev;
  val->parent = array;
  val->next = curr->next;
  val->is_array_ele = true;
  if (val->prev != NULL) val->prev->next = val;
  if (val->next != NULL) val->next->prev = val;
  if (index == 0) array->child = val;
  return curr;
}

bool json_array_insert(struct JsonNode *array, size_t index, struct JsonNode *val) {
  if (index > array->ele_count) return false;
  if (index == array->ele_count) {
    json_array_append(array, val);
    return true;
  }
  array->ele_count++;
  if (index == 0) array->child = val;
  struct JsonNode *next = json_array_get(array, index);
  val->parent = array;
  val->is_array_ele = true;
  val->next = next;
  val->prev = next->prev;
  next->prev = val;
  if (val->prev != NULL) val->prev->next = val;
  return true;
}

void json_array_append(struct JsonNode *array, struct JsonNode *val) {
  struct JsonNode *last = json_array_get(array, array->ele_count - 1);
  array->ele_count++;
  val->next = NULL;
  val->parent = array;
  val->prev = last;
  val->is_array_ele = true;
  if (last == NULL) {
    array->child = val;
    return;
  }
  last->next = val;
}

struct JsonNode *json_object_set_kv(struct JsonNode *object, char *key, struct JsonNode *val) {
  key = (char *)json_copy_data(key, strlen(key));

  if (object->type != JSON_TYPE_OBJECT) return NULL;
  struct JsonNode *old_val = json_object_get(object, key);
  val->key = key;
  val->parent = object;
  if (old_val != NULL) {
    val->prev = old_val->prev;
    val->next = old_val->next;
    if (val->prev != NULL) val->prev->next = val;
    if (val->next != NULL) val->next->prev = val;
    if (object->child == old_val) object->child = val;
    return old_val;
  } else {
    object->ele_count++;
    struct JsonNode *old_child = object->child;
    if (old_child != NULL) old_child->prev = val;
    val->next = old_child;
    val->prev = NULL;
    object->child = val;
    return NULL;
  }
}

bool json_object_change_key(struct JsonNode *object, char *original, char *new) {
  new = (char *)json_copy_data(new, strlen(new));
  if (object->type != JSON_TYPE_OBJECT) return false;
  struct JsonNode *old_val = json_object_get(object, original);
  if (original == NULL || json_object_get(object, new)) return false;
  free(old_val->key);
  old_val->key = new;
  return true;
}

void json_node_set_value(struct JsonNode *json_node, enum JsonDataType type, void *value) {
  json_node->type = type;
  switch (type) {
    case JSON_TYPE_ARRAY:
    case JSON_TYPE_OBJECT: {
      json_node->child = (struct JsonNode *)value;

      // Calculate number of elements
      struct JsonNode *curr = json_node->child;
      json_node->ele_count = 0;
      while (curr != NULL) {
        json_node->ele_count++;
        curr = curr->next;
      }
      break;
    }
    case JSON_TYPE_BOOL:
      json_node->value_bool = *(bool *)value;
      break;
    case JSON_TYPE_DOUBLE:
      json_node->value_double = *(double *)value;
      break;
    case JSON_TYPE_LONG_LONG:
      json_node->value_ll = *(long long *)value;
      break;
    case JSON_TYPE_NULL:
      json_node->value_void = NULL;
      break;
    case JSON_TYPE_STRING:
      value = json_copy_data(value, strlen((const char *)value) + 1);
      json_node->value_str = (char *)value;
      json_node->ele_count = strlen(json_node->value_str);
      break;
  }
}

struct JsonNode *json_new_node(enum JsonDataType type, void *value) {
  struct JsonNode *node = json_node_default();
  json_node_set_value(node, type, value);
  return node;
}

void *json_copy_data(void *data, size_t size) {
  void *ret = malloc(size);
  JSON_MALLOC_CHECK(ret);
  memcpy(ret, data, size);
  return ret;
}

const char *json_type_as_str(enum JsonDataType type) {
  switch (type) {
    case JSON_TYPE_ARRAY:
      return JSON_TYPE_ARRAY_STR;
      break;
    case JSON_TYPE_BOOL:
      return JSON_TYPE_BOOL_STR;
      break;
    case JSON_TYPE_DOUBLE:
      return JSON_TYPE_DOUBLE_STR;
      break;
    case JSON_TYPE_STRING:
      return JSON_TYPE_STRING_STR;
      break;
    case JSON_TYPE_OBJECT:
      return JSON_TYPE_OBJECT_STR;
      break;
    case JSON_TYPE_NULL:
      return JSON_TYPE_NULL_STR;
      break;
    case JSON_TYPE_LONG_LONG:
      return JSON_TYPE_LONG_LONG_STR;
      break;
  };
  return NULL;
}