#include "factions.h"

#include "other.h"
#include "util.h"

bool parse_faction(struct JsonNode *root, struct Faction *faction) {
  JSON_OBJECT_GET_SET(root, "symbol", faction->symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "name", faction->name, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "description", faction->desc, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "headquarters", faction->headquarters, JSON_TYPE_STRING);

  struct JsonNode *traits;
  JSON_OBJECT_GET_SET(root, "traits", traits, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(traits, faction->traits, {
    if (!parse_trait(curr, &(faction->traits.start[i]))) return false;
  });

  JSON_OBJECT_GET_SET(root,"isRecruiting",faction->is_recruiting,JSON_TYPE_BOOL);

  return true;
}

void free_faction(struct Faction *faction){
  free(faction->symbol);
  free(faction->name);
  free(faction->desc);
  free(faction->headquarters);
  ARRAY_STRUCT_FREE(faction->traits,{
    free_trait(&(faction->traits.start[i]));
  });
}