#ifndef NETWORK_H
#define NETWORK_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_PEERS 50
#define DEFAULT_PORT 8333
#define BUFFER_SIZE 4096
#define MAX_MESSAGE_SIZE 1024

// Network message types
typedef enum {
    MSG_HANDSHAKE = 1,
    MSG_BLOCK,
    MSG_TRANSACTION,
    MSG_PEER_REQUEST,
    MSG_PEER_RESPONSE,
    MSG_BLOCKCHAIN_REQUEST,
    MSG_BLOCKCHAIN_RESPONSE,
    MSG_PING,
    MSG_PONG
} MessageType;

// Network message structure
typedef struct {
    MessageType type;
    uint32_t length;
    uint8_t data[MAX_MESSAGE_SIZE];
} NetworkMessage;

// Peer information
typedef struct {
    char ip[16];
    int port;
    int socket;
    bool connected;
    time_t last_seen;
} Peer;

// Network manager
typedef struct {
    Peer peers[MAX_PEERS];
    int peer_count;
    int server_socket;
    bool running;
    int port;
} NetworkManager;

// Function declarations
bool init_network(NetworkManager *nm, int port);
void shutdown_network(NetworkManager *nm);
bool add_peer(NetworkManager *nm, const char *ip, int port);
bool remove_peer(NetworkManager *nm, const char *ip, int port);
void broadcast_message(NetworkManager *nm, const NetworkMessage *msg);
bool send_message_to_peer(Peer *peer, const NetworkMessage *msg);
void handle_incoming_connections(NetworkManager *nm);
void process_peer_messages(NetworkManager *nm);
void discover_peers(NetworkManager *nm);

#endif
