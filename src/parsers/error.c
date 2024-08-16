#include "error.h"

#include "util.h"

bool parse_error_response(JsonNode *root, ErrorResponse *error) {
  error->root = root;

  JSON_OBJECT_GET_SET(root, "error", root, JSON_TYPE_OBJECT);

  JSON_OBJECT_GET_SET(root, "message", error->msg, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "code", error->code, JSON_TYPE_LONG_LONG);

  JsonNode *error_data = NULL;
  JSON_OBJECT_GET_SET(root, "data", error->data, JSON_TYPE_OBJECT);

  JSON_OBJECT_MAP(error_data, error->data, {
    error->data.start[i].key = curr->key;
    if (curr->type != JSON_TYPE_STRING) return false;
    error->data.start[i].msg = curr->value_str;
  });

  return true;
}

void free_error_response(ErrorResponse error) {
  free_json(error.root);
  free(error.data.start);
};
