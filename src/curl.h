#ifndef SPACETRADERS_CURL
#define SPACETRADERS_CURL

#include <curl/curl.h>

#include "constants.h"

void curl_add_auth(CURL* hnd, struct curl_slist* curl_headers, char* token);

struct curl_slist* curl_init(CURL** hnd);

size_t curl_response_to_json(char* data, size_t size, size_t nmemb, void* clientp);

// Takes CURL pointer with all options fully set and returns JSON root node
// If the JSON node is an error, will return the Error
struct CurlJsonResponse curl_get_json(char* method, char* url, char* postfields);

// Handles a CurlJsonResponse and exits the program if it fails, returning the valid json node only if there is no error
struct JsonNode* handle_curl_error(struct CurlJsonResponse resp);

struct CurlResponse {
  char data[MAX_RESPONSE_LEN];
  size_t size;
};

struct CurlJsonResponse {
  struct JsonNode* root;
  struct ErrorResponse* error;
};

#define JSON_PARSE_FAILURE_CODE 203984

#endif