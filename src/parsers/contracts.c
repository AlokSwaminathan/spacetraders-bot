#include "contracts.h"

#include <string.h>

#include "util.h"

bool parse_contract_payment(struct JsonNode *root, struct ContractPayment *payment) {
  JSON_OBJECT_GET_SET(root, "onAccepted", payment->on_accepted, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "onFulfilled", payment->on_fulfilled, JSON_TYPE_LONG_LONG);

  return true;
}

void free_contract_payment(struct ContractPayment *payment __attribute__((unused))){}

bool parse_contract_delivery(struct JsonNode *root, struct ContractDeliverGood *delivery) {
  JSON_OBJECT_GET_SET(root, "tradeSymbol", delivery->trade_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "destinationSymbol", delivery->dest_symbol, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "unitsRequired", delivery->units_required, JSON_TYPE_LONG_LONG);

  JSON_OBJECT_GET_SET(root, "unitsFulfilled", delivery->units_fulfilled, JSON_TYPE_LONG_LONG);

  return true;
}

void free_contract_delivery(struct ContractDeliverGood *good){
  free(good->trade_symbol);
  free(good->dest_symbol);
}

bool parse_contract_terms(struct JsonNode *root, struct ContractTerms *terms) {
  char *deadline_str;
  JSON_OBJECT_GET_SET(root, "deadline", deadline_str, JSON_TYPE_STRING);
  terms->deadline = parse_date_time_str(deadline_str);
  CHECK_TIME_T(terms->deadline); 

  struct JsonNode *payment;
  JSON_OBJECT_GET_SET(root, "payment", payment, JSON_TYPE_OBJECT);
  if (!parse_contract_payment(payment, &(terms->payment))) return false;

  struct JsonNode *deliveries;
  JSON_OBJECT_GET_SET(root, "deliver", deliveries, JSON_TYPE_ARRAY);

  JSON_ARRAY_MAP(deliveries, terms->deliveries, {
    if (!parse_contract_delivery(curr, &(terms->deliveries.start[i]))) return false;
  });

  return true;
}

void free_contract_terms(struct ContractTerms *terms){
  free_contract_payment(&(terms->payment));
  ARRAY_STRUCT_FREE(terms->deliveries,{
    free_contract_delivery(&(terms->deliveries.start[i]));
  });
}

bool parse_contract(struct JsonNode *root, struct Contract *contract) {
  JSON_OBJECT_GET_SET(root, "id", contract->id, JSON_TYPE_STRING);

  JSON_OBJECT_GET_SET(root, "factionSymbol", contract->faction_symbol, JSON_TYPE_STRING);

  char *contract_type;
  JSON_OBJECT_GET_SET(root, "type", contract_type, JSON_TYPE_STRING);

  if (strcmp(contract_type, CONTRACT_PROCUREMENT_STRING) == 0) {
    contract->type = CONTRACT_PROCUREMENT;
  } else if (strcmp(contract_type, CONTRACT_TRANSPORT_STRING) == 0) {
    contract->type = CONTRACT_TRANSPORT;
  } else if (strcmp(contract_type, CONTRACT_SHUTTLE_STRING) == 0) {
    contract->type = CONTRACT_SHUTTLE;
  } else {
    return false;
  }

  struct JsonNode *terms;
  JSON_OBJECT_GET_SET(root, "terms", terms, JSON_TYPE_OBJECT);
  if (!parse_contract_terms(terms, &(contract->terms))) return false;

  JSON_OBJECT_GET_SET(root, "accepted", contract->accepted, JSON_TYPE_BOOL);

  JSON_OBJECT_GET_SET(root, "fulfilled", contract->fulfilled, JSON_TYPE_BOOL);

  char *deadline_str;
  JSON_OBJECT_GET_SET(root, "deadlineToAccept", deadline_str, JSON_TYPE_STRING);

  contract->accept_deadline = parse_date_time_str(deadline_str);
  CHECK_TIME_T(contract->accept_deadline); 

  return true;
}

void free_contract(struct Contract *contract){
  free(contract->id);
  free(contract->faction_symbol);
  free_contract_terms(&(contract->terms));
}