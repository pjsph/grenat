#ifndef PACKETS_H
#define PACKETS_H

#include <stdlib.h>
#include <malloc.h>
#include <string.h>

#define WRONG_PACKET 1

#define ID_HELLO 0
#define ID_CHANGE_STATE 1

#define CHANGE_STATE_LEN 2

typedef struct HelloPacket {
    char *name;
} HelloPacket;

int decodeHelloPacket(char* buff, HelloPacket *result);
char *encodeHelloPacket(HelloPacket *packet);

typedef struct ChangeStatePacket {
    char next_state;
} ChangeStatePacket;

int decodeChangeStatePacket(char *buff, ChangeStatePacket *result);
char *encodeChangeStatePacket(ChangeStatePacket *packet);

void printPacket(char *buff, int len);
#endif // PACKETS_H