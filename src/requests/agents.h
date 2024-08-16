#ifndef SPACETRADERS_REQUEST_AGENTS
#define SPACETRADERS_REQUEST_AGENTS

// Returns null-terminated token string
char* register_agent(char* agent_symbol, char* faction, char* email);

// Returns null-terminated agent symbol
char* get_agent_data(char* token);

#endif