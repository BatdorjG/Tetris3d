#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <raylib.h>
#include "units.h"

typedef struct {
    Blocks block_type;
    int32_t cells[4][3];
    int32_t gx;
    int32_t gy;
    int32_t gz;
} ActivePiece;

extern uint8_t board[BOARD_Y][BOARD_Z][BOARD_X];
extern int32_t SCORE;
extern int32_t HIGH_SCORE;

void getBlockCells(Blocks block, int32_t out_cells[4][3]);
Color getBlockColor(uint8_t id);
void resetBoard(void);
bool canMove(ActivePiece piece, int32_t dx, int32_t dy, int32_t dz);
void rotatePiece(ActivePiece *piece, RotationAxis axis);
void lockPiece(ActivePiece piece);
void clearFullLayers(void);
ActivePiece spawnPiece(void);

#endif // BOARD_H
