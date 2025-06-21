#include "wallet.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <inttypes.h>

// Initialize reward system with Bitcoin-like parameters
void init_reward_system(RewardSystem *rs) {
    if (rs == NULL) return;
    
    rs->base_reward = 100000000000ULL;  // 1000 coins in smallest units (like satoshi)
    rs->halving_interval = 210000;      // Halve reward every 210,000 blocks
    rs->current_block_height = 0;
    rs->total_supply = 0;
    rs->max_supply = 2100000000000000ULL;  // 21 million coins max (like Bitcoin)
}

// Calculate mining reward based on block height and halving
uint64_t calculate_mining_reward(const RewardSystem *rs, int block_height) {
    if (rs == NULL) return 0;
    
    // Calculate number of halvings that have occurred
    int halvings = block_height / rs->halving_interval;
    
    // After 64 halvings, reward becomes 0 (practically impossible to reach)
    if (halvings >= 64) return 0;
    
    // Calculate reward: base_reward / (2^halvings)
    uint64_t reward = rs->base_reward >> halvings;  // Efficient division by 2^halvings
    
    return reward;
}

// Generate a simple wallet address (simplified for demo)
bool generate_wallet_address(Wallet *wallet) {
    if (wallet == NULL) return false;
    
    // Generate pseudo-random address and private key
    srand(time(NULL) + (uintptr_t)wallet);
    
    // Generate address (simplified - in real blockchain this would use cryptographic methods)
    snprintf(wallet->address, WALLET_ADDRESS_LENGTH, "ARC%016llX%08X", 
             (unsigned long long)time(NULL), rand());
    
    // Generate private key (simplified)
    snprintf(wallet->private_key, PRIVATE_KEY_LENGTH, "PVT%016llX%016llX",
             (unsigned long long)rand(), (unsigned long long)rand());
    
    return true;
}

// Initialize wallet
bool init_wallet(Wallet *wallet) {
    if (wallet == NULL) return false;
    
    memset(wallet, 0, sizeof(Wallet));
    
    if (!generate_wallet_address(wallet)) {
        return false;
    }
    
    wallet->balance = 0;
    wallet->transaction_count = 0;
    
    return true;
}

// Get wallet balance
uint64_t get_wallet_balance(const Wallet *wallet) {
    if (wallet == NULL) return 0;
    return wallet->balance;
}

// Create transaction
bool create_transaction(Transaction *tx, const char *from, const char *to, uint64_t amount) {
    if (tx == NULL || from == NULL || to == NULL) return false;
    
    memset(tx, 0, sizeof(Transaction));
    
    strncpy(tx->from_address, from, WALLET_ADDRESS_LENGTH - 1);
    strncpy(tx->to_address, to, WALLET_ADDRESS_LENGTH - 1);
    tx->amount = amount;
    tx->timestamp = time(NULL);
    tx->is_coinbase = false;
      // Calculate transaction hash
    char tx_data[256];
    snprintf(tx_data, sizeof(tx_data), "%s%s%" PRIu64 "%lld", 
             tx->from_address, tx->to_address, tx->amount, (long long)tx->timestamp);
    tx->hash = simple_hash(tx_data);
    
    return true;
}

// Create coinbase transaction (mining reward)
bool create_coinbase_transaction(Transaction *tx, const char *miner_address, uint64_t reward) {
    if (tx == NULL || miner_address == NULL) return false;
    
    memset(tx, 0, sizeof(Transaction));
    
    strcpy(tx->from_address, "COINBASE");
    strncpy(tx->to_address, miner_address, WALLET_ADDRESS_LENGTH - 1);
    tx->amount = reward;
    tx->timestamp = time(NULL);
    tx->is_coinbase = true;
      // Calculate transaction hash
    char tx_data[256];
    snprintf(tx_data, sizeof(tx_data), "COINBASE%s%" PRIu64 "%lld", 
             tx->to_address, tx->amount, (long long)tx->timestamp);
    tx->hash = simple_hash(tx_data);
    
    return true;
}

// Award mining reward
bool award_mining_reward(Wallet *miner_wallet, const RewardSystem *rs, int block_height) {
    if (miner_wallet == NULL || rs == NULL) return false;
    
    uint64_t reward = calculate_mining_reward(rs, block_height);
    if (reward == 0) return false;
    
    // Create coinbase transaction
    if (miner_wallet->transaction_count >= MAX_TRANSACTIONS) return false;
    
    Transaction *tx = &miner_wallet->transactions[miner_wallet->transaction_count];
    if (!create_coinbase_transaction(tx, miner_wallet->address, reward)) {
        return false;
    }
    
    // Add to wallet
    miner_wallet->balance += reward;
    miner_wallet->transaction_count++;
    
    return true;
}

// Add transaction to wallet
bool add_transaction_to_wallet(Wallet *wallet, const Transaction *tx) {
    if (wallet == NULL || tx == NULL) return false;
    
    if (wallet->transaction_count >= MAX_TRANSACTIONS) return false;
    
    // Copy transaction
    wallet->transactions[wallet->transaction_count] = *tx;
    wallet->transaction_count++;
    
    // Update balance
    if (strcmp(tx->to_address, wallet->address) == 0) {
        wallet->balance += tx->amount;  // Received money
    }
    if (strcmp(tx->from_address, wallet->address) == 0 && !tx->is_coinbase) {
        wallet->balance -= tx->amount;  // Sent money
    }
    
    return true;
}

// Format amount for display
void format_amount(uint64_t amount, char *buffer, size_t buffer_size) {
    if (buffer == NULL) return;
    
    // Convert from smallest units to coins (divide by 100,000,000)
    uint64_t coins = amount / 100000000ULL;    uint64_t fractions = amount % 100000000ULL;
    
    snprintf(buffer, buffer_size, "%" PRIu64 ".%08" PRIu64 " ARC", coins, fractions);
}

// Parse amount from string
uint64_t parse_amount(const char *amount_str) {
    if (amount_str == NULL) return 0;
    
    double amount = atof(amount_str);
    return (uint64_t)(amount * 100000000.0);  // Convert to smallest units
}

// Print wallet information
void print_wallet_info(const Wallet *wallet) {
    if (wallet == NULL) return;
    
    char balance_str[64];
    format_amount(wallet->balance, balance_str, sizeof(balance_str));
    
    printf("+==============================================================================+\n");
    printf("|                              WALLET INFO                                     |\n");
    printf("+==============================================================================+\n");
    printf("| Address      : %-56s |\n", wallet->address);
    printf("| Balance      : %-56s |\n", balance_str);
    printf("| Transactions : %-56d |\n", wallet->transaction_count);
    printf("+==============================================================================+\n");
}

// Print transaction
void print_transaction(const Transaction *tx) {
    if (tx == NULL) return;
    
    char amount_str[64];
    format_amount(tx->amount, amount_str, sizeof(amount_str));
    
    printf("| %s: %s -> %s | %s |\n", 
           tx->is_coinbase ? "REWARD" : "TRANSFER",
           tx->from_address, tx->to_address, amount_str);
}

// Update reward system
void update_reward_system(RewardSystem *rs, int new_block_height) {
    if (rs == NULL) return;    
    rs->current_block_height = new_block_height;
    // Total supply would be calculated by summing all rewards given
}
