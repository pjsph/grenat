#include "network_client.h"

#include <enet/enet.h>
#include <stdio.h>

ENetAddress address = { 0 };
ENetPeer *server = { 0 };
ENetHost *client = { 0 };

int init_connection(void) {
	if(enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return 1;
	}
    return 0;
}

void deinit_connection(void) {
    enet_deinitialize();
}

int connect_client(char *address_name, unsigned short port) {
    client = enet_host_create(NULL, 1, 1, 0, 0);

    if(client == NULL) {
        fprintf(stderr, "An error occurred while trying to create a host!\n");
        return 1;
    }

    enet_address_set_host(&address, address_name);
    address.port = (enet_uint16)port;

    server = enet_host_connect(client, &address, 1, 0);
    if(server == NULL) {
        fprintf(stderr, "No available peers for initiating a connection!\n");
        return 1;
    }

    // if(enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
    //     printf("Connection to %s:%u succeeded.\n", address_name, port);
    // } else {
    //     enet_peer_reset(server);
    //     printf("Connection to %s:%u failed.\n", address_name, port);
    //     return 1;
    // }

    // enet_host_service(client, &event, 1000);

    return 0;
}

void disconnect_client(void) {
    if(server != NULL)
        enet_peer_disconnect(server, 0);

    ENetEvent event;

    if(client != NULL) {
        int success = 0;
        if(enet_host_service(client, &event, 3000) > 0)
        {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    puts("Disconnection succeeded.");
                    success = 1;
                    break;
            }
        }

        if(success == 0) {
            enet_peer_reset(server);
        }

        enet_host_destroy(client);
    }

    client = NULL;
    server = NULL;
}

char is_connected(void) {
    return server != NULL;
}

void update_client(void) {
    if(server == NULL)
        return;
    
    ENetEvent event;

    if(enet_host_service(client, &event, 0) > 0) {
        switch(event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                server = NULL;
                break;
        }
    }
}