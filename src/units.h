#ifndef UNITS_H
#define UNITS_H

#include <stdlib.h>

#define WINDOW_WIDTH 1536
#define WINDOW_HEIGHT 960
#define TARGET_FPS 60
#define WINDOW_TITLE "Tetris"

#define BOARD_X 5 
#define BOARD_Y 8
#define BOARD_Z 5
#define SIZE 1.0f

typedef enum { 
    MENU, 
    PLAYING 
} GameState;

typedef enum { 
    HERO = 1, 
    TEEWEE = 2, 
    SMASHBOY = 3,     
    ORANGE_RICKY = 4,
    BLUE_RICKY = 5,
    CLEVELAND_Z = 6,
    RHODE_ISLAND_Z = 7
} Blocks;

typedef enum {
    X, 
    Y,
    Z
} RotationAxis;

#endif // UNITS_H
