#ifndef SPACETRADERS_MODELS_CONTRACTS
#define SPACETRADERS_MODELS_CONTRACTS

#include "util.h"

enum ContractType {
  CONTRACT_PROCUREMENT,
  CONTRACT_TRANSPORT,
  CONTRACT_SHUTTLE,
};

struct ContractDeliverGood {
  char* trade_symbol;
  char* dest_symbol;
  int units_required;
  int units_fulfilled;
};

struct ContractPayment {
  int on_accepted;
  int on_fulfilled;
};

struct ContractTerms {
  time_t deadline;
  struct ContractPayment payment;
  ARRAY_STRUCT(ContractDeliverGood, deliveries);
};

struct Contract {
  char* id;
  char* faction_symbol;
  enum ContractType type;
  bool accepted;
  bool fulfilled;
  struct ContractTerms terms;
  time_t accept_deadline;
};

#endif