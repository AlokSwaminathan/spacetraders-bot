#ifndef SPACETRADERS_REQUESTS_UTIL
#define SPACETRADERS_REQUESTS_UTIL

#include <curl/curl.h>

#include "../constants.h"
#include "json.h"
#include "../models.h"

#define JSON_PARSE_FAILURE_CODE 203984

struct CurlResponse{
  char data[MAX_RESPONSE_LEN];
  size_t size;
};

struct CurlJsonResponse{
  struct JsonNode *root;
  struct ErrorResponse *error;
} CurlJsonResponse;

extern struct CurlResponse curl_response;

extern CURL *curl_hnd;

size_t curl_response_to_json(char* data, size_t size, size_t nmemb, void* clientp);

// Takes CURL pointer with all options fully set and returns JSON root node
// If the JSON node is an error, will return the Error
struct CurlJsonResponse curl_get_json(CURL *hnd);

// Handles a CurlJsonResponse and exits the program if it fails, returning the valid json node only if there is no error
struct JsonNode* handle_curl_error(struct CurlJsonResponse resp);

#define curl_get_valid_json(hnd) handle_curl_error(curl_get_json(hnd))

#endif