#ifndef SPACETRADERS_PARSER_ERROR
#define SPACETRADERS_PARSER_ERROR

#include "../models.h"
#include "json.h"

bool parse_error_response(JsonNode *root, ErrorResponse *error);

void free_error_response(ErrorResponse *error);

#endif