#include "util.h"

size_t curl_response_to_json(char* data, size_t size, size_t nmemb, void* clientp) {
  size_t realsize = size * nmemb;
  struct CurlResponse* response = (struct CurlResponse*)clientp;

  if (response->size + realsize > MAX_RESPONSE_LEN) return 0;

  memcpy(&(response->data[response->size]), data, realsize);
  response->size += realsize;
  response->data[response->size] = '\0';

  return realsize;
}