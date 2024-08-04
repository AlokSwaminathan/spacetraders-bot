#include "json.h"

#include <stdlib.h>

// "private" functions
char* json_parse_object(char *json_string, JsonNode *json_node);
char* json_parse_kv_pair(char *json_string, JsonNode *json_node);
char* json_parse_string(char *json_string, JsonNode *json_node);
char* json_parse_num(char *json_string, JsonNode *json_node);
char* json_parse_array(char *json_string, JsonNode *json_node);
char* json_parse_null(char *json_string, JsonNode *json_node);

void free_json(JsonNode *json_root) {
}

JsonNode *parse_json(char *json_string) {
}

bool validate_json(char *json_string) {
}

char *parse_json_symbol(char *json_string, JsonNode *json_node) {
}

char *json_parse_object(char *json_string, JsonNode *json_node) {
}

char *json_parse_kv_pair(char *json_string, JsonNode *json_node) {
}

char *json_parse_array(char *json_string, JsonNode *json_node) {
}

char *json_parse_string(char *json_string, JsonNode *json_node) {
}

char *json_parse_num(char *json_string, JsonNode *json_node) {
}

char *json_parse_null(char *json_string, JsonNode *json_node) {
}
