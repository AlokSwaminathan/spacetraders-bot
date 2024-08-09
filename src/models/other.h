#ifndef SPACETRADERS_RESPONSES_OTHER
#define SPACETRADERS_RESPONSES_OTHER

typedef enum ActivityLevel {
  WEAK,
  GROWING,
  STRONG,
  RESTRICTED,
} ActivityLevel;

typedef enum SupplyLevel {
  SCARCE,
  LIMITED,
  MODERATE,
  HIGH,
  ABUNDANT,
} SupplyLevel;

typedef struct ConstructionMaterials {
  char* trade_symbol;
  int required;
  int fulfilled;
} ConstructionMaterials;

typedef struct Trait {
  char* symbol;
  char* name;
  char* description;
} Trait;

typedef struct Pagination {
  int total;
  int page;
  int limit;
} Pagination;

#endif