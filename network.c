#include "network.h"

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

int connect_client(char *address_name, enet_uint16 port, ClientSession *session) {
    session->client = enet_host_create(NULL, 1, 1, 0, 0);

    if(session->client == NULL) {
        fprintf(stderr, "An error occurred while trying to create a host!\n");
        return 1;
    }

    ENetAddress address;
    ENetEvent event;

    enet_address_set_host(&address, address_name);
    address.port = port;

    session->server = enet_host_connect(session->client, &address, 1, 0);
    if(session->server == NULL) {
        fprintf(stderr, "No available peers for initiating a connection!\n");
        return 1;
    }

    if(enet_host_service(session->client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%u succeeded.\n", address_name, port);
    } else {
        enet_peer_reset(session->server);
        printf("Connection to %s:%u failed.\n", address_name, port);
        return 1;
    }

    enet_host_service(session->client, &event, 1000);

    return 0;
}

void disconnect_client(ClientSession *client) {
    enet_peer_disconnect(client->server, 0);

    ENetEvent event;
    int success = 0;
    while(enet_host_service(client->client, &event, 3000) > 0)
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
        enet_peer_reset(client->server);
    }

    enet_host_destroy(client->client);
    free(client);
}

int create_server(char *address_name, enet_uint16 port, ENetHost **server) {
	ENetAddress address;

    enet_address_set_host(&address, address_name);
	address.port = port;

	*server = enet_host_create(&address, 2, 1, 0, 0);
	if(*server == NULL){
		printf("An error occurred while trying to create an ENet server host.\n");
		return 1;
	}

    return 0;
}

void destroy_server(ENetHost *server) {
    enet_host_destroy(server);
}