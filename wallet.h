#ifndef WALLET_H
#define WALLET_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_TRANSACTIONS 1000
#define WALLET_ADDRESS_LENGTH 64
#define PRIVATE_KEY_LENGTH 64

// Transaction structure
typedef struct {
    char from_address[WALLET_ADDRESS_LENGTH];
    char to_address[WALLET_ADDRESS_LENGTH];
    uint64_t amount;  // Amount in satoshi-like units (1 coin = 100,000,000 units)
    time_t timestamp;
    uint32_t hash;
    bool is_coinbase;  // True for mining rewards
} Transaction;

// Wallet structure
typedef struct {
    char address[WALLET_ADDRESS_LENGTH];
    char private_key[PRIVATE_KEY_LENGTH];
    uint64_t balance;
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
} Wallet;

// Blockchain reward system
typedef struct {
    uint64_t base_reward;        // Base reward (1000 coins = 100,000,000,000 units)
    int halving_interval;        // Blocks between halvings (e.g., 210,000 like Bitcoin)
    int current_block_height;
    uint64_t total_supply;
    uint64_t max_supply;         // Maximum coins that can ever exist
} RewardSystem;

// Function declarations
bool init_wallet(Wallet *wallet);
bool generate_wallet_address(Wallet *wallet);
uint64_t get_wallet_balance(const Wallet *wallet);
bool create_transaction(Transaction *tx, const char *from, const char *to, uint64_t amount);
bool create_coinbase_transaction(Transaction *tx, const char *miner_address, uint64_t reward);
bool sign_transaction(Transaction *tx, const char *private_key);
bool verify_transaction(const Transaction *tx, const char *public_key);
bool add_transaction_to_wallet(Wallet *wallet, const Transaction *tx);

// Reward system functions
void init_reward_system(RewardSystem *rs);
uint64_t calculate_mining_reward(const RewardSystem *rs, int block_height);
bool award_mining_reward(Wallet *miner_wallet, const RewardSystem *rs, int block_height);
void update_reward_system(RewardSystem *rs, int new_block_height);

// Utility functions
void format_amount(uint64_t amount, char *buffer, size_t buffer_size);
uint64_t parse_amount(const char *amount_str);
void print_wallet_info(const Wallet *wallet);
void print_transaction(const Transaction *tx);

#endif
