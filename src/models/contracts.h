#ifndef SPACETRADERS_RESPONSES_CONTRACTS
#define SPACETRADERS_RESPONSES_CONTRACTS

#include "../util.h"

typedef enum ContractType {
  CONTRACT_PROCUREMENT,
  CONTRACT_TRANSPORT,
  CONTRACT_SHUTTLE,
} ContractType;

typedef struct ContractDeliverGood{
  char* trade_symbol;
  char* dest_symbol;
  int units_required;
  int units_fulfilled;
} ContractDeliverGood;

typedef struct ContractPayment{
  int on_accepted;
  int on_fulfilled;
} ContractPayment;

typedef struct ContractTerms {
  time_t deadline;
  ContractPayment payment;
  ARRAY_STRUCT(ContractDeliverGood, deliveries);
} ContractTerms;

typedef struct Contract {
  char* id;
  char* faction_symbol;
  ContractType type;
  bool accepted;
  bool fulfilled;
  ContractTerms terms;
  time_t accept_deadline;
} Contract;

#endif