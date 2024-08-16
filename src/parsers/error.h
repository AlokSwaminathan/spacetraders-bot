#ifndef SPACETRADERS_PARSER_ERROR
#define SPACETRADERS_PARSER_ERROR

#include "../models.h"
#include "json.h"

bool parse_error_response(struct JsonNode *root, struct ErrorResponse *error);

void free_error_response(struct ErrorResponse *error);

#endif