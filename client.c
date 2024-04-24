#include <stdio.h>
#include <enet/enet.h>

#include "packets.h"

typedef struct ClientSession {
    ENetHost *client;
    ENetPeer *server;
} ClientSession;

int init_connection(char *address_name, enet_uint16 port, ClientSession *session) {
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
    }

    // enet_host_flush(session->client);
    // enet_host_service(session->client, &event, 1000);

    return 0;
}

int main(int argc, char ** argv)
{
	if(enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet!\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);

    ClientSession session;
    init_connection("127.0.0.1", 7676, &session);

    ENetHost *client = session.client;
    ENetPeer *peer = session.server;

    ENetEvent event;

    int stop = 0;
    while(stop == 0) {
        while(enet_host_service(client, &event, 3000) > 0) {
            switch(event.type)
            {
                case ENET_EVENT_TYPE_RECEIVE:
                    // printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
                    //     event.packet->dataLength,
                    //     (char*)event.packet->data,
                    //     (char*)event.peer->data,
                    //     event.channelID);
                    ChangeStatePacket packet;
                    decodeChangeStatePacket(event.packet->data, &packet);
                    enet_packet_destroy(event.packet);
                    printf("Changed state to: %d\n", packet.next_state);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    puts("Disconnection succeeded here.");
                    return 0;
                    stop = 1;
                    break;
            }
        }
    }

    enet_peer_disconnect(peer, 0);

    while(enet_host_service(client, &event, 3000) > 0)
    {
        switch(event.type)
        {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Disconnection succeeded.");
                break;
        }
    }

    return EXIT_SUCCESS;
}