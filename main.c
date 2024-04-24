#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>

#include <sys/timeb.h>

#include "entities.h"
#include "network_client.h"
#include "packets.h"

#define GLSL_VERSION 330

// void loadPlayer(Texture2D *walk, Texture2D *run) {
//     for(int i = 0; i < 12; i++) {
//         char buff[33];
//         snprintf(buff, 33, "assets/Zombie1/Zombie1_%05d.png", i+9);
//         walk[i] = LoadTexture(buff);
//     }

//     for(int i = 0; i < 7; i++) {
//         char buff[33];
//         snprintf(buff, 33, "assets/Zombie1/Zombie1_%05d.png", i+22);
//         run[i] = LoadTexture(buff);
//     }
// }

// void unloadPlayer(Texture2D *walk, Texture2D * run) {
//     for(int i = 0; i < 12; i++) {
//         UnloadTexture(walk[i]);
//     }
//     for(int i = 0; i < 7; i++) {
//         UnloadTexture(run[i]);
//     }
// }

Texture2D loadPlayer() {
    return LoadTexture("assets/player/placeholder.png");
}

void unloadPlayer(Texture2D *player) {
    UnloadTexture(*player);
}

void loadLevel(char *name, Color **colorsPtr, Texture2D *tilePtr, int *levelDim, int** floorsPtr, int **wallsPtr) {
    Image bg = LoadImage(name);
    Color *colors = LoadImageColors(bg);

    Texture2D tile = LoadTexture("assets/bg/slime.png");
    *colorsPtr = colors;
    *tilePtr = tile;

    levelDim[0] = bg.width;
    levelDim[1] = bg.height;

    int *floors = (int *)calloc(bg.width * bg.height, sizeof(int));
    int *walls = (int *)calloc(bg.width * bg.height, sizeof(int));

    for(int i = 0; i < bg.width * bg.height; i++) {
        Color c = *(colors+i);
        int x = i % bg.width;
        int y = i / bg.width;
        if(c.r == 0 && c.g == 0 && c.b == 0) {
            if(isFloor(colors, bg.width, bg.height, x, y)) {
                floors[i] = 1;
            }
            if(isWall(colors, bg.width, bg.height, x, y)) {
                walls[i] = 1;
            }
        }
    }

    *floorsPtr = floors;
    *wallsPtr = walls;
    
    UnloadImage(bg);
}

int isWall(Color *colors, int width, int height, int x, int y) {
    int i = y * width + x;
    int wallRight = 0, wallLeft = 0;
    if(x > 0 && colors[i-1].r == 255 && colors[i-1].g == 255 && colors[i-1].b == 255) {
        wallRight = 1;
    }
    if(x < width-1 && colors[i+1].r == 255 && colors[i+1].g == 255 && colors[i+1].b == 255) {
        wallLeft = 1;
    }

    return wallLeft || wallRight;
}

int isFloor(Color *colors, int width, int height, int x, int y) {
    int i = y * width + x;
    int floor = 0, ceiling = 0;
    if(y > 0 && colors[i-width].r == 255 && colors[i-width].g == 255 && colors[i-width].b == 255) {
        floor = 1;
    }
    if(y < height-1 && colors[i+width].r == 255 && colors[i+width].g == 255 && colors[i+width].b == 255) {
        ceiling = 1;
    }

    return floor || ceiling;
}

void drawLevel(int width, int height, Color *colors, Texture2D *tile, int *floors, int *walls) {
    for(int i = 0; i < width * height; i++) {
        Color c = *(colors+i);
        int x = i % width;
        int y = i / width;
        if(c.r == 0 && c.g == 0 && c.b == 0) {
            if(floors[i]) {
                DrawTextureEx(*tile, (Vector2){ x * 40, y * 40 }, 0.0f, 4.0f, BLUE);
            } else if(walls[i]) {
                DrawTextureEx(*tile, (Vector2){ x * 40, y * 40 }, 0.0f, 4.0f, RED);
            } else
                DrawTextureEx(*tile, (Vector2){ x * 40, y * 40 }, 0.0f, 4.0f, WHITE);
        }
    }
}

void unloadLevel(Color *colors, Texture2D *tile, int *floors, int *walls) {
    UnloadImageColors(colors);
    UnloadTexture(*tile);
    free(floors);
    free(walls);
}

// int collideWithSprite(Vector2 *position, int width, int height, int x, int y) {
//     int colX = !(position->x >= (x + 1) * 40) && !(position->x + 40 <= x * 40);
//     int colY = !(position->y >= (y + 1) * 40) && !(position->y + 40 <= y * 40);

//     return colX && colY;
// }

void getNeighboringTiles(Vector2 *position, int width, int height, int *out) {
    out[0] = (int)(position->y / 40) * width + (int)((position->x - 1) / 40);
    out[1] = (int)((position->y - 1) / 40) * width + (int)(position->x / 40);

    out[2] = (int)((position->y - 1) / 40) * width + (int)((position->x + 39) / 40);
    out[3] = (int)(position->y / 40) * width + (int)((position->x + 40) / 40);

    out[4] = (int)((position->y + 39) / 40) * width + (int)((position->x + 40) / 40);
    out[5] = (int)((position->y + 40) / 40) * width + (int)((position->x + 39) / 40);

    out[6] = (int)((position->y + 40) / 40) * width + (int)(position->x / 40);
    out[7] = (int)((position->y + 39) / 40) * width + (int)((position->x - 1) / 40);
}

void close(Player *p, Grenade *grenades, char *takenIndices, int grenadeLen, Texture2D *playerTex, Texture2D *grenadeTex, Color *colors, Texture2D *tile, int *floors, int *walls) {
    deletePlayer(p);
    for(char i = 0; i < grenadeLen; i++) {
        if(takenIndices[i]) {
            deleteGrenade(&grenades[i]);
        }
    }
    UnloadTexture(*playerTex);
    UnloadTexture(*grenadeTex);
    unloadLevel(colors, tile, floors, walls);
    CloseWindow();
}

int main(void) {
    printf("Hello, world!\n");

    InitWindow(800, 600, "grenat!");
    
    Image icon = LoadImage("assets/icons/icon.png");
    Texture2D playerTex = LoadTexture("assets/player/placeholder.png");
    Texture2D grenadeTex = LoadTexture("assets/player/placeholder.png");

    SetWindowIcon(icon);
    
    // int display = GetCurrentMonitor();
    // SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
    // ToggleFullscreen();

    Color *colors = NULL;
    Texture2D tile;
    int levelDim[2];
    int *floors;
    int *walls;
    
    loadLevel("assets/bg/bg.png", &colors, &tile, levelDim, &floors, &walls);

    // Texture2D walk[12];
    // Texture2D run[7];

    Player p1 = createPlayer(0, &playerTex);

    SetTargetFPS(60);

    p1.position = (Vector2) { (800 - p1.texture->width * 4) / 2, /*(600 - player.height * 4) / 2*/600 - 20*4 };

    int currentFrame = 0;

    int framesSpeed = 8;
    int framesCounter = 0;

    int isRunning = 0;

    Grenade grenades[100];
    char takenIndices[100] = {0};

    int res;
    res = init_connection();
    if(res != 0) {
        close(&p1, grenades, takenIndices, 100, &playerTex, &grenadeTex, colors, &tile, floors, walls);
        printf("An error occured\n");
        return 1;
    }

    res = connect_client("127.0.0.1", 7676);
    if(res != 0) {
        close(&p1, grenades, takenIndices, 100, &playerTex, &grenadeTex, colors, &tile, floors, walls);
        return 1;
    }

    // struct timeb start, end;
    // ftime(&start);
    // int diff;

    while(!WindowShouldClose()) {
        // ftime(&end);
        // diff = (int) (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
        // ftime(&start);
        // printf("Took %u ms\n", diff);

        if(is_connected()) {
            framesCounter++;

            if(framesCounter >= 60/framesSpeed) {
                framesCounter = 0;
                currentFrame++;

                if(!isRunning && currentFrame > 11 || isRunning && currentFrame > 6) {
                    currentFrame = 0;
                }
            }

            inputPlayer(&p1);
            moveAndCheckPlayer(&p1, levelDim, floors, walls);

            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                {
                    Grenade g = createGrenade(0, &p1, &grenadeTex);
                    g.position = p1.position;
                    g.velocity.x = p1.velocity.x * 2.0f;
                    g.velocity.y = p1.velocity.y * 2.0f;
                    if(storeGrenade(grenades, 100, takenIndices, &g)) {
                        printf("Too many entities showed on screen!\n");
                        return 1;
                    }
                }
            }

            for(char i = 0; i < 100; i++) {
                if(takenIndices[i]) {
                    updateGrenade(&grenades[i], levelDim, floors, walls);
                    if(grenades[i].dead) {
                        takenIndices[i] = 0;
                    }
                }
            }
        }

        update_client();

        // printf("//%f, %f\n", position.y, velocity.y);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        drawLevel(levelDim[0], levelDim[1], colors, &tile, floors, walls);
        // DrawTextureEx(isRunning ? run[currentFrame] : walk[currentFrame], position, 0.0f, 10.0f, WHITE);
        DrawTextureEx(*p1.texture, p1.position, 0.0f, 4.0f, WHITE);
        for(char i = 0; i < 100; i++) {
            if(takenIndices[i]) {
                DrawTextureEx(*grenades[i].texture, grenades[i].position, 0.0f, 4.0f, WHITE);
            }
        }
        EndDrawing();

    }

    close(&p1, grenades, takenIndices, 100, &playerTex, &grenadeTex, colors, &tile, floors, walls);

    disconnect_client();
    deinit_connection();

    return 0;
}