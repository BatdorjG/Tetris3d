#include "board.h"
#include "gamemath.h"
#include <string.h>
#include <math.h>

uint8_t board[BOARD_Y][BOARD_Z][BOARD_X] = {0};
int32_t SCORE = 0;
int32_t HIGH_SCORE = 0;

void getBlockCells(Blocks block, int32_t out_cells[4][3]) {
    static const int32_t hero[4][3]           = { { 0, 0, -1 }, { 0, 0, 0 }, { 0, 0, 1 }, { 0, 0, 2 } };
    static const int32_t teewee[4][3]         = { { -1, 0, 0 }, { 0, 0, 0 }, { 1, 0, 0 }, { 0, 0, 1 } };
    static const int32_t smashboy[4][3]       = { { 0, 0, 0 },  { 1, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 } };
    static const int32_t orange_ricky[4][3]   = { { -1, 0, 0 }, { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 } };
    static const int32_t blue_ricky[4][3]     = { { -1, 0, 1 }, { -1, 0, 0 }, { 0, 0, 0 }, { 1, 0, 0 } };
    static const int32_t cleveland_z[4][3]    = { { -1, 0, 0 }, { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 } };
    static const int32_t rhode_island_z[4][3] = { { 0, 0, 0 },  { 1, 0, 0 }, { -1, 0, 1 }, { 0, 0, 1 } };

    const int32_t (*src)[3] = hero;
    switch (block) {
        case HERO:           src = hero;           break;
        case TEEWEE:         src = teewee;         break;
        case SMASHBOY:       src = smashboy;       break;
        case ORANGE_RICKY:   src = orange_ricky;   break;
        case BLUE_RICKY:     src = blue_ricky;     break;
        case CLEVELAND_Z:    src = cleveland_z;    break;
        case RHODE_ISLAND_Z: src = rhode_island_z; break;
    }
    memcpy(out_cells, src, sizeof(int32_t) * 4 * 3);
}


Color getBlockColor(uint8_t id) {
    switch (id) {
        case 1:  return SKYBLUE;
        case 2:  return PURPLE;
        case 3:  return YELLOW;
        case 4:  return ORANGE;
        case 5:  return BLUE;
        case 6:  return RED;
        case 7:  return GREEN;
        default: return WHITE;
    }
}

void resetBoard(void) {
    memset(board, 0, sizeof(board));
}

bool canMove(ActivePiece piece, int32_t dx, int32_t dy, int32_t dz) {
    for (int i = 0; i < 4; i++) {
        int32_t nx = piece.gx + piece.cells[i][0] + dx;
        int32_t ny = piece.gy + piece.cells[i][1] + dy;
        int32_t nz = piece.gz + piece.cells[i][2] + dz;

        if (nx < 0 || nx >= BOARD_X) return false;
        if (nz < 0 || nz >= BOARD_Z) return false;
        if (ny < 0) return false;

        if (ny < BOARD_Y) {
            if (board[ny][nz][nx] != 0) {
                return false;
            }
        }
    }
    return true;
}

void rotatePiece(ActivePiece *piece, RotationAxis axis) {
    ActivePiece rotated = *piece;
    float angle = GetRadian(90.0f);

    Mat4 rot_mat;
    switch (axis) {
        case X: rot_mat = Mat4_rotateX(angle); break;
        case Y: rot_mat = Mat4_rotateY(angle); break;
        case Z: rot_mat = Mat4_rotateZ(angle); break;
    }

    for (int i = 0; i < 4; i++) {
        Vector3 pos = {
            .x = (float)rotated.cells[i][0],
            .y = (float)rotated.cells[i][1],
            .z = (float)rotated.cells[i][2]
        };

        Vector3 transformed = MatrixTransform(pos, rot_mat);

        rotated.cells[i][0] = (int32_t)roundf(transformed.x);
        rotated.cells[i][1] = (int32_t)roundf(transformed.y);
        rotated.cells[i][2] = (int32_t)roundf(transformed.z);
    }

    static const int32_t kicks[10][3] = {
        { 0, 0, 0 },
        { -1, 0, 0 }, { 1, 0, 0 },
        { 0, 0, -1 }, { 0, 0, 1 },
        { 0, 1, 0 },
        { -2, 0, 0 }, { 2, 0, 0 },
        { 0, 0, -2 }, { 0, 0, 2 }
    };

    for (int i = 0; i < 10; i++) {
        int32_t kx = kicks[i][0];
        int32_t ky = kicks[i][1];
        int32_t kz = kicks[i][2];

        if (canMove(rotated, kx, ky, kz)) {
            rotated.gx += kx;
            rotated.gy += ky;
            rotated.gz += kz;
            *piece = rotated;
            return;
        }
    }
}

void lockPiece(ActivePiece piece) {
    uint8_t color_id = (uint8_t)piece.block_type;
    for (int i = 0; i < 4; i++) {
        int32_t nx = piece.gx + piece.cells[i][0];
        int32_t ny = piece.gy + piece.cells[i][1];
        int32_t nz = piece.gz + piece.cells[i][2];

        if (nx >= 0 && nx < BOARD_X && ny >= 0 && ny < BOARD_Y && nz >= 0 && nz < BOARD_Z) {
            board[ny][nz][nx] = color_id;
        }
    }
}

void clearFullLayers(void) {
    size_t y = 0;
    while (y < BOARD_Y) {
        bool is_full = true;
        for (size_t z = 0; z < BOARD_Z; z++) {
            for (size_t x = 0; x < BOARD_X; x++) {
                if (board[y][z][x] == 0) {
                    is_full = false;
                    break;
                }
            }
            if (!is_full) break;
        }

        if (is_full) {
            SCORE += 100;
            if (SCORE > HIGH_SCORE) {
                HIGH_SCORE = SCORE;
            }
            for (size_t shift_y = y; shift_y < BOARD_Y - 1; shift_y++) {
                memcpy(board[shift_y], board[shift_y + 1], sizeof(board[0]));
            }
            memset(board[BOARD_Y - 1], 0, sizeof(board[0]));
        } else {
            y += 1;
        }
    }
}

ActivePiece spawnPiece(void) {
    int rand_idx = (rand() % 7) + 1;
    Blocks b_type = (Blocks)rand_idx;

    ActivePiece piece = {
        .block_type = b_type,
        .gx = BOARD_X / 2,
        .gy = BOARD_Y - 1,
        .gz = BOARD_Z / 2
    };
    getBlockCells(b_type, piece.cells);
    return piece;
}
