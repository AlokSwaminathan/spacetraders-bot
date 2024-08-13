#ifndef SPACETRADERS_REQUESTS_UTIL
#define SPACETRADERS_REQUESTS_UTIL

#include <curl/curl.h>

#include "../constants.h"

struct CurlResponse{
  char data[MAX_RESPONSE_LEN];
  size_t size;
};

extern struct CurlResponse curl_response;

extern CURL *curl_hnd;

extern struct curl_slist *curl_headers;

size_t curl_response_to_json(char* data, size_t size, size_t nmemb, void* clientp);

#endif