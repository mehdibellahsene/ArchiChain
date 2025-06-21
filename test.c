#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "block.h"
#include "pi.h"
#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "block.h"
#include "pi.h"
#include "utils.h"
#include "wallet.h"

void test_pi_calculation() {
    printf("Testing Pi calculation...\n");
    
    // Test get_pi_digits function instead of compute_pi
    char digits[100];
    get_pi_digits(digits, 10);
    printf("First 10 Pi digits: %s\n", digits);
    
    // First few digits of pi are 1415926535
    assert(strncmp(digits, "14159", 5) == 0);
    
    printf("✓ Pi calculation tests passed\n\n");
}

void test_hash_function() {
    printf("Testing hash function...\n");
    
    uint32_t hash1 = simple_hash("test");
    uint32_t hash2 = simple_hash("test");
    uint32_t hash3 = simple_hash("different");
    
    printf("Hash of 'test': %u\n", hash1);
    printf("Hash of 'test' (again): %u\n", hash2);
    printf("Hash of 'different': %u\n", hash3);
    
    // Same input should produce same hash
    assert(hash1 == hash2);
    
    // Different inputs should produce different hashes (very likely)
    assert(hash1 != hash3);
    
    // Null input should return 0
    assert(simple_hash(NULL) == 0);
    
    printf("✓ Hash function tests passed\n\n");
}

void test_block_mining() {
    printf("Testing block mining...\n");
    
    Block block;
    block.index = 0;
    
    // Initialize wallet and reward system for mining
    Wallet wallet;
    init_wallet(&wallet, "test_wallet.dat");
    
    RewardSystem reward_system;
    init_reward_system(&reward_system);
    
    mine_block(&block, 0, NULL, wallet.address, &reward_system);
    
    printf("Mined block:\n");
    printf("  Index: %d\n", block.index);
    printf("  Timestamp: %lld\n", (long long)block.timestamp);
    printf("  Difficulty: %d\n", block.difficulty);
    printf("  Pi digits length: %d\n", block.pi_digits_count);
    printf("  Hash: %u\n", block.hash);
    
    // Basic validation
    assert(block.index == 0);
    assert(block.prev_hash == 0);
    assert(block.hash != 0);
    assert(block.pi_digits_count > 0);
    
    printf("✓ Block mining tests passed\n\n");
}

void test_blockchain_sequence() {
    printf("Testing blockchain sequence...\n");
    
    Block blocks[3];
    
    // Initialize wallet and reward system for mining
    Wallet wallet;
    init_wallet(&wallet, "test_wallet_seq.dat");
    
    RewardSystem reward_system;
    init_reward_system(&reward_system);
    
    for (int i = 0; i < 3; i++) {
        blocks[i].index = i;
        uint32_t prev_hash = (i == 0) ? 0 : blocks[i-1].hash;
        const Block *prev_block = (i == 0) ? NULL : &blocks[i-1];
        mine_block(&blocks[i], prev_hash, prev_block, wallet.address, &reward_system);
        
        printf("Block %d: hash=%u, prev_hash=%u\n", 
               i, blocks[i].hash, blocks[i].prev_hash);
    }
    
    // Verify chain integrity
    assert(blocks[0].prev_hash == 0);
    assert(blocks[1].prev_hash == blocks[0].hash);
    assert(blocks[2].prev_hash == blocks[1].hash);
    
    // Verify increasing difficulty
    assert(blocks[1].difficulty > blocks[0].difficulty);
    assert(blocks[2].difficulty > blocks[1].difficulty);
    
    printf("✓ Blockchain sequence tests passed\n\n");
}

int main() {
    printf("Archimedes Blockchain - Test Suite\n");
    printf("===================================\n\n");
    
    test_pi_calculation();
    test_hash_function();
    test_block_mining();
    test_blockchain_sequence();
    
    printf("🎉 All tests passed successfully!\n");
    return 0;
}
