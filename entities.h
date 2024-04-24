#ifndef ENTITIES_H 
#define ENTITIES_H

#include <raylib.h>

typedef struct Player {
    int id;
    Vector2 position;
    Vector2 velocity;
    Texture2D *texture;
    int wasOnGround;
} Player;

Player createPlayer(int type, Texture2D *texture);
void deletePlayer(Player *player);
void inputPlayer(Player *player);
void moveAndCheckPlayer(Player *player, int *levelDim, int *floors, int *walls);

typedef struct Grenade {
    int id;
    Vector2 position;
    Vector2 velocity;
    Texture2D *texture;
    Player *owner;
    unsigned char bounces;
    char dead;
} Grenade;

Grenade createGrenade(int id, Player *owner, Texture2D *texture);
void deleteGrenade(Grenade *grenade);
void updateGrenade(Grenade *grenade, int *levelDim, int *floors, int *walls);

int storeGrenade(Grenade *array, char arraySize, char *takenIndices, Grenade *obj);
Grenade *unstoreGrenade(Grenade *array, char arraySize, char *takenIndices, int id);

#endif // ENTITIES_H