#include "packets.h"

int decodeHelloPacket(char* buff, HelloPacket *result) {
    if(buff[0] != ID_HELLO) {
        return WRONG_PACKET;
    }

    int i = 1;
    while(buff[i] != '\0' && ++i < 32);

    char *name = (char*) calloc(i+1, sizeof(char));
    strncpy(name, buff+1, i);

    *result = (HelloPacket) { name };

    return 0;
}

char *encodeHelloPacket(HelloPacket *packet) {
    int nameLen = strlen(packet->name);
    char *buff = (char*) malloc(sizeof(char)*(nameLen+1));

    buff[0] = ID_HELLO;
    strcpy(buff+1, packet->name);

    return buff;
}

int decodeChangeStatePacket(char *buff, ChangeStatePacket *result) {
    if(buff[0] != ID_CHANGE_STATE) {
        return WRONG_PACKET;
    }

    result->next_state = buff[1];

    return 0;
}

char *encodeChangeStatePacket(ChangeStatePacket *packet) {
    int len = 2;
    char *buff = (char*) malloc(sizeof(char)*len);

    buff[0] = ID_CHANGE_STATE;
    buff[1] = packet->next_state;

    return buff;
}

void printPacket(char *buff, int len) {
    for(int i = 0; i < len; i++) {
        for(int j = 7; j >= 0; j--) {
            printf("%u", (buff[i] >> j) & 0x01);
        }
        printf(" ");
    }
    printf("\n");
}