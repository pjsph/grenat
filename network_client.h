#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

int init_connection(void);
void deinit_connection(void);

int connect_client(char *address_name, unsigned short port);
void disconnect_client(void);

void update_client(void);

#endif // NETWORK_CLIENT_H