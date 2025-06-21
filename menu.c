#include "menu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>

void print_app_banner(void) {
    printf("\n");
    printf("+=================================================================================+\n");
    printf("|              ARCHIMEDES BLOCKCHAIN - Pi-Mining Cryptocurrency Network          |\n");
    printf("+=================================================================================+\n");
    printf("| The world's first blockchain that computes Pi digits as proof of work!        |\n");
    printf("| Features: P2P Network • Bitcoin-like Economics • Real Pi Calculations         |\n");
    printf("+=================================================================================+\n");
    printf("\n");
}

void display_main_menu(void) {
    printf("+==============================================================================+\n");
    printf("|                              MAIN MENU                                      |\n");
    printf("+==============================================================================+\n");
    printf("| 1. Mine New Blocks          | Mine blocks and earn ARC coins               |\n");
    printf("| 2. View Blockchain          | Browse all blocks and transactions           |\n");
    printf("| 3. Check Balance            | View your wallet balance                     |\n");
    printf("| 4. Transfer Funds           | Send ARC coins to another address           |\n");
    printf("| 5. View Wallet Details      | Show wallet info and transaction history    |\n");
    printf("| 6. Network Status           | View P2P network and peer information       |\n");
    printf("| 7. Performance Statistics   | View mining and system performance          |\n");
    printf("| 8. Sync Blockchain          | Synchronize with network peers              |\n");
    printf("| 9. Export Blockchain        | Save blockchain to file                     |\n");
    printf("| 10. Import Blockchain       | Load blockchain from file                   |\n");
    printf("| 11. Create New Wallet       | Generate a new wallet address               |\n");
    printf("| 12. Load Existing Wallet    | Load wallet from file                       |\n");
    printf("| 0. Exit                     | Quit the application                        |\n");
    printf("+==============================================================================+\n");
    printf("Enter your choice (0-12): ");
}

MenuOption get_menu_choice(void) {
    int choice;
    char input[10];
    
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (sscanf(input, "%d", &choice) == 1) {
            if (choice >= 0 && choice <= 12) {
                return (MenuOption)choice;
            }
        }
    }
    
    printf("Invalid choice! Please enter a number between 0 and 12.\n");
    return -1; // Invalid choice
}

bool init_app_state(AppState *app) {
    if (app == NULL) return false;
    
    memset(app, 0, sizeof(AppState));
    
    // Initialize blockchain
    app->blockchain_capacity = 100;
    app->blockchain = malloc(app->blockchain_capacity * sizeof(Block));
    if (app->blockchain == NULL) {
        printf("Error: Could not allocate memory for blockchain\n");
        return false;
    }
    app->blockchain_size = 0;
    
    // Initialize wallet
    if (!init_wallet(&app->miner_wallet)) {
        printf("Error: Could not initialize wallet\n");
        free(app->blockchain);
        return false;
    }
    
    // Initialize reward system
    init_reward_system(&app->reward_system);
    
    // Initialize performance monitoring
    init_performance_metrics(&app->performance);
    
    // Initialize memory pool
    if (!init_memory_pool(&app->memory_pool, 64 * 1024 * 1024)) {
        printf("Warning: Could not initialize memory pool\n");
    }
    
    // Set default filenames
    strcpy(app->blockchain_file, "archimed_blockchain.dat");
    strcpy(app->wallet_file, "archimed_wallet.dat");
    
    // Try to initialize network
    app->network_enabled = init_network(&app->network, 8333);
    if (app->network_enabled) {
        discover_peers(&app->network);
        printf("P2P Network initialized successfully\n");
    } else {
        printf("Running in offline mode (network initialization failed)\n");
    }
    
    app->performance_monitoring = true;
    
    return true;
}

void cleanup_app_state(AppState *app) {
    if (app == NULL) return;
    
    // Save blockchain and wallet before cleanup
    save_blockchain(app);
    save_wallet(&app->miner_wallet, app->wallet_file);
    
    // Cleanup blockchain
    if (app->blockchain != NULL) {
        for (int i = 0; i < app->blockchain_size; i++) {
            cleanup_block(&app->blockchain[i]);
        }
        free(app->blockchain);
    }
    
    // Cleanup network
    if (app->network_enabled) {
        shutdown_network(&app->network);
    }
    
    // Cleanup memory pool
    cleanup_memory_pool(&app->memory_pool);
    
    printf("Application cleanup completed\n");
}

void handle_menu_choice(AppState *app, MenuOption choice) {
    if (app == NULL) return;
    
    switch (choice) {
        case MENU_MINE_BLOCKS:
            handle_mine_blocks(app);
            break;
        case MENU_VIEW_BLOCKCHAIN:
            handle_view_blockchain(app);
            break;
        case MENU_CHECK_BALANCE:
            handle_check_balance(app);
            break;
        case MENU_TRANSFER_FUNDS:
            handle_transfer_funds(app);
            break;
        case MENU_VIEW_WALLET:
            handle_view_wallet(app);
            break;
        case MENU_NETWORK_STATUS:
            handle_network_status(app);
            break;
        case MENU_PERFORMANCE_STATS:
            handle_performance_stats(app);
            break;
        case MENU_SYNC_BLOCKCHAIN:
            handle_sync_blockchain(app);
            break;
        case MENU_EXPORT_BLOCKCHAIN:
            handle_export_blockchain(app);
            break;
        case MENU_IMPORT_BLOCKCHAIN:
            handle_import_blockchain(app);
            break;
        case MENU_CREATE_WALLET:
            handle_create_wallet(app);
            break;
        case MENU_LOAD_WALLET:
            handle_load_wallet(app);
            break;
        case MENU_EXIT:
            printf("Shutting down Archimedes Blockchain...\n");
            break;
        default:
            printf("Invalid menu option\n");
            break;
    }
}

void handle_mine_blocks(AppState *app) {
    if (app == NULL) return;
    
    int blocks_to_mine;
    printf("\n+==============================================================================+\n");
    printf("|                              MINING BLOCKS                                  |\n");
    printf("+==============================================================================+\n");
    printf("How many blocks would you like to mine? (0 for continuous mining): ");
    
    char input[20];
    if (fgets(input, sizeof(input), stdin) != NULL) {
        sscanf(input, "%d", &blocks_to_mine);
    } else {
        printf("Invalid input\n");
        return;
    }
    
    bool continuous = (blocks_to_mine <= 0);
    if (continuous) {
        printf("Starting continuous mining. Press 'q' and Enter to stop.\n");
    } else {
        printf("Mining %d blocks...\n", blocks_to_mine);
    }
    
    int blocks_mined = 0;
    char stop_input[10];
    
    printf("\nStarting mining operations...\n");
    
    while (continuous || blocks_mined < blocks_to_mine) {
        // Check if we need to expand blockchain capacity
        if (app->blockchain_size >= app->blockchain_capacity) {
            app->blockchain_capacity *= 2;
            Block *temp = realloc(app->blockchain, app->blockchain_capacity * sizeof(Block));
            if (temp == NULL) {
                printf("Error: Could not expand blockchain memory\n");
                break;
            }
            app->blockchain = temp;
        }
        
        int block_index = app->blockchain_size;
        Block *new_block = &app->blockchain[block_index];
        new_block->index = block_index;
        
        uint32_t prev_hash = (block_index == 0) ? 0 : app->blockchain[block_index - 1].hash;
        const Block *prev_block = (block_index == 0) ? NULL : &app->blockchain[block_index - 1];
        
        // Update reward system
        update_reward_system(&app->reward_system, block_index);
        
        // Calculate expected reward
        uint64_t expected_reward = calculate_mining_reward(&app->reward_system, block_index);
        char reward_str[64];
        format_amount(expected_reward, reward_str, sizeof(reward_str));
        
        printf("\n>> Mining Block %d\n", block_index);
        printf("   - Expected Reward: %s\n", reward_str);
        printf("   - Miner: %s\n", app->miner_wallet.address);
        
        // Start performance timing
        if (app->performance_monitoring) {
            start_timing(&app->performance);
        }
        
        // Mine the block with Pi digit proof of work
        mine_block(new_block, prev_hash, prev_block, app->miner_wallet.address, &app->reward_system);
        
        // End performance timing
        if (app->performance_monitoring) {
            end_timing(&app->performance);
            calculate_performance_stats(&app->performance, new_block->pi_digits_count, new_block->nonce + 1);
        }
        
        // Award mining reward
        if (expected_reward > 0) {
            award_mining_reward(&app->miner_wallet, &app->reward_system, block_index);
        }
        
        app->blockchain_size++;
        blocks_mined++;
        
        printf(">> Block %d mined successfully!\n", block_index);
        printf("   - Block Hash: %u\n", new_block->hash);
        printf("   - Pi Digits Calculated: %d\n", new_block->pi_digits_count);
        printf("   - Nonce: %u\n", new_block->nonce);        if (app->performance_monitoring) {
            printf("   - Mining Time: %.4f seconds\n", app->performance.mining_time);
            printf("   - Pi Digits/sec: %" PRIu64 "\n", app->performance.pi_digits_per_second);
        }
        
        // Broadcast block to network
        if (app->network_enabled && app->network.peer_count > 0) {
            NetworkMessage msg;
            msg.type = MSG_BLOCK;
            msg.length = sizeof(Block);
            broadcast_message(&app->network, &msg);
            printf("   - Block broadcasted to %d peers\n", app->network.peer_count);
        }
        
        // Check for stop command in continuous mode
        if (continuous) {
            printf("Press 'q' + Enter to stop, or just Enter to continue: ");
            if (fgets(stop_input, sizeof(stop_input), stdin) != NULL) {
                if (tolower(stop_input[0]) == 'q') {
                    break;
                }
            }
        }
    }
    
    printf("\nMining completed! Mined %d blocks.\n", blocks_mined);
    
    // Show updated wallet balance
    char balance_str[64];
    format_amount(get_wallet_balance(&app->miner_wallet), balance_str, sizeof(balance_str));
    printf("Updated wallet balance: %s\n", balance_str);
    
    // Save blockchain after mining
    if (blocks_mined > 0) {
        save_blockchain(app);
        save_wallet(&app->miner_wallet, app->wallet_file);
        printf("Blockchain and wallet saved successfully.\n");
    }
}

void handle_view_blockchain(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                            BLOCKCHAIN EXPLORER                              |\n");
    printf("+==============================================================================+\n");
    
    if (app->blockchain_size == 0) {
        printf("| Blockchain is empty. Mine some blocks first!                                |\n");
        printf("+==============================================================================+\n");
        return;
    }
    
    printf("| Total Blocks: %-10d                                                |\n", app->blockchain_size);
    printf("| Blockchain File: %-50s        |\n", app->blockchain_file);
    printf("+==============================================================================+\n");
    
    printf("\nChoose viewing option:\n");
    printf("1. View all blocks\n");
    printf("2. View specific block\n");
    printf("3. View block summary\n");
    printf("4. Search by hash\n");
    printf("Enter choice (1-4): ");
    
    char input[10];
    int choice;
    if (fgets(input, sizeof(input), stdin) != NULL) {
        sscanf(input, "%d", &choice);
    } else {
        return;
    }
    
    switch (choice) {
        case 1: {
            // View all blocks
            printf("\nDisplaying all %d blocks:\n", app->blockchain_size);
            for (int i = 0; i < app->blockchain_size; i++) {
                print_block(&app->blockchain[i]);
                if ((i + 1) % 3 == 0 && i < app->blockchain_size - 1) {
                    printf("Press Enter to continue or 'q' to quit...");
                    if (fgets(input, sizeof(input), stdin) != NULL) {
                        if (tolower(input[0]) == 'q') break;
                    }
                }
            }
            break;
        }
        case 2: {
            // View specific block
            printf("Enter block index (0-%d): ", app->blockchain_size - 1);
            int block_index;
            if (fgets(input, sizeof(input), stdin) != NULL) {
                if (sscanf(input, "%d", &block_index) == 1) {
                    if (block_index >= 0 && block_index < app->blockchain_size) {
                        print_block(&app->blockchain[block_index]);
                    } else {
                        printf("Block index out of range\n");
                    }
                }
            }
            break;
        }
        case 3: {
            // View block summary
            printf("\n+==============================================================================+\n");
            printf("|                           BLOCKCHAIN SUMMARY                                |\n");
            printf("+==============================================================================+\n");
            
            uint64_t total_rewards = 0;
            int total_pi_digits = 0;
            
            for (int i = 0; i < app->blockchain_size; i++) {
                total_rewards += app->blockchain[i].mining_reward;
                total_pi_digits += app->blockchain[i].pi_digits_count;
                
                printf("| Block %3d | Hash: %10u | Pi: %6d | Reward: ", 
                       i, app->blockchain[i].hash, app->blockchain[i].pi_digits_count);
                
                char reward_str[32];
                format_amount(app->blockchain[i].mining_reward, reward_str, sizeof(reward_str));
                printf("%-20s |\n", reward_str);
            }
            
            printf("+==============================================================================+\n");
            char total_reward_str[64];
            format_amount(total_rewards, total_reward_str, sizeof(total_reward_str));
            printf("| Total Pi Digits: %-10d | Total Rewards: %-30s |\n", 
                   total_pi_digits, total_reward_str);
            printf("+==============================================================================+\n");
            break;
        }
        case 4: {
            // Search by hash
            printf("Enter block hash to search: ");
            uint32_t search_hash;
            if (fgets(input, sizeof(input), stdin) != NULL) {
                if (sscanf(input, "%u", &search_hash) == 1) {
                    bool found = false;
                    for (int i = 0; i < app->blockchain_size; i++) {
                        if (app->blockchain[i].hash == search_hash) {
                            printf("Block found at index %d:\n", i);
                            print_block(&app->blockchain[i]);
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        printf("Block with hash %u not found\n", search_hash);
                    }
                }
            }
            break;
        }
        default:
            printf("Invalid choice\n");
            break;
    }
}

void handle_check_balance(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                            BALANCE CHECK                                    |\n");
    printf("+==============================================================================+\n");
    
    char balance_str[64];
    format_amount(get_wallet_balance(&app->miner_wallet), balance_str, sizeof(balance_str));
    
    printf("| Wallet Address: %-56s |\n", app->miner_wallet.address);
    printf("| Current Balance: %-55s |\n", balance_str);
    printf("| Total Transactions: %-50d |\n", app->miner_wallet.transaction_count);
    printf("+==============================================================================+\n");
    
    // Show recent transactions
    if (app->miner_wallet.transaction_count > 0) {
        printf("\nRecent Transactions (last 5):\n");
        printf("+==============================================================================+\n");
        int start = (app->miner_wallet.transaction_count > 5) ? 
                   app->miner_wallet.transaction_count - 5 : 0;
        
        for (int i = start; i < app->miner_wallet.transaction_count; i++) {
            print_transaction(&app->miner_wallet.transactions[i]);
        }
        printf("+==============================================================================+\n");
    }
}

void handle_transfer_funds(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                           TRANSFER FUNDS                                    |\n");
    printf("+==============================================================================+\n");
    
    char balance_str[64];
    format_amount(get_wallet_balance(&app->miner_wallet), balance_str, sizeof(balance_str));
    printf("| Your Current Balance: %-51s |\n", balance_str);
    printf("+==============================================================================+\n");
    
    if (get_wallet_balance(&app->miner_wallet) == 0) {
        printf("You have no funds to transfer. Mine some blocks first!\n");
        return;
    }
    
    char to_address[WALLET_ADDRESS_LENGTH];
    char amount_str[64];
    
    printf("Enter recipient address: ");
    if (fgets(to_address, sizeof(to_address), stdin) != NULL) {
        // Remove newline
        to_address[strcspn(to_address, "\n")] = 0;
    } else {
        printf("Invalid address input\n");
        return;
    }
    
    printf("Enter amount to transfer (in ARC): ");
    if (fgets(amount_str, sizeof(amount_str), stdin) != NULL) {
        // Remove newline
        amount_str[strcspn(amount_str, "\n")] = 0;
    } else {
        printf("Invalid amount input\n");
        return;
    }
    
    uint64_t amount = parse_amount(amount_str);
    if (amount == 0) {
        printf("Invalid amount\n");
        return;
    }
    
    if (amount > get_wallet_balance(&app->miner_wallet)) {
        printf("Insufficient funds\n");
        return;
    }
    
    // Create transaction
    Transaction tx;
    if (create_transaction(&tx, app->miner_wallet.address, to_address, amount)) {
        printf("\nTransaction created successfully!\n");
        printf("Transaction Hash: %u\n", tx.hash);
        
        char tx_amount_str[64];
        format_amount(amount, tx_amount_str, sizeof(tx_amount_str));
        printf("Amount: %s\n", tx_amount_str);
        printf("From: %s\n", tx.from_address);
        printf("To: %s\n", tx.to_address);
        
        // Add transaction to wallet
        add_transaction_to_wallet(&app->miner_wallet, &tx);
        
        // In a real blockchain, you would broadcast this transaction to the network
        // and wait for it to be included in a block
        if (app->network_enabled && app->network.peer_count > 0) {
            NetworkMessage msg;
            msg.type = MSG_TRANSACTION;
            msg.length = sizeof(Transaction);
            // In practice, you'd serialize the transaction properly
            broadcast_message(&app->network, &msg);
            printf("Transaction broadcasted to network\n");
        }
        
        // Save updated wallet
        save_wallet(&app->miner_wallet, app->wallet_file);
        
        printf("Transaction completed successfully!\n");
    } else {
        printf("Failed to create transaction\n");
    }
}

void handle_view_wallet(AppState *app) {
    if (app == NULL) return;
    
    printf("\n");
    print_wallet_info(&app->miner_wallet);
    
    if (app->miner_wallet.transaction_count > 0) {
        printf("\nTransaction History:\n");
        printf("+==============================================================================+\n");
        for (int i = 0; i < app->miner_wallet.transaction_count; i++) {
            printf("| %3d. ", i + 1);
            print_transaction(&app->miner_wallet.transactions[i]);
        }
        printf("+==============================================================================+\n");
    } else {
        printf("No transactions found.\n");
    }
}

void handle_network_status(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                            NETWORK STATUS                                   |\n");
    printf("+==============================================================================+\n");
    
    if (!app->network_enabled) {
        printf("| Network Status: OFFLINE                                                     |\n");
        printf("| P2P networking is disabled or failed to initialize                          |\n");
        printf("+==============================================================================+\n");
        return;
    }
    
    printf("| Network Status: %-20s                                        |\n", 
           app->network.running ? "ONLINE" : "OFFLINE");
    printf("| Listening Port: %-10d                                              |\n", 
           app->network.port);
    printf("| Connected Peers: %-10d                                             |\n", 
           app->network.peer_count);
    printf("+==============================================================================+\n");
    
    if (app->network.peer_count > 0) {
        printf("|                              PEER LIST                                      |\n");
        printf("+==============================================================================+\n");
        for (int i = 0; i < app->network.peer_count; i++) {
            Peer *peer = &app->network.peers[i];
            printf("| %2d. %15s:%-5d | Status: %-10s | Last Seen: %10ld |\n",
                   i + 1, peer->ip, peer->port, 
                   peer->connected ? "Connected" : "Disconnected",
                   (long)peer->last_seen);
        }
        printf("+==============================================================================+\n");
    }
    
    printf("\nNetwork Commands:\n");
    printf("1. Discover new peers\n");
    printf("2. Add manual peer\n");
    printf("3. Remove peer\n");
    printf("4. Refresh network status\n");
    printf("Enter choice (1-4, or 0 to return): ");
    
    char input[10];
    int choice;
    if (fgets(input, sizeof(input), stdin) != NULL) {
        sscanf(input, "%d", &choice);
        
        switch (choice) {
            case 1:
                discover_peers(&app->network);
                printf("Peer discovery initiated\n");
                break;
            case 2: {                char ip[16];
                int port;
                printf("Enter peer IP: ");
                if (fgets(ip, sizeof(ip), stdin) != NULL) {
                    ip[strcspn(ip, "\n")] = 0;
                }
                printf("Enter peer port: ");
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    sscanf(input, "%d", &port);
                }
                if (add_peer(&app->network, ip, port)) {
                    printf("Peer added successfully\n");
                } else {
                    printf("Failed to add peer\n");
                }
                break;
            }
            case 3: {
                char ip[16];
                int port;
                printf("Enter peer IP to remove: ");                if (fgets(ip, sizeof(ip), stdin) != NULL) {
                    ip[strcspn(ip, "\n")] = 0;
                }
                printf("Enter peer port: ");
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    sscanf(input, "%d", &port);
                }
                if (remove_peer(&app->network, ip, port)) {
                    printf("Peer removed successfully\n");
                } else {
                    printf("Peer not found\n");
                }
                break;
            }
            case 4:
                printf("Network status refreshed\n");
                break;
            default:
                break;
        }
    }
}

void handle_performance_stats(AppState *app) {
    if (app == NULL) return;
    
    printf("\n");
    if (app->performance_monitoring) {
        print_performance_report(&app->performance);
    } else {
        printf("Performance monitoring is disabled\n");
    }
    
    // Memory pool statistics
    printf("+==============================================================================+\n");
    printf("|                           MEMORY STATISTICS                                 |\n");
    printf("+==============================================================================+\n");
    if (app->memory_pool.initialized) {
        printf("| Memory Pool Size: %-10zu bytes                                      |\n", 
               app->memory_pool.pool_size);
        printf("| Memory Pool Used: %-10zu bytes                                      |\n", 
               app->memory_pool.used_size);
        printf("| Memory Available: %-10zu bytes                                      |\n", 
               app->memory_pool.pool_size - app->memory_pool.used_size);
    } else {
        printf("| Memory Pool: Not initialized                                             |\n");
    }
    printf("+==============================================================================+\n");
}

void handle_sync_blockchain(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                        BLOCKCHAIN SYNCHRONIZATION                           |\n");
    printf("+==============================================================================+\n");
    
    if (!app->network_enabled) {
        printf("| Network is offline. Cannot synchronize with peers.                          |\n");
        printf("+==============================================================================+\n");
        return;
    }
    
    if (app->network.peer_count == 0) {
        printf("| No peers connected. Cannot synchronize blockchain.                          |\n");
        printf("+==============================================================================+\n");
        return;
    }
    
    printf("| Current blockchain size: %-10d blocks                               |\n", 
           app->blockchain_size);
    printf("| Connected peers: %-10d                                              |\n", 
           app->network.peer_count);
    printf("+==============================================================================+\n");
    
    printf("Requesting blockchain from peers...\n");
    
    // Send blockchain request to all peers
    NetworkMessage msg;
    msg.type = MSG_BLOCKCHAIN_REQUEST;
    msg.length = 0;
    broadcast_message(&app->network, &msg);
    
    printf("Blockchain sync request sent to %d peers\n", app->network.peer_count);
    printf("Note: In a full implementation, this would wait for peer responses\n");
    printf("and merge any longer valid chains received from the network.\n");
}

void handle_export_blockchain(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                          EXPORT BLOCKCHAIN                                  |\n");
    printf("+==============================================================================+\n");
    
    if (save_blockchain(app)) {
        printf("| Blockchain exported successfully to: %-38s |\n", app->blockchain_file);
        printf("| Blocks exported: %-10d                                           |\n", 
               app->blockchain_size);
    } else {
        printf("| Failed to export blockchain                                                 |\n");
    }
    printf("+==============================================================================+\n");
}

void handle_import_blockchain(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                          IMPORT BLOCKCHAIN                                  |\n");
    printf("+==============================================================================+\n");
    
    printf("Enter blockchain file path (or press Enter for default): ");
    char filename[256];
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        filename[strcspn(filename, "\n")] = 0;
        if (strlen(filename) > 0) {
            strcpy(app->blockchain_file, filename);
        }
    }
    
    int old_size = app->blockchain_size;
    if (load_blockchain(app)) {
        printf("| Blockchain imported successfully from: %-36s |\n", app->blockchain_file);
        printf("| Blocks imported: %-10d                                           |\n", 
               app->blockchain_size - old_size);
    } else {
        printf("| Failed to import blockchain from: %-40s |\n", app->blockchain_file);
    }
    printf("+==============================================================================+\n");
}

void handle_create_wallet(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                           CREATE NEW WALLET                                 |\n");
    printf("+==============================================================================+\n");
    
    Wallet new_wallet;
    if (init_wallet(&new_wallet)) {
        printf("| New wallet created successfully!                                            |\n");
        printf("| Address: %-62s |\n", new_wallet.address);
        printf("| Private Key: %-58s |\n", new_wallet.private_key);
        printf("+==============================================================================+\n");
        
        printf("\nWould you like to:\n");
        printf("1. Replace current wallet with new wallet\n");
        printf("2. Save new wallet to file\n");
        printf("3. Just display info (don't save)\n");
        printf("Enter choice (1-3): ");
        
        char input[10];
        int choice;
        if (fgets(input, sizeof(input), stdin) != NULL) {
            sscanf(input, "%d", &choice);
            
            switch (choice) {
                case 1:
                    app->miner_wallet = new_wallet;
                    save_wallet(&app->miner_wallet, app->wallet_file);
                    printf("Current wallet replaced and saved\n");
                    break;
                case 2: {
                    printf("Enter filename for new wallet: ");
                    char filename[256];
                    if (fgets(filename, sizeof(filename), stdin) != NULL) {
                        filename[strcspn(filename, "\n")] = 0;
                        if (save_wallet(&new_wallet, filename)) {
                            printf("New wallet saved to: %s\n", filename);
                        } else {
                            printf("Failed to save wallet\n");
                        }
                    }
                    break;
                }
                case 3:
                default:
                    printf("Wallet info displayed only\n");
                    break;
            }
        }
    } else {
        printf("| Failed to create new wallet                                                 |\n");
        printf("+==============================================================================+\n");
    }
}

void handle_load_wallet(AppState *app) {
    if (app == NULL) return;
    
    printf("\n+==============================================================================+\n");
    printf("|                           LOAD EXISTING WALLET                              |\n");
    printf("+==============================================================================+\n");
    
    printf("Enter wallet file path (or press Enter for default): ");
    char filename[256];
    if (fgets(filename, sizeof(filename), stdin) != NULL) {
        filename[strcspn(filename, "\n")] = 0;
        if (strlen(filename) == 0) {
            strcpy(filename, app->wallet_file);
        }
    } else {
        return;
    }
    
    Wallet loaded_wallet;
    if (load_wallet(&loaded_wallet, filename)) {
        printf("| Wallet loaded successfully from: %-41s |\n", filename);
        printf("| Address: %-62s |\n", loaded_wallet.address);
        
        char balance_str[64];
        format_amount(get_wallet_balance(&loaded_wallet), balance_str, sizeof(balance_str));
        printf("| Balance: %-62s |\n", balance_str);
        printf("| Transactions: %-57d |\n", loaded_wallet.transaction_count);
        printf("+==============================================================================+\n");
        
        printf("Replace current wallet with loaded wallet? (y/n): ");
        char input[10];
        if (fgets(input, sizeof(input), stdin) != NULL) {
            if (tolower(input[0]) == 'y') {
                app->miner_wallet = loaded_wallet;
                strcpy(app->wallet_file, filename);
                printf("Wallet replaced successfully\n");
            }
        }
    } else {
        printf("| Failed to load wallet from: %-46s |\n", filename);
        printf("+==============================================================================+\n");
    }
}

// File I/O functions
bool save_blockchain(const AppState *app) {
    if (app == NULL || app->blockchain == NULL) return false;
    
    FILE *file = fopen(app->blockchain_file, "wb");
    if (file == NULL) return false;
    
    // Write blockchain size
    fwrite(&app->blockchain_size, sizeof(int), 1, file);
    
    // Write each block (without Pi digits to save space)
    for (int i = 0; i < app->blockchain_size; i++) {
        Block block_copy = app->blockchain[i];
        // Don't save Pi digits to reduce file size
        block_copy.pi_digits = NULL;
        fwrite(&block_copy, sizeof(Block), 1, file);
    }
    
    fclose(file);
    return true;
}

bool load_blockchain(AppState *app) {
    if (app == NULL) return false;
    
    FILE *file = fopen(app->blockchain_file, "rb");
    if (file == NULL) return false;
    
    int saved_size;
    if (fread(&saved_size, sizeof(int), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    // Expand blockchain capacity if needed
    if (saved_size > app->blockchain_capacity) {
        app->blockchain_capacity = saved_size * 2;
        Block *temp = realloc(app->blockchain, app->blockchain_capacity * sizeof(Block));
        if (temp == NULL) {
            fclose(file);
            return false;
        }
        app->blockchain = temp;
    }
    
    // Read blocks
    for (int i = app->blockchain_size; i < saved_size; i++) {
        if (fread(&app->blockchain[i], sizeof(Block), 1, file) != 1) {
            fclose(file);
            return false;
        }
        // Pi digits are not saved/loaded to save space
        app->blockchain[i].pi_digits = NULL;
        app->blockchain[i].pi_digits_count = 0;
    }
    
    app->blockchain_size = saved_size;
    fclose(file);
    return true;
}

bool save_wallet(const Wallet *wallet, const char *filename) {
    if (wallet == NULL || filename == NULL) return false;
    
    FILE *file = fopen(filename, "wb");
    if (file == NULL) return false;
    
    fwrite(wallet, sizeof(Wallet), 1, file);
    fclose(file);
    return true;
}

bool load_wallet(Wallet *wallet, const char *filename) {
    if (wallet == NULL || filename == NULL) return false;
    
    FILE *file = fopen(filename, "rb");
    if (file == NULL) return false;
    
    if (fread(wallet, sizeof(Wallet), 1, file) != 1) {
        fclose(file);
        return false;
    }
    
    fclose(file);
    return true;
}
