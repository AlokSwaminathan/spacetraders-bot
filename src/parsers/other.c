#include "other.h"

#include "util.h"

bool parse_trait(struct JsonNode *root, struct Trait *trait) {
  JSON_OBJECT_GET_SET(root, "symbol", trait->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", trait->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", trait->description, JSON_TYPE_STRING);

  return true;
}

void free_trait(struct Trait *trait){
  free(trait->symbol);
  free(trait->name);
  free(trait->description);
}