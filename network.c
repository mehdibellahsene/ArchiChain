#include "network.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

bool init_network(NetworkManager *nm, int port) {
    if (nm == NULL) return false;
    
    memset(nm, 0, sizeof(NetworkManager));
    nm->port = port;
    nm->running = false;
    nm->peer_count = 0;
    
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return false;
    }
#endif
    
    // Create server socket
    nm->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (nm->server_socket < 0) {
        printf("Failed to create server socket\n");
        return false;
    }
    
    // Set socket options
    int opt = 1;
    if (setsockopt(nm->server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        printf("setsockopt failed\n");
        return false;
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(nm->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Bind failed on port %d\n", port);
        return false;
    }
    
    // Start listening
    if (listen(nm->server_socket, 10) < 0) {
        printf("Listen failed\n");
        return false;
    }
    
    nm->running = true;
    printf("Network initialized on port %d\n", port);
    return true;
}

void shutdown_network(NetworkManager *nm) {
    if (nm == NULL) return;
    
    nm->running = false;
    
    // Close all peer connections
    for (int i = 0; i < nm->peer_count; i++) {
        if (nm->peers[i].socket > 0) {
#ifdef _WIN32
            closesocket(nm->peers[i].socket);
#else
            close(nm->peers[i].socket);
#endif
        }
    }
    
    // Close server socket
    if (nm->server_socket > 0) {
#ifdef _WIN32
        closesocket(nm->server_socket);
        WSACleanup();
#else
        close(nm->server_socket);
#endif
    }
    
    printf("Network shutdown complete\n");
}

bool add_peer(NetworkManager *nm, const char *ip, int port) {
    if (nm == NULL || ip == NULL || nm->peer_count >= MAX_PEERS) {
        return false;
    }
    
    // Check if peer already exists
    for (int i = 0; i < nm->peer_count; i++) {
        if (strcmp(nm->peers[i].ip, ip) == 0 && nm->peers[i].port == port) {
            return false; // Peer already exists
        }
    }
    
    // Add new peer
    Peer *peer = &nm->peers[nm->peer_count];
    strncpy(peer->ip, ip, sizeof(peer->ip) - 1);
    peer->port = port;
    peer->socket = -1;
    peer->connected = false;
    peer->last_seen = time(NULL);
    
    nm->peer_count++;
    printf("Added peer: %s:%d\n", ip, port);
    return true;
}

bool remove_peer(NetworkManager *nm, const char *ip, int port) {
    if (nm == NULL || ip == NULL) return false;
    
    for (int i = 0; i < nm->peer_count; i++) {
        if (strcmp(nm->peers[i].ip, ip) == 0 && nm->peers[i].port == port) {
            // Close connection if open
            if (nm->peers[i].socket > 0) {
#ifdef _WIN32
                closesocket(nm->peers[i].socket);
#else
                close(nm->peers[i].socket);
#endif
            }
            
            // Move last peer to this position
            if (i < nm->peer_count - 1) {
                nm->peers[i] = nm->peers[nm->peer_count - 1];
            }
            nm->peer_count--;
            printf("Removed peer: %s:%d\n", ip, port);
            return true;
        }
    }
    return false;
}

bool send_message_to_peer(Peer *peer, const NetworkMessage *msg) {
    if (peer == NULL || msg == NULL) return false;
    
    // Connect to peer if not connected
    if (!peer->connected) {
        peer->socket = socket(AF_INET, SOCK_STREAM, 0);
        if (peer->socket < 0) return false;
        
        struct sockaddr_in peer_addr;
        memset(&peer_addr, 0, sizeof(peer_addr));
        peer_addr.sin_family = AF_INET;
        peer_addr.sin_port = htons(peer->port);
        
        if (inet_pton(AF_INET, peer->ip, &peer_addr.sin_addr) <= 0) {
            return false;
        }
        
        if (connect(peer->socket, (struct sockaddr*)&peer_addr, sizeof(peer_addr)) < 0) {
            return false;
        }
        
        peer->connected = true;
    }
    
    // Send message
    int bytes_sent = send(peer->socket, (char*)msg, sizeof(NetworkMessage), 0);
    if (bytes_sent != sizeof(NetworkMessage)) {
        peer->connected = false;
        return false;
    }
    
    peer->last_seen = time(NULL);
    return true;
}

void broadcast_message(NetworkManager *nm, const NetworkMessage *msg) {
    if (nm == NULL || msg == NULL) return;
    
    printf("Broadcasting message type %d to %d peers\n", msg->type, nm->peer_count);
    
    for (int i = 0; i < nm->peer_count; i++) {
        if (!send_message_to_peer(&nm->peers[i], msg)) {
            printf("Failed to send message to peer %s:%d\n", 
                   nm->peers[i].ip, nm->peers[i].port);
        }
    }
}

void handle_incoming_connections(NetworkManager *nm) {
    if (nm == NULL || !nm->running) return;
    
    // This is a simplified version - in a real implementation,
    // you would use select() or epoll() to handle multiple connections
    printf("Listening for incoming connections on port %d...\n", nm->port);
}

void process_peer_messages(NetworkManager *nm) {
    if (nm == NULL || !nm->running) return;
    
    // Process messages from connected peers
    // This is a placeholder for the actual message processing logic
    printf("Processing peer messages...\n");
}

void discover_peers(NetworkManager *nm) {
    if (nm == NULL) return;
    
    // Simple peer discovery - in a real implementation, you might:
    // 1. Connect to known seed nodes
    // 2. Use DNS discovery
    // 3. Use DHT (Distributed Hash Table)
    // 4. Use mDNS for local network discovery
    
    printf("Discovering peers...\n");
    
    // Add some default peers for testing (localhost on different ports)
    if (nm->port != 8333) add_peer(nm, "127.0.0.1", 8333);
    if (nm->port != 8334) add_peer(nm, "127.0.0.1", 8334);
    if (nm->port != 8335) add_peer(nm, "127.0.0.1", 8335);
}
