#ifndef NETWORK_H
#define NETWORK_H

#include <enet/enet.h>
#include <stdio.h>

typedef struct ClientSession {
    ENetHost *client;
    ENetPeer *server;
} ClientSession;

int init_connection(void);
void deinit_connection(void);

int connect_client(char *address_name, enet_uint16 port, ClientSession *session);
void disconnect_client(ClientSession *client);

int create_server(char *address_name, enet_uint16 port, ENetHost **server);
void destroy_server(ENetHost *server);

#endif // NETWORK_H