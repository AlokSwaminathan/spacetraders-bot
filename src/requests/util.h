#ifndef SPACETRADERS_REQUESTS_UTIL
#define SPACETRADERS_REQUESTS_UTIL

#include <curl/curl.h>

#include "../constants.h"
#include "json.h"
#include "../models.h"
#include "../curl.h"

extern struct CurlResponse curl_response;

extern CURL *curl_hnd;

extern const char* g_error_json_name;

#define curl_get_valid_json(method,url,postfields) handle_curl_error(curl_get_json(method,url,postfields))

#endif