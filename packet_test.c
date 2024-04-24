#include "packets.h"

int main(void) {
    char *name = "tamerell";
    HelloPacket hello = { name };

    char *buff = encodeHelloPacket(&hello);

    printPacket(buff, 9);

    HelloPacket resPacket;
    int res = decodeHelloPacket(buff, &resPacket);
    
    printf("res = %d, name = %s\n", res, resPacket.name);

    return 0;
}