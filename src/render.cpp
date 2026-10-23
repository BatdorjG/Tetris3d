#include "render.h"
#include "gamemath.h"
#include "board.h"
#include <rlgl.h>

#define CUBE_SCALE 0.94f // small gap between cubes so they read as separate

typedef struct {
    Vector3 n;
    Vector3 u;
    Vector3 v;
    float shade;
} Face;

static inline Vector3 v3(float x, float y, float z) {
    return Vector3{ x, y, z };
}

// u x v == n for every face, so triangles wind CCW from outside (backface culling stays on)
static const Face FACES[6] = {
    { { 0,  1,  0 }, { 0, 0, 1 }, { 1, 0, 0 },  0.15f }, // top
    { { 0, -1,  0 }, { 1, 0, 0 }, { 0, 0, 1 }, -0.55f }, // bottom
    { { 1,  0,  0 }, { 0, 1, 0 }, { 0, 0, 1 }, -0.15f },
    { {-1,  0,  0 }, { 0, 0, 1 }, { 0, 1, 0 }, -0.35f },
    { { 0,  0,  1 }, { 1, 0, 0 }, { 0, 1, 0 },  0.0f  },
    { { 0,  0, -1 }, { 0, 1, 0 }, { 1, 0, 0 }, -0.25f }
};

static Vector3 faceCorner(Vector3 p, Face f, float su, float sv, float h) {
    return Vector3{
        p.x + (f.n.x + su * f.u.x + sv * f.v.x) * h,
        p.y + (f.n.y + su * f.u.y + sv * f.v.y) * h,
        p.z + (f.n.z + su * f.u.z + sv * f.v.z) * h
    };
}

static void drawCubeShaded(Vector3 pos, Color color) {
    float h = SIZE * 0.5f * CUBE_SCALE;
    for (int i = 0; i < 6; i++) {
        Face f = FACES[i];
        Vector3 a = faceCorner(pos, f, -1.0f, -1.0f, h);
        Vector3 b = faceCorner(pos, f,  1.0f, -1.0f, h);
        Vector3 c = faceCorner(pos, f,  1.0f,  1.0f, h);
        Vector3 d = faceCorner(pos, f, -1.0f,  1.0f, h);
        Color col = ColorBrightness(color, f.shade);
        DrawTriangle3D(a, b, c, col);
        DrawTriangle3D(a, c, d, col);
    }
    float s = SIZE * CUBE_SCALE;
    DrawCubeWires(pos, s, s, s, ColorBrightness(color, -0.6f));
}

static void drawGhostCube(Vector3 pos, Color color) {
    float s = SIZE * CUBE_SCALE;
    DrawCube(pos, s, s, s, Fade(color, 0.15f));
    DrawCubeWires(pos, s, s, s, Fade(color, 0.7f));
}

static void drawFloor(void) {
    float hx = BOARD_X * SIZE / 2.0f;
    float hz = BOARD_Z * SIZE / 2.0f;
    float fy = -(BOARD_Y * SIZE / 2.0f);

    DrawCube(v3(0.0f, fy - 0.03f, 0.0f), hx * 2.0f, 0.06f, hz * 2.0f, Color{ 25, 30, 55, 255 });

    Color line = Fade(SKYBLUE, 0.35f);
    for (int i = 0; i <= BOARD_X; i++) {
        float x = -hx + (float)i * SIZE;
        DrawLine3D(v3(x, fy, -hz), v3(x, fy, hz), line);
    }
    for (int i = 0; i <= BOARD_Z; i++) {
        float z = -hz + (float)i * SIZE;
        DrawLine3D(v3(-hx, fy, z), v3(hx, fy, z), line);
    }
}

static void DrawBoard(void) {
    drawFloor();
    DrawCubeWires(
        v3(0.0f, 0.0f, 0.0f),
        (float)BOARD_X * SIZE,
        (float)BOARD_Y * SIZE,
        (float)BOARD_Z * SIZE,
        Fade(LIGHTGRAY, 0.45f)
    );
}

void drawScene3D(const Game *game) {
    BeginMode3D(game->camera);

    DrawBoard();

    // 1. Locked blocks (opaque first)
    for (int y = 0; y < BOARD_Y; y++) {
        for (int z = 0; z < BOARD_Z; z++) {
            for (int x = 0; x < BOARD_X; x++) {
                uint8_t id = board[y][z][x];
                if (id != 0) {
                    drawCubeShaded(gridToWorld(x, y, z), getBlockColor(id));
                }
            }
        }
    }

    if (game->state == PLAYING) {
        ActivePiece piece = game->active_piece;
        Color color = getBlockColor((uint8_t)piece.block_type);

        // 2. Active piece (opaque)
        for (int i = 0; i < 4; i++) {
            drawCubeShaded(
                gridToWorld(
                    piece.gx + piece.cells[i][0],
                    piece.gy + piece.cells[i][1],
                    piece.gz + piece.cells[i][2]
                ),
                color
            );
        }

        // 3. Ghost piece & floor footprints (translucent, depth write disabled)
        ActivePiece ghost = piece;
        while (canMove(ghost, 0, -1, 0)) {
            ghost.gy -= 1;
        }

        rlDrawRenderBatchActive();
        rlDisableDepthMask();

        float fy = -(BOARD_Y * SIZE / 2.0f);
        for (int i = 0; i < 4; i++) {
            Vector3 p = gridToWorld(
                ghost.gx + ghost.cells[i][0],
                ghost.gy + ghost.cells[i][1],
                ghost.gz + ghost.cells[i][2]
            );
            int32_t fp = piece.gx + piece.cells[i][0];
            int32_t fz = piece.gz + piece.cells[i][2];
            Vector3 fpos = gridToWorld(fp, 0, fz);
            DrawCube(v3(fpos.x, fy + 0.01f, fpos.z), SIZE * 0.9f, 0.02f, SIZE * 0.9f, Fade(color, 0.3f));
            drawGhostCube(p, color);
        }

        rlDrawRenderBatchActive();
        rlEnableDepthMask();
    }

    EndMode3D();
}

void drawMenu(const Game *game) {
    // Semi-transparent dark overlay over the 3D background
    DrawRectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, Fade(BLACK, 0.7f));

    int cx = WINDOW_WIDTH / 2;

    // Title
    const char *title = "3D TETRIS";
    int title_w = MeasureText(title, 70);
    DrawText(title, cx - title_w / 2, 180, 70, SKYBLUE);

    // Highest Score & Last Score
    const char *hs_text = TextFormat("HIGHEST SCORE: %d", HIGH_SCORE);
    int hs_w = MeasureText(hs_text, 36);
    DrawText(hs_text, cx - hs_w / 2, 290, 36, GOLD);

    if (SCORE > 0) {
        const char *last_text = TextFormat("LAST SCORE: %d", SCORE);
        int last_w = MeasureText(last_text, 24);
        DrawText(last_text, cx - last_w / 2, 345, 24, LIGHTGRAY);
    }

    // Start Button
    Rectangle start_rect = getStartButtonRect();
    bool start_selected = (game->menu_selection == 0);
    DrawRectangleRec(start_rect, start_selected ? SKYBLUE : DARKGRAY);
    DrawRectangleLinesEx(start_rect, 3.0f, WHITE);
    const char *start_label = "START";
    int start_w = MeasureText(start_label, 32);
    DrawText(
        start_label,
        cx - start_w / 2,
        (int)start_rect.y + 16,
        32,
        start_selected ? BLACK : WHITE
    );

    // Exit Button
    Rectangle exit_rect = getExitButtonRect();
    bool exit_selected = (game->menu_selection == 1);
    DrawRectangleRec(exit_rect, exit_selected ? RED : DARKGRAY);
    DrawRectangleLinesEx(exit_rect, 3.0f, WHITE);
    const char *exit_label = "EXIT";
    int exit_w = MeasureText(exit_label, 32);
    DrawText(
        exit_label,
        cx - exit_w / 2,
        (int)exit_rect.y + 16,
        32,
        WHITE
    );

    const char *hint = "Use W/S or Mouse to Select | ENTER or Click to Confirm";
    int hint_w = MeasureText(hint, 20);
    DrawText(hint, cx - hint_w / 2, 640, 20, GRAY);
}

void drawHUD(void) {
    DrawRectangleRounded(Rectangle{ 12.0f, 12.0f, 280.0f, 96.0f }, 0.2f, 8, Fade(BLACK, 0.55f));
    DrawText(TextFormat("SCORE  %d", SCORE), 28, 24, 30, WHITE);
    DrawText(TextFormat("BEST   %d", HIGH_SCORE), 28, 66, 24, GOLD);

    const char *hint = "Move WASD   Rotate Q/E/R   Drop SPACE   Camera J/;   Menu M";
    int w = MeasureText(hint, 20);
    int bx = (WINDOW_WIDTH - w) / 2;
    DrawRectangleRounded(
        Rectangle{ (float)(bx - 16), (float)(WINDOW_HEIGHT - 52), (float)(w + 32), 36.0f },
        0.4f,
        8,
        Fade(BLACK, 0.55f)
    );
    DrawText(hint, bx, WINDOW_HEIGHT - 44, 20, LIGHTGRAY);
}

void drawFrame(const Game *game) {
    BeginDrawing();

    // 1. Wipe both the color buffer and the 3D depth buffer
    ClearBackground(BLACK);

    // 2. Draw 2D background gradient WITHOUT writing to the depth buffer
    rlDisableDepthTest();
    rlDisableDepthMask();
    DrawRectangleGradientV(
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        Color{ 28, 34, 66, 255 },
        Color{ 6, 6, 14, 255 }
    );
    rlDrawRenderBatchActive();
    rlEnableDepthMask();
    rlEnableDepthTest();

    // 3. Draw 3D scene
    drawScene3D(game);

    // 4. Draw 2D UI overlay
    switch (game->state) {
        case MENU:    drawMenu(game); break;
        case PLAYING: drawHUD();      break;
    }

    EndDrawing();
}
