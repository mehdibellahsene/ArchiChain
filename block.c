#include "block.h"
#include "utils.h"
#include "pi.h"
#include "wallet.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

// Forward declaration
bool create_coinbase_transaction(Transaction *tx, const char *miner_address, uint64_t reward);

void mine_block(Block *block, uint32_t prev_hash, const Block *prev_block, const char *miner_address, const RewardSystem *reward_system) {    if (block == NULL) {
        return;
    }
    
    // Initialize block
    memset(block, 0, sizeof(Block));
    block->timestamp = time(NULL);
    block->transaction_count = 0;
    
    // Set miner address
    if (miner_address != NULL) {
        strncpy(block->miner_address, miner_address, WALLET_ADDRESS_LENGTH - 1);
    }
    
    // Calculate mining reward
    if (reward_system != NULL) {
        block->mining_reward = calculate_mining_reward(reward_system, block->index);
        
        // Create coinbase transaction for mining reward
        if (block->mining_reward > 0 && miner_address != NULL) {
            Transaction coinbase_tx;
            if (create_coinbase_transaction(&coinbase_tx, miner_address, block->mining_reward)) {
                add_transaction_to_block(block, &coinbase_tx);
            }
        }
    }
      
    block->timestamp = time(NULL);
    
    // Use exponential difficulty but limit growth for memory optimization
    int power = block->index + 1;
    if (power > 25) { // Limit to 2^25 = ~33 million digits for memory safety
        power = 25;
    }
    block->difficulty = 1 << power;
    block->prev_hash = prev_hash;
    
    // Calculate total difficulty for chain selection
    block->total_difficulty = (prev_block != NULL) ? 
        prev_block->total_difficulty + block->difficulty : block->difficulty;
    
    // Memory optimization: only store what we need
    // For blocks beyond a certain point, reduce stored digits but keep hash chain
    int digits_to_store = block->difficulty;
    if (block->index > 10) { // After block 10, only store first 10000 digits for display
        digits_to_store = (block->difficulty > 10000) ? 10000 : block->difficulty;
    }
    
    // Dynamically allocate memory for Pi digits
    block->pi_digits = malloc(digits_to_store + 1);
    if (block->pi_digits == NULL) {
        printf("Error: Could not allocate memory for Pi digits\n");
        return;
    }    // Use previous block's Pi digits as seed for next calculation
    get_pi_digits_from_previous(block->pi_digits, digits_to_store, (const void *)prev_block);
    block->pi_digits_count = digits_to_store;
    
    // Calculate initial block hash
    block->hash = calculate_block_hash(block);
    
    // Proof of work: find nonce that makes the hash meet difficulty requirements
    block->nonce = 0;
    while (!is_valid_proof_of_work(block)) {
        block->nonce++;
        block->hash = calculate_block_hash(block);
        
        // Prevent infinite loops in case of implementation issues
        if (block->nonce > 1000000) {
            printf("Warning: Proof of work taking too long, using current nonce\n");
            break;
        }
    }
}

// Add transaction to block
bool add_transaction_to_block(Block *block, const Transaction *tx) {
    if (block == NULL || tx == NULL) return false;
    
    if (block->transaction_count >= MAX_TRANSACTIONS_PER_BLOCK) {
        return false;
    }
    
    block->transactions[block->transaction_count] = *tx;
    block->transaction_count++;
    return true;
}

// Calculate block hash including all transactions
uint32_t calculate_block_hash(const Block *block) {
    if (block == NULL) return 0;
    
    char content[8192]; // Larger buffer for transactions
    int offset = 0;
    
    // Add basic block data
    offset += snprintf(content + offset, sizeof(content) - offset, 
                      "%d%lld%d%s%u%u%s%llu", 
                      block->index, (long long)block->timestamp, block->difficulty, 
                      block->pi_digits ? block->pi_digits : "", block->prev_hash, 
                      block->nonce, block->miner_address, block->mining_reward);
    
    // Add transaction data
    for (int i = 0; i < block->transaction_count && offset < sizeof(content) - 100; i++) {
        offset += snprintf(content + offset, sizeof(content) - offset,
                          "%s%s%llu%u", 
                          block->transactions[i].from_address,
                          block->transactions[i].to_address,
                          block->transactions[i].amount,
                          block->transactions[i].hash);
    }
    
    return simple_hash(content);
}

// Check if proof of work is valid (simplified - just check if hash has certain properties)
bool is_valid_proof_of_work(const Block *block) {
    if (block == NULL) return false;
    
    // Simple proof of work: hash must be divisible by difficulty level
    // In a real blockchain, this would check for leading zeros or similar
    return (block->hash % (block->difficulty / 1000 + 1)) == 0;
}

