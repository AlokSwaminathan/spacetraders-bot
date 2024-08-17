#include "error.h"

#include "../util.h"
#include "util.h"

bool parse_error_response(struct JsonNode *root, struct ErrorResponse *error) {
  JSON_OBJECT_GET_SET(root, "error", root, JSON_TYPE_OBJECT);

  JSON_OBJECT_GET_SET(root, "message", error->msg, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "code", error->code, JSON_TYPE_LONG_LONG);

  struct JsonNode *error_data = json_object_get(root, "data");

  if (error_data == NULL) {
    error->data.start = NULL;
    error->data.len = 0;
    return true;
  }

  JSON_OBJECT_MAP(error_data, error->data, {
    error->data.start[i].key = curr->key;
    if (curr->type == JSON_TYPE_STRING) {
      error->data.start[i].msgs = malloc(sizeof(char *));
      MALLOC_CHECK(error->data.start[i].msgs);
      error->data.start[i].msgs_len = 1;
      error->data.start[i].msgs[0] = curr->value_str;
    } else if (curr->type == JSON_TYPE_ARRAY) {
      error->data.start[i].msgs_len = curr->ele_count;
      error->data.start[i].msgs = malloc(sizeof(char *) * curr->ele_count);
      MALLOC_CHECK(error->data.start[i].msgs);
      struct JsonNode *ele = curr->child;
      for (int j = 0; ele != NULL; j++, ele = ele->next) {
        error->data.start[i].msgs[j] = ele->value_str;
      }
    } else {
      return false;
    }
  });

  return true;
}

void free_error_response(struct ErrorResponse *error) {
  free(error->msg);
  if (error->data.start == NULL) return;
  for (size_t i = 0; i < error->data.len; i++) {
    for (size_t j = 0; j < error->data.start[i].msgs_len; j++){
      free(error->data.start[i].msgs[j]);
    }
    free(error->data.start[i].msgs);
  }
  free(error->data.start);
};
