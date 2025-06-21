#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

// Performance monitoring structure
typedef struct {
    clock_t start_time;
    clock_t end_time;
    double mining_time;
    double hash_time;
    double pi_calculation_time;
    uint64_t hashes_per_second;
    uint64_t pi_digits_per_second;
} PerformanceMetrics;

// Memory pool for efficient allocation
typedef struct {
    void *memory_pool;
    size_t pool_size;
    size_t used_size;
    bool initialized;
} MemoryPool;

// Function declarations
void init_performance_metrics(PerformanceMetrics *pm);
void start_timing(PerformanceMetrics *pm);
void end_timing(PerformanceMetrics *pm);
void calculate_performance_stats(PerformanceMetrics *pm, int pi_digits, int hashes);
void print_performance_report(const PerformanceMetrics *pm);

// Memory pool functions
bool init_memory_pool(MemoryPool *pool, size_t size);
void* allocate_from_pool(MemoryPool *pool, size_t size);
void reset_memory_pool(MemoryPool *pool);
void cleanup_memory_pool(MemoryPool *pool);

// Optimized hash functions
uint32_t fast_hash(const char *data, size_t length);
uint64_t fast_hash64(const char *data, size_t length);

// Multi-threading support structures
typedef struct {
    int thread_id;
    int start_block;
    int end_block;
    void *blockchain;
    void *reward_system;
    void *miner_wallet;
    bool active;
} MiningThread;

// Multi-threading functions
bool init_parallel_mining(int num_threads);
void shutdown_parallel_mining(void);
void distribute_mining_work(int total_blocks, int num_threads);

#endif
