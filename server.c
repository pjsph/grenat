#include <stdio.h>
#include <string.h>

#include "network.h"
#include "packets.h"

#include <windows.h>

#define CONNECTION_PHASE 0
#define PLAY_PHASE 1

char *ltrim(char *str) {
	int len;
	char *curr;

	if(str && *str) {
		len = strlen(str);
		curr = str;

		while(*curr && isspace(*curr))
			++curr, --len;
		
		if(str != curr)
			memmove(str, curr, len + 1);
	}

	return str;
}

char *rtrim(char *str) {
	int len;
	char *curr;

	if(str && *str) {
		len = strlen(str);
		curr = str + len - 1;

		while(curr != str && isspace(*curr))
			--curr, --len;
		
		curr[isspace(*curr) ? 0 : 1] = '\0';
	}

	return str;
}

char *trim(char *str) {
	rtrim(str);
	ltrim(str);

	return str;
}

DWORD WINAPI ConsoleInput(int *stop) {
	char input[256];
	
	while(*stop == 0) {
		printf("> ");
		if(fgets(input, 256, stdin) != NULL) {
			if(strcmp(trim(input), "") == 0) {
				
			} else if(strcmp(trim(input), "stop") == 0) {
				*stop = 1;
			} else if(strcmp(trim(input), "help") == 0) {
				printf("- help		-- show this message\n");
				printf("- stop		-- stop the server\n");
			} else {
				printf("Command not found. Please try again.\n");
			}
		}
	}

	return 0;
}

void connection_phase(ENetHost *server, ENetEvent *event, ENetPeer *clients[2], int *current_phase) {
	while(*current_phase == CONNECTION_PHASE && enet_host_service(server, event, 1000) > 0) {
		switch(event->type) {
			case ENET_EVENT_TYPE_CONNECT:
				char client_address_name[16];
				enet_address_get_host_ip(&event->peer->address, client_address_name, sizeof(client_address_name));
				printf("A new client connected from %s:%u.\n",
				client_address_name,
				event->peer->address.port);

				for(int i = 0; i < 2; i++) {
					if(clients[i] == NULL) {
						clients[i] = event->peer;
						char *name = (char *)calloc(9, sizeof(char));
						snprintf(name, 9, "Player %d", i+1);
						event->peer->data = name;
						break;
					}
				}
				int all_connected = 1;
				for(int i = 0; i < 2; i++) {
					if(clients[i] == NULL) {
						all_connected = 0;
						break;
					}
				}
				if(all_connected) {
					*current_phase = PLAY_PHASE;
					printf("Starting the game...\n");
					ChangeStatePacket startPacket = { PLAY_PHASE };
					char *data = encodeChangeStatePacket(&startPacket);
					ENetPacket *startPkt = enet_packet_create(data, CHANGE_STATE_LEN, ENET_PACKET_FLAG_RELIABLE);
					free(data);
					enet_host_broadcast(server, 0, startPkt);
					// Sleep(10);
				}
			break;

			case ENET_EVENT_TYPE_RECEIVE:
				// printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
				// 	event.packet->dataLength,
				// 	(char*)event.packet->data,
				// 	(char*)event.peer->data,
				// 	event.channelID);
				enet_packet_destroy(event->packet);
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", (char*)event->peer->data);
				for(int i = 0; i < 2; i++) {
					if(clients[i] == event->peer) {
						free(event->peer->data);
						clients[i] = NULL;
						break;
					}
				}
				event->peer->data = NULL;
		}
	}
}

void play_phase(ENetHost *server, ENetEvent *event, ENetPeer *clients[2], int *current_phase) {
	while(*current_phase == PLAY_PHASE && enet_host_service(server, event, 1000) > 0) {
		switch(event->type) {
			case ENET_EVENT_TYPE_RECEIVE:
				printf("A packet of length %zu containing %s was received from %s on channel %u.\n",
					event->packet->dataLength,
					(char*)event->packet->data,
					(char*)event->peer->data,
					event->channelID);
				enet_packet_destroy(event->packet);
			break;

			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", (char*)event->peer->data);
				free(event->peer->data);
				for(int i = 0; i < 2; i++) {
					if(clients[i] == event->peer) {
						clients[i] = NULL;
						break;
					}
				}
				event->peer->data = NULL;
				*current_phase = CONNECTION_PHASE;
				printf("A player disconnected. The game is reset.\n");
				ChangeStatePacket resetPacket = { CONNECTION_PHASE };
				char *data = encodeChangeStatePacket(&resetPacket);
				ENetPacket *resetPkt = enet_packet_create(data, CHANGE_STATE_LEN, ENET_PACKET_FLAG_RELIABLE);
				free(data);
				enet_host_broadcast(server, 0, resetPkt);
			break;
		}
	}
}

void disconnect_all(ENetHost *server, ENetEvent *event, ENetPeer *clients[2]) {
	int all_disconnected = 1;

	for(int i = 0; i < 2; i++) {
		if(clients[i] != NULL) {
			all_disconnected = 0;
			enet_peer_disconnect(clients[i], 0);
		}
	}

	while(all_disconnected == 0 && enet_host_service(server, event, 3000) > 0) {
		switch(event->type) {
			case ENET_EVENT_TYPE_RECEIVE:
				enet_packet_destroy(event->packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				printf("%s disconnected.\n", (char*)event->peer->data);
				free(event->peer->data);
				for(int i = 0; i < 2; i++) {
					if(clients[i] == event->peer) {
						clients[i] = NULL;
						break;
					}
				}
				all_disconnected = 1;
				for(int i = 0; i < 2; i++) {
					if(clients[i] != NULL) {
						all_disconnected = 0;
						break;
					}
				}
				break;
		}
	}

	for(int i = 0; i < 2; i++) {
		if(clients[i] != NULL) {
			enet_peer_reset(clients[i]);
		}
	}
}

int main(void) {
	printf("Starting server...\n");

	int res = 0;
	int stop = 0;

	res = init_connection();
	if(res != 0) {
		return 1;
	}

	ENetEvent event;
	ENetHost *server;

	res = create_server("0.0.0.0", 7676, &server);
	if(res != 0) {
		return 1;
	}

	HANDLE console_thread = CreateThread(NULL, 0, ConsoleInput, &stop, 0, NULL);

	char address_name[16];
	enet_address_get_host_ip(&server->address, address_name, sizeof(address_name));
	printf("Server started! Listening on %s:%u.\nType 'stop' to quit.\n", address_name, server->address.port);

	int current_phase = CONNECTION_PHASE;
	ENetPeer *clients[2] = { NULL, NULL };

	while(stop == 0) {
		if(current_phase == CONNECTION_PHASE) {
			connection_phase(server, &event, clients, &current_phase);
		} else if(current_phase == PLAY_PHASE) {
			play_phase(server, &event, clients, &current_phase);
		}
	}

	printf("Stopping server...\n");

	disconnect_all(server, &event, clients);

	destroy_server(server);
	deinit_connection();

	// WaitForSingleObject(console_thread, INFINITE);
	TerminateThread(console_thread, 0);
	CloseHandle(console_thread);

	return 0;
}