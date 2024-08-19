#include "curl.h"

#include "models.h"
#include "constants.h"
#include "parsers.h"

extern struct CurlResponse curl_response;

extern CURL* curl_hnd;

struct curl_slist* curl_init(CURL** hnd) {
  *hnd = curl_easy_init();
  if (*hnd == NULL) {
    fprintf(stderr, "Curl didn't init right\n");
    exit(1);
  }

  struct curl_slist* curl_headers = NULL;

  curl_headers = curl_slist_append(curl_headers, "Content-Type: application/json");
  curl_headers = curl_slist_append(curl_headers, "Accept: application/json");

  curl_easy_setopt(*hnd, CURLOPT_HTTPHEADER, curl_headers);

  curl_easy_setopt(*hnd, CURLOPT_WRITEFUNCTION, curl_response_to_json);
  curl_easy_setopt(*hnd, CURLOPT_WRITEDATA, (void*)&curl_response);

  return curl_headers;
}

void curl_add_auth(CURL* hnd, struct curl_slist* curl_headers, char* token) {
  // Add bearer auth token
  char header_token[BEARER_TOKEN_START_LEN + strlen(token) + 1];
  memcpy(header_token, BEARER_TOKEN_START, BEARER_TOKEN_START_LEN);
  memcpy(header_token + BEARER_TOKEN_START_LEN, token, strlen(token) + 1);
  curl_headers = curl_slist_append(curl_headers, (const char*)header_token);

  curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, curl_headers);
}

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
  memset(resp.error,0,sizeof(*resp.error));
  MALLOC_CHECK(resp.error);
  resp.root = NULL;

  // Set up CURL request
  curl_easy_setopt(curl_hnd, CURLOPT_CUSTOMREQUEST, method);
  curl_easy_setopt(curl_hnd, CURLOPT_URL, url);
  if (postfields != NULL) {
    curl_easy_setopt(curl_hnd, CURLOPT_POSTFIELDS, postfields);
  }

  CURLcode ret = curl_easy_perform(curl_hnd);
  curl_response.size = 0;

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
    fprintf(stderr, "JSON Key: %s, Error:", resp.error->data.start[i].key);
    for (size_t j = 0; j < resp.error->data.start[i].msgs_len; j++) {
      fprintf(stderr, " %s", resp.error->data.start[i].msgs[j]);
    }
    fprintf(stderr, "\n");
  }

  free_error_response(resp.error);

  handle_exit(0);

  return NULL;
}