#ifndef SPACETRADERS_MODELS_MARKET
#define SPACETRADERS_MODELS_MARKET

#include "other.h"
#include "util.h"

struct TradeGood {
  char* symbol;
  char* name;
  char* desc;
};

enum TransactionType {
  TRANSACTION_TYPE_PURCHASE,
  TRANSACTION_TYPE_SELL,
};

struct MarketTransaction {
  char* waypoint_symbol;
  char* ship_symbol;
  char* trade_symbol;
  enum TransactionType type;
  int uints;
  int price_per_unit;
  int total_price;
  time_t timestamp;
};

struct MarketTradeGood {
  char* symbol;
  char* type;
  int trade_volume;
  enum SupplyLevel supply;
  enum ActivityLevel activity;
  int purchase_price;
  int sell_price;
};

struct Market {
  char* symbol;
  ARRAY_STRUCT(TradeGood, exports);
  ARRAY_STRUCT(TradeGood, imports);
  ARRAY_STRUCT(TradeGood, exchanges);
  ARRAY_STRUCT(MarketTransaction, transactions);
  ARRAY_STRUCT(MarketTradeGood, trade_goods);
};

struct RepairTransaction {
  char* waypoint_symbol;
  char* ship_symbol;
  int total_price;
  time_t timestamp;
};

struct ScrapTransaction {
  char* waypoint_symbol;
  char* ship_symbol;
  int total_price;
  time_t timestamp;
};

struct Waypoint {
  char* symbol;
  char* type;
  char* system_symbol;
  int x;
  int y;
  char** orbitals;
  size_t orbitals_len;
  char** factions;
  size_t factions_len;
  ARRAY_STRUCT(Trait, traits);
  ARRAY_STRUCT(Trait, modifiers);
  struct Chart chart;
  bool is_under_construction;
};

#endif