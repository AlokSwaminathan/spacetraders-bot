#include "util.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

time_t parse_date_time_str(char *str) {
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));

  char tz = '\0';

  int matches = sscanf(str, "%4d-%2d-%2dT%2d:%2d:%2d%c",
                       &tm.tm_year,
                       &tm.tm_mon,
                       &tm.tm_mday,
                       &tm.tm_hour,
                       &tm.tm_min,
                       &tm.tm_sec, &tz);

  if (matches < 6) {
    return (time_t)-1;
  }

  // Adjust time to fit tm structure reqs
  tm.tm_year -= 1900;
  tm.tm_mon -= 1;

  time_t t;

  // Check if UTC
  if (tz == 'Z') {
    t = timegm(&tm);
  } else {
    t = mktime(&tm);
  }

  return t;
}

void write_parsing_error(struct JsonNode *parent, struct JsonNode *child, void *accessor, enum JsonDataType expected_type) {
  size_t buf_size = 1024;
  size_t buf_cur = 0;
  char *buf = malloc(sizeof(char) * buf_size);
  MALLOC_CHECK(buf);
  if (parent->type == JSON_TYPE_ARRAY) {
    buf_cur += sprintf(buf, "[%zu]", *(size_t *)accessor);
  } else if (parent->type == JSON_TYPE_OBJECT) {
    while (buf_cur + 4 + strlen((const char *)accessor) >= buf_size) {
      char *new_buf = realloc(buf, buf_size * 2);
      if (new_buf == NULL) {
        free(buf);
        MALLOC_CHECK(new_buf);
      }
      buf = new_buf;
      buf_size *= 2;
    }
    buf_cur += sprintf(buf, "[\"%s\"]", (char *)accessor);
  } else {
    return;
  }
  struct JsonNode *curr_node = parent;
  while (curr_node->parent != NULL) {
    if (curr_node->parent->type == JSON_TYPE_ARRAY) {
      int index = json_array_index_get(curr_node);
      if (index == -1) {
        fprintf(stderr, "something has gone very wrong with JSON parsing\n");
        exit(1);
      }
      int digit_len = 0;
      {
        int temp = (index > 0) ? index : 1;
        while (temp > 0) {
          digit_len++;
          temp /= 10;
        }
      }
      while (buf_cur + 2 + digit_len >= buf_size) {
        char *new_buf = realloc(buf, buf_size * 2);
        if (new_buf == NULL) {
          free(buf);
          MALLOC_CHECK(new_buf);
        }
        buf = new_buf;
        buf_size *= 2;
      }
      buf_cur += sprintf(&buf[buf_cur], "[%d]", index);
    } else if (curr_node->parent->type == JSON_TYPE_OBJECT) {
      size_t key_len = strlen(curr_node->key);
      while (buf_cur + 4 + key_len >= buf_size) {
        char *new_buf = realloc(buf, buf_size * 2);
        if (new_buf == NULL) {
          free(buf);
          MALLOC_CHECK(new_buf);
        }
        buf = new_buf;
        buf_size *= 2;
      }
      buf_cur += sprintf(&buf[buf_cur], "[\"%s\"]", curr_node->key);
    } else {
      fprintf(stderr, "Something has gone very wrong with JSON parsing\n");
      exit(1);
    }
    curr_node = curr_node->parent;
  }

  size_t error_cur = sprintf(g_error_msg, g_error_json_name);

  while ((buf_cur--) > 0) {
    size_t copy_len = 1;
    while (buf[buf_cur] != '[') {
      buf_cur--;
      copy_len++;
    }
    if (copy_len + error_cur >= ERROR_MSG_LEN - ERROR_LEFTOVER_MSG_LEN) break;
    memcpy(&g_error_msg[error_cur], &buf[buf_cur], copy_len);
    error_cur += copy_len;
  }

  if (child == NULL) {
    sprintf(&g_error_msg[error_cur], "\nExpected a node of type %s, got NULL", json_type_as_str(expected_type));
  } else {
    sprintf(&g_error_msg[error_cur], "\nExpected a node of type %s, got a node of type %s", json_type_as_str(expected_type), json_type_as_str(child->type));
  }
}