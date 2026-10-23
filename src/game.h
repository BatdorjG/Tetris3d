#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <stddef.h>
#include <raylib.h>
#include "units.h"
#include "board.h"

typedef struct {
    GameState state;
    size_t menu_selection; // 0 = Start, 1 = Exit
    bool should_quit;
    Camera3D camera;
    Model unit_cube_model;
    ActivePiece active_piece;
    float fall_interval;
    float fall_timer;
    float radian;
} Game;

Rectangle getStartButtonRect(void);
Rectangle getExitButtonRect(void);
void updateMenu(Game *game);
void updatePlaying(Game *game);
void updateFrame(Game *game);

#endif // GAME_H