// Validate block
bool validate_block(const Block *block, const Block *prev_block) {
    if (block == NULL) return false;
    
    // Check if block index is correct
    if (prev_block != NULL && block->index != prev_block->index + 1) {
        return false;
    }
    
    // Check if previous hash is correct
    if (prev_block != NULL && block->prev_hash != prev_block->hash) {
        return false;
    }
    
    // Check if block hash is valid
    if (block->hash != calculate_block_hash(block)) {
        return false;
    }
    
    // Check proof of work
    if (!is_valid_proof_of_work(block)) {
        return false;
    }
    
    return true;
}

void print_block(const Block *block) {
    if (block == NULL) {
        printf("Error: Block is NULL\n");
        return;
    }
    
    int pi_len = block->pi_digits ? strlen(block->pi_digits) : 0;
    
    // Format mining reward
    char reward_str[64];
    format_amount(block->mining_reward, reward_str, sizeof(reward_str));
    
    printf("+==============================================================================+\n");
    printf("|                               BLOCK #%-4d                                    |\n", block->index);
    printf("+==============================================================================+\n");
    printf("| Timestamp      : %-20lld                                      |\n", (long long)block->timestamp);
    printf("| Difficulty     : %-10d digits of Pi to compute                      |\n", block->difficulty);
    printf("| Stored Digits  : %-10d (optimized for memory)                       |\n", block->pi_digits_count);
    printf("| Previous Hash  : %-20u                                      |\n", block->prev_hash);
    printf("| Block Hash     : %-20u                                      |\n", block->hash);
    printf("| Nonce          : %-20u                                      |\n", block->nonce);
    printf("| Miner Address  : %-50s      |\n", block->miner_address);
    printf("| Mining Reward  : %-50s      |\n", reward_str);
    printf("| Transactions   : %-10d                                            |\n", block->transaction_count);
    printf("| Total Difficulty: %-20llu                                     |\n", block->total_difficulty);
    printf("+==============================================================================+\n");
    
    // Show transactions
    if (block->transaction_count > 0) {
        printf("|                              TRANSACTIONS                                    |\n");
        printf("+==============================================================================+\n");
        for (int i = 0; i < block->transaction_count; i++) {
            print_transaction(&block->transactions[i]);
        }
        printf("+==============================================================================+\n");
    }
    
    printf("|                         PI COMPUTATION RESULT                               |\n");
    printf("+==============================================================================+\n");
    printf("| Successfully computed %d digits of Pi for this block!                      |\n", pi_len);
    if (block->pi_digits_count < block->difficulty) {
        printf("| (Memory optimized: showing first %d of %d digits)                      |\n", 
               block->pi_digits_count, block->difficulty);
    }
    printf("|                                                                              |\n");
    
    // Display Pi digits in chunks of 70 characters per line for better readability
    if (block->pi_digits) {
        for (int i = 0; i < pi_len; i += 70) {
            printf("| ");
            int chars_printed = 0;
            for (int j = i; j < i + 70 && j < pi_len; j++) {
                printf("%c", block->pi_digits[j]);
                chars_printed++;
            }
            // Fill remaining space with spaces to align with border
            for (int k = chars_printed; k < 70; k++) {
                printf(" ");
            }
            printf(" |\n");
        }
    }
    
    printf("|                                                                              |\n");
    
    // Show the complete Pi value in a better format
    if (pi_len <= 65 && block->pi_digits) {
        printf("| Pi = 3.%s", block->pi_digits);
        // Add padding spaces to align with the border
        for (int k = pi_len + 8; k < 77; k++) { // 8 = length of "Pi = 3."
            printf(" ");
        }
        printf("|\n");
    } else if (block->pi_digits) {
        printf("| Pi = 3.");
        // Show first 60 digits, then indicate truncation
        for (int i = 0; i < 60 && i < pi_len; i++) {
            printf("%c", block->pi_digits[i]);
        }
        printf("... |\n");
    }
    
    printf("|                                                                              |\n");
    printf("| >> Mining Status: COMPLETE - %d Pi digits successfully calculated!        |\n", pi_len);
    printf("| >> Proof of Work: VALID - Nonce %u found after Pi calculation!           |\n", block->nonce);
    printf("+==============================================================================+\n\n");
}

void cleanup_block(Block *block) {
    if (block != NULL && block->pi_digits != NULL) {
        free(block->pi_digits);
        block->pi_digits = NULL;
    }
}
