#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "block.h"
#include "wallet.h"
#include "network.h"
#include "performance.h"
#include "menu.h"

static volatile int keep_running = 1;
static AppState *global_app = NULL;

void interrupt_handler(int sig) {
    (void)sig; // Suppress unused parameter warning
    keep_running = 0;
    printf("\n\nInterrupt received. Returning to main menu...\n");
}

int main() {
    // Initialize application state
    AppState app;
    global_app = &app;
    
    // Set up signal handler
    signal(SIGINT, interrupt_handler);
    
    // Print banner
    print_app_banner();
    
    // Initialize application
    printf("Initializing Archimedes Blockchain...\n");
    if (!init_app_state(&app)) {
        printf("Failed to initialize application. Exiting...\n");
        return 1;
    }
    
    // Try to load existing blockchain and wallet
    printf("Loading existing blockchain and wallet...\n");
    load_blockchain(&app);
    load_wallet(&app.miner_wallet, app.wallet_file);
    
    printf("Initialization complete!\n");
    printf("Blockchain size: %d blocks\n", app.blockchain_size);
    
    char balance_str[64];
    format_amount(get_wallet_balance(&app.miner_wallet), balance_str, sizeof(balance_str));
    printf("Wallet balance: %s\n", balance_str);
    
    if (app.network_enabled) {
        printf("Network status: Online (%d peers)\n", app.network.peer_count);
    } else {
        printf("Network status: Offline\n");
    }
    
    // Main menu loop
    MenuOption choice;
    do {
        printf("\n");
        display_main_menu();
        
        choice = get_menu_choice();
        if (choice != -1) {
            handle_menu_choice(&app, choice);
        }
        
        // Reset interrupt flag for continuous operation
        keep_running = 1;
        
    } while (choice != MENU_EXIT);
    
    // Cleanup
    printf("\nShutting down Archimedes Blockchain...\n");
    cleanup_app_state(&app);
    
    printf("Thank you for using Archimedes Blockchain!\n");
    printf("Keep calculating Pi digits and securing the network! 🥧\n");
    
    return 0;
}
