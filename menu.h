#ifndef MENU_H
#define MENU_H

#include "block.h"
#include "wallet.h"
#include "network.h"
#include "performance.h"

// Menu options
typedef enum {
    MENU_EXIT = 0,
    MENU_MINE_BLOCKS,
    MENU_VIEW_BLOCKCHAIN,
    MENU_CHECK_BALANCE,
    MENU_TRANSFER_FUNDS,
    MENU_VIEW_WALLET,
    MENU_NETWORK_STATUS,
    MENU_PERFORMANCE_STATS,
    MENU_SYNC_BLOCKCHAIN,
    MENU_EXPORT_BLOCKCHAIN,
    MENU_IMPORT_BLOCKCHAIN,
    MENU_CREATE_WALLET,
    MENU_LOAD_WALLET
} MenuOption;

// Application state
typedef struct {
    Block *blockchain;
    int blockchain_size;
    int blockchain_capacity;
    Wallet miner_wallet;
    RewardSystem reward_system;
    NetworkManager network;
    PerformanceMetrics performance;
    MemoryPool memory_pool;
    bool network_enabled;
    bool performance_monitoring;
    char blockchain_file[256];
    char wallet_file[256];
} AppState;

// Function declarations
void display_main_menu(void);
MenuOption get_menu_choice(void);
void handle_menu_choice(AppState *app, MenuOption choice);

// Menu handlers
void handle_mine_blocks(AppState *app);
void handle_view_blockchain(AppState *app);
void handle_check_balance(AppState *app);
void handle_transfer_funds(AppState *app);
void handle_view_wallet(AppState *app);
void handle_network_status(AppState *app);
void handle_performance_stats(AppState *app);
void handle_sync_blockchain(AppState *app);
void handle_export_blockchain(AppState *app);
void handle_import_blockchain(AppState *app);
void handle_create_wallet(AppState *app);
void handle_load_wallet(AppState *app);

// Utility functions
bool init_app_state(AppState *app);
void cleanup_app_state(AppState *app);
bool save_blockchain(const AppState *app);
bool load_blockchain(AppState *app);
bool save_wallet(const Wallet *wallet, const char *filename);
bool load_wallet(Wallet *wallet, const char *filename);
void print_app_banner(void);

#endif
