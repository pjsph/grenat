#include "entities.h"

Player createPlayer(int type, Texture2D *texture) {
    int id = 0; // TODO: hashmap or list or array
    Vector2 position = { 0.0f, 0.0f };
    Vector2 velocity = { 0.0f, 0.0f };
    return (Player) { id, position, velocity, texture, 0 };
}

void deletePlayer(Player *player) {
    // ??
}

void inputPlayer(Player *player) {
    if(IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT)) {
        // currentFrame = 0;
        if(player->velocity.x < 10) {
            player->velocity.x = 10;
        }
    } else if(IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT)) {
        // currentFrame = 0;
        if(player->velocity.x > -10) {
            player->velocity.x = -10;
        }
    } else {
        if(player->velocity.x <= 10 && player->velocity.x >= -10) {
            player->velocity.x = 0;
        }
    }
    if(IsKeyDown(KEY_SPACE) && player->wasOnGround) {
        // currentFrame = 0;
        player->velocity.y = -20;
    }
    if(IsKeyPressed(KEY_LEFT_SHIFT)) {
        player->velocity.x *= 10;
    } else if(player->velocity.x > 10) {
        player->velocity.x -= player->velocity.x * player->velocity.x / 150;
        if(player->velocity.x < 10.1f) {
            player->velocity.x = 10;
        }
    } else if(player->velocity.x < -10) {
        player->velocity.x += player->velocity.x * player->velocity.x / 150;
        if(player->velocity.x > -10.1f) {
            player->velocity.x = -10;
        }
    }
}

void moveAndCheckPlayer(Player *player, int *levelDim, int *floors, int *walls) {
    player->position.x += player->velocity.x;

    int neighborTiles[8];
    getNeighboringTiles(&player->position, levelDim[0], 0, neighborTiles); // TODO: compute only x neighboringTiles

    // printf("%d,%d,%d,%d\n", neighborTiles[0], neighborTiles[1], neighborTiles[2], neighborTiles[3]);

    if(walls[neighborTiles[0]] || walls[neighborTiles[7]]) {
        player->position.x = (neighborTiles[0] % levelDim[0] + 1) * 40;
        player->velocity.x = 0;
    }

    if(walls[neighborTiles[3]] || walls[neighborTiles[4]]) {
        player->position.x = (neighborTiles[3] % levelDim[0] - 1) * 40;
        player->velocity.x = 0;
    }

    player->position.y += player->velocity.y;
    getNeighboringTiles(&player->position, levelDim[0], 0, neighborTiles);

    // printf("%f, %f\n", position.y, velocity.y);

    if(floors[neighborTiles[1]] || floors[neighborTiles[2]]) {
        player->position.y = (neighborTiles[1] / levelDim[0] + 1) * 40;
        player->velocity.y = 0;
    }

    // printf(">%d, %d\n", neighborTiles[5], neighborTiles[6]);
    if(floors[neighborTiles[5]] || floors[neighborTiles[6]]) {
        player->position.y = (neighborTiles[5] / levelDim[0] - 1) * 40;
        player->velocity.y = 0;
        player->wasOnGround = 1;
    } else {
        player->wasOnGround = 0;
    }

    if(!player->wasOnGround) {
        player->velocity.y += 1;
    }
}