#ifndef SPACETRADERS_RESPONSES_MARKET
#define SPACETRADERS_RESPONSES_MARKET

#include "responses.h"

typedef struct TradeGood{
  char* symbol;
  char* name;
  char* desc;
} TradeGood;

typedef enum TransactionType{
  PURCHASE,
  SELL,
} TransactionType;

typedef struct MarketTransactions{
  char* waypoint_symbol;
  char* ship_symbol;
  char* trade_symbol;
  TransactionType type;
  int uints;
  int price_per_unit;
  int total_price;
  time_t timestamp;
} MarketTransactions;

typedef struct MarketTradeGood{
  char* symbol;
  char* type;
  int trade_volume;
  SupplyLevel supply;
  ActivityLevel activity;
  int purchase_price;
  int sell_price;
} MarketTradeGood;

typedef struct Market{
  char* symbol;
  ARRAY_STRUCT(TradeGood,exports);
  ARRAY_STRUCT(TradeGood,imports);
  ARRAY_STRUCT(TradeGood,exchanges);
  ARRAY_STRUCT(MarketTransaction,transactions);
  ARRAY_STRUCT(MarketTradeGood, trade_goods);
} Market;

typedef struct RepairTransaction{
  char* waypoint_symbol;
  char* ship_symbol;
  int total_price;
  time_t timestamp;
} RepairTransaction;

typedef struct ScrapTransaction{
  char* waypoint_symbol;
  char* ship_symbol;
  int total_price;
  time_t timestamp; 
} ScrapTransaction;

typedef struct Waypoint{
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
  char** orbitals;
  size_t orbitals_len;
  char** factions;
  size_t factions_len;
  ARRAY_STRUCT(Trait,traits);
  ARRAY_STRUCT(Trait,modifiers);
  Chart chart;
  bool is_under_construction;
} Waypoint;

#endif