#include "performance.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// Initialize performance metrics
void init_performance_metrics(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    
    memset(pm, 0, sizeof(PerformanceMetrics));
}

// Start timing
void start_timing(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    
    pm->start_time = clock();
}

// End timing
void end_timing(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    
    pm->end_time = clock();
    pm->mining_time = ((double)(pm->end_time - pm->start_time)) / CLOCKS_PER_SEC;
}

// Calculate performance statistics
void calculate_performance_stats(PerformanceMetrics *pm, int pi_digits, int hashes) {
    if (pm == NULL || pm->mining_time <= 0) return;
    
    pm->pi_digits_per_second = (uint64_t)(pi_digits / pm->mining_time);
    pm->hashes_per_second = (uint64_t)(hashes / pm->mining_time);
}

// Print performance report
void print_performance_report(const PerformanceMetrics *pm) {
    if (pm == NULL) return;
    
    printf("+==============================================================================+\n");
    printf("|                           PERFORMANCE REPORT                                |\n");
    printf("+==============================================================================+\n");    printf("| Mining Time       : %.4f seconds                                       |\n", pm->mining_time);
    printf("| Pi Digits/sec     : %" PRIu64 "                                               |\n", pm->pi_digits_per_second);
    printf("| Hashes/sec        : %" PRIu64 "                                               |\n", pm->hashes_per_second);
    printf("| Hash Time         : %.6f seconds                                       |\n", pm->hash_time);
    printf("| Pi Calc Time      : %.6f seconds                                       |\n", pm->pi_calculation_time);
    printf("+==============================================================================+\n");
}

// Initialize memory pool
bool init_memory_pool(MemoryPool *pool, size_t size) {
    if (pool == NULL) return false;
    
    pool->memory_pool = malloc(size);
    if (pool->memory_pool == NULL) return false;
    
    pool->pool_size = size;
    pool->used_size = 0;
    pool->initialized = true;
    
    return true;
}

// Allocate from memory pool
void* allocate_from_pool(MemoryPool *pool, size_t size) {
    if (pool == NULL || !pool->initialized) return NULL;
    
    // Align to 8-byte boundary
    size = (size + 7) & ~7;
    
    if (pool->used_size + size > pool->pool_size) {
        return NULL; // Pool exhausted
    }
    
    void *ptr = (char*)pool->memory_pool + pool->used_size;
    pool->used_size += size;
    
    return ptr;
}

// Reset memory pool
void reset_memory_pool(MemoryPool *pool) {
    if (pool == NULL) return;
    
    pool->used_size = 0;
}

// Cleanup memory pool
void cleanup_memory_pool(MemoryPool *pool) {
    if (pool == NULL) return;
    
    if (pool->memory_pool != NULL) {
        free(pool->memory_pool);
        pool->memory_pool = NULL;
    }
    
    pool->pool_size = 0;
    pool->used_size = 0;
    pool->initialized = false;
}

// Fast hash function (FNV-1a variant)
uint32_t fast_hash(const char *data, size_t length) {
    if (data == NULL) return 0;
    
    uint32_t hash = 2166136261U; // FNV offset basis
    const uint32_t prime = 16777619U; // FNV prime
    
    for (size_t i = 0; i < length; i++) {
        hash ^= (uint32_t)data[i];
        hash *= prime;
    }
    
    return hash;
}

// Fast 64-bit hash function
uint64_t fast_hash64(const char *data, size_t length) {
    if (data == NULL) return 0;
    
    uint64_t hash = 14695981039346656037ULL; // FNV offset basis
    const uint64_t prime = 1099511628211ULL; // FNV prime
    
    for (size_t i = 0; i < length; i++) {
        hash ^= (uint64_t)data[i];
        hash *= prime;
    }
    
    return hash;
}

// Multi-threading support (simplified implementation)
static MiningThread *mining_threads = NULL;
static int num_mining_threads = 0;

bool init_parallel_mining(int num_threads) {
    if (num_threads <= 0 || num_threads > 16) return false;
    
    mining_threads = malloc(num_threads * sizeof(MiningThread));
    if (mining_threads == NULL) return false;
    
    num_mining_threads = num_threads;
    
    for (int i = 0; i < num_threads; i++) {
        mining_threads[i].thread_id = i;
        mining_threads[i].active = false;
    }
    
    printf("Parallel mining initialized with %d threads\n", num_threads);
    return true;
}

void shutdown_parallel_mining(void) {
    if (mining_threads != NULL) {
        free(mining_threads);
        mining_threads = NULL;
    }
    num_mining_threads = 0;
}

void distribute_mining_work(int total_blocks, int num_threads) {
    if (mining_threads == NULL || num_threads <= 0) return;
    
    int blocks_per_thread = total_blocks / num_threads;
    
    for (int i = 0; i < num_threads; i++) {
        mining_threads[i].start_block = i * blocks_per_thread;
        mining_threads[i].end_block = (i == num_threads - 1) ? 
            total_blocks : (i + 1) * blocks_per_thread;
        mining_threads[i].active = true;
    }
    
    printf("Mining work distributed across %d threads\n", num_threads);
}
