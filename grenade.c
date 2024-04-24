#include "entities.h"

Grenade createGrenade(int id, Player *owner, Texture2D *texture) {
    Vector2 position = { 0.0f, 0.0f };
    Vector2 velocity = { 0.0f, 0.0f };
    return (Grenade) { id, position, velocity, texture, owner, 0, 0 };
}

void deleteGrenade(Grenade *grenade) {
    // Maybe set all fields to 0?
}

void bounceGrenade(Grenade *grenade) {
    if(grenade->dead) return;

    grenade->bounces++;
    if(grenade->bounces == 3) {
        grenade->dead = 1;
    }
}

void updateGrenade(Grenade *grenade, int *levelDim, int *floors, int *walls) {
    if(grenade->dead) return;

    if(grenade->velocity.x != 0.0f) {
        grenade->position.x += grenade->velocity.x;

        int neighborTiles[8];
        getNeighboringTiles(&grenade->position, levelDim[0], 0, neighborTiles); // TODO: compute only x neighboringTiles

        // printf("%d,%d,%d,%d\n", neighborTiles[0], neighborTiles[1], neighborTiles[2], neighborTiles[3]);

        if(walls[neighborTiles[0]] || walls[neighborTiles[7]]) {
            grenade->position.x = (neighborTiles[0] % levelDim[0] + 1) * 40;
            grenade->velocity.x *= -1.0f;
            bounceGrenade(grenade);
        }

        if(walls[neighborTiles[3]] || walls[neighborTiles[4]]) {
            grenade->position.x = (neighborTiles[3] % levelDim[0] - 1) * 40;
            grenade->velocity.x *= -1.0f;
            bounceGrenade(grenade);
        }

        grenade->velocity.x += grenade->velocity.x > 0 ? -0.5f : 0.5f;
        if(grenade->velocity.x <= 0.5f && grenade->velocity.x >= -0.05f) {
            grenade->velocity.x = 0.0f;
        }
    }

    grenade->position.y += grenade->velocity.y;

    int neighborTiles[8];
    getNeighboringTiles(&grenade->position, levelDim[0], 0, neighborTiles);

    if(floors[neighborTiles[1]] || floors[neighborTiles[2]]) {
        grenade->position.y = (neighborTiles[1] / levelDim[0] + 1) * 40;
        grenade->velocity.y *= -0.8f;
        bounceGrenade(grenade);
    }

    if(floors[neighborTiles[5]] || floors[neighborTiles[6]]) {
        grenade->position.y = (neighborTiles[5] / levelDim[0] - 1) * 40;
        // grenade->velocity.y += 2; // ugly fix
        grenade->velocity.y *= -0.8f;
        bounceGrenade(grenade);
    }

    grenade->velocity.y += 2;
    if(grenade->velocity.y <= 0.5f && grenade->velocity.y >= -0.5f) {
        grenade->velocity.y = 0.0f;
    }
}

int storeGrenade(Grenade *array, char arraySize, char *takenIndices, Grenade *obj) {
    for(char i = 0; i < arraySize; i++) {
        if(!takenIndices[i]) {
            takenIndices[i] = 1;
            array[i] = *obj;
            array[i].id = 10;
            return 0;
        }
    }

    return 1;
}

Grenade *unstoreGrenade(Grenade *array, char arraySize, char *takenIndices, int id) {
    for(char i = 0; i < arraySize; i++) {
        if(takenIndices[i] && array[i].id == id) {
            takenIndices[i] = 0;
            return &array[i];
        }
    }

    return NULL;
}