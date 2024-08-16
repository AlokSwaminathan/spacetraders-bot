#include "util.h"

#include "../parsers.h"
#include "../util.h"

size_t curl_response_to_json(char* data, size_t size, size_t nmemb, void* clientp) {
  size_t realsize = size * nmemb;
  struct CurlResponse* response = (struct CurlResponse*)clientp;

  if (response->size + realsize > MAX_RESPONSE_LEN) return 0;

  memcpy(&(response->data[response->size]), data, realsize);
  response->size += realsize;
  response->data[response->size] = '\0';

  return realsize;
}

struct CurlJsonResponse curl_get_json(char* method, char* url, char* postfields) {
  struct CurlJsonResponse resp;
  resp.error = calloc(1, sizeof(*resp.error));
  MALLOC_CHECK(resp.error);
  resp.root = NULL;

  // Set up CURL request
  curl_easy_setopt(curl_hnd,CURLOPT_CUSTOMREQUEST,method);
  curl_easy_setopt(curl_hnd,CURLOPT_URL,url);
  if (postfields != NULL){
    curl_easy_setopt(curl_hnd,CURLOPT_POSTFIELDS,postfields);
  }

  CURLcode ret = curl_easy_perform(curl_hnd);

  if (ret != CURLE_OK) {
    resp.error->msg = "CURL failed to perform request";
    resp.error->is_curl_failure = true;
    resp.error->code = (int)ret;
    return resp;
  }

  struct JsonNode* root = parse_json(curl_response.data);

  if (root == NULL) {
    resp.error->msg = "Failed to parse response to json";
    resp.error->is_curl_failure = false;
    resp.error->code = JSON_PARSE_FAILURE_CODE;
    return resp;
  }

  if (parse_error_response(root, resp.error)) {
    return resp;
  }

  resp.root = root;

  free(resp.error);
  resp.error = NULL;

  return resp;
}

struct JsonNode* handle_curl_error(struct CurlJsonResponse resp) {
  if (resp.error == NULL) {
    return resp.root;
  }

  fprintf(stderr, "ERROR IN HTTP REQUEST\n\n");
  fprintf(stderr, "Main message: %s\n", resp.error->msg);
  fprintf(stderr, "Error code: %d\n", resp.error->code);
  fprintf(stderr, "Error data\n");
  for (size_t i = 0; i < resp.error->data.len; i++) {
    fprintf(stderr, "JSON Key: %s, Error: %s\n", resp.error->data.start[i].key, resp.error->data.start[i].msg);
  }

  free_error_response(resp.error);

  handle_exit(0);
}