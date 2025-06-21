#ifndef BLOCK_H
#define BLOCK_H

#include <stdint.h>
#include <time.h>
#include "wallet.h"

#define MAX_PI_DIGITS 10000000  // 10 million digits - very large buffer
#define MAX_TRANSACTIONS_PER_BLOCK 100
// No MAX_DIFFICULTY limit - allow unlimited difficulty

typedef struct {
    int index;
    time_t timestamp;
    int difficulty;
    char *pi_digits;     // Dynamically allocated Pi digits
    int pi_digits_count; // Number of Pi digits stored
    uint32_t prev_hash;
    uint32_t hash;
    
    // Enhanced blockchain features
    Transaction transactions[MAX_TRANSACTIONS_PER_BLOCK];
    int transaction_count;
    char miner_address[WALLET_ADDRESS_LENGTH];
    uint64_t mining_reward;
    uint32_t nonce;  // For additional proof of work validation
    uint64_t total_difficulty;  // Cumulative difficulty for chain selection
} Block;

void mine_block(Block *block, uint32_t prev_hash, const Block *prev_block, const char *miner_address, const RewardSystem *reward_system);
void print_block(const Block *block);
void cleanup_block(Block *block);  // Free dynamically allocated memory
bool add_transaction_to_block(Block *block, const Transaction *tx);
bool validate_block(const Block *block, const Block *prev_block);
uint32_t calculate_block_hash(const Block *block);
bool is_valid_proof_of_work(const Block *block);

#endif
