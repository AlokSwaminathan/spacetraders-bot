#include "json.h"
#include <stdio.h>

int main(int argc, char** argv){
  if (argc != 2){
    printf("Only one argument allowed\n");
    return 1;
  }
  JsonNode* root = parse_json(argv[1]);
  if (root == NULL){
    printf("Failed to parse JSON");
    return 1;
  }
  char buf[500*1024];
  if (json_dump(root,buf,sizeof(buf))){
    printf("%s",buf);
    free_json(root);
  }
}