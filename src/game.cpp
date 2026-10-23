#include "game.h"
#include "gamemath.h"
#include <math.h>

static void getCameraGridBasis(Camera3D cam, int32_t *fx, int32_t *fz, int32_t *rx, int32_t *rz) {
    float dx = cam.target.x - cam.position.x;
    float dz = cam.target.z - cam.position.z;

    if (fabsf(dx) > fabsf(dz)) {
        *fx = (dx > 0.0f) ? 1 : -1;
        *fz = 0;
    } else {
        *fx = 0;
        *fz = (dz > 0.0f) ? 1 : -1;
    }

    *rx = -(*fz);
    *rz = *fx;
}

Rectangle getStartButtonRect(void) {
    return Rectangle{ (float)WINDOW_WIDTH / 2.0f - 150.0f, 440.0f, 300.0f, 65.0f };
}

Rectangle getExitButtonRect(void) {
    return Rectangle{ (float)WINDOW_WIDTH / 2.0f - 150.0f, 530.0f, 300.0f, 65.0f };
}

void updateMenu(Game *game) {
    game->camera.position = rotate_camera(game->camera.position, game->camera.target, GetRadian(0.25f));

    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP)) {
        game->menu_selection = 0;
    }
    if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        game->menu_selection = 1;
    }

    Vector2 mouse_pos = GetMousePosition();
    Rectangle start_rect = getStartButtonRect();
    Rectangle exit_rect = getExitButtonRect();

    bool hover_start = CheckCollisionPointRec(mouse_pos, start_rect);
    bool hover_exit = CheckCollisionPointRec(mouse_pos, exit_rect);

    if (hover_start) game->menu_selection = 0;
    if (hover_exit) game->menu_selection = 1;

    bool clicked = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && (hover_start || hover_exit);
    bool confirmed = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || clicked;

    if (confirmed) {
        if (game->menu_selection == 0) {
            resetBoard();
            SCORE = 0;
            game->active_piece = spawnPiece();
            game->fall_timer = 0.0f;
            game->state = PLAYING;
        } else {
            game->should_quit = true;
        }
    }
}

void updatePlaying(Game *game) {
    if (IsKeyPressed(KEY_M) || IsKeyPressed(KEY_P)) {
        game->state = MENU;
        return;
    }

    if (IsKeyDown(KEY_SEMICOLON)) {
        game->camera.position = rotate_camera(game->camera.position, game->camera.target, game->radian);
    }
    if (IsKeyDown(KEY_J)) {
        game->camera.position = rotate_camera(game->camera.position, game->camera.target, -game->radian);
    }

    int32_t fx, fz, rx, rz;
    getCameraGridBasis(game->camera, &fx, &fz, &rx, &rz);

    if (IsKeyPressed(KEY_W) && canMove(game->active_piece, fx, 0, fz)) {
        game->active_piece.gx += fx;
        game->active_piece.gz += fz;
    }
    if (IsKeyPressed(KEY_S) && canMove(game->active_piece, -fx, 0, -fz)) {
        game->active_piece.gx -= fx;
        game->active_piece.gz -= fz;
    }
    if (IsKeyPressed(KEY_A) && canMove(game->active_piece, -rx, 0, -rz)) {
        game->active_piece.gx -= rx;
        game->active_piece.gz -= rz;
    }
    if (IsKeyPressed(KEY_D) && canMove(game->active_piece, rx, 0, rz)) {
        game->active_piece.gx += rx;
        game->active_piece.gz += rz;
    }

    if (IsKeyPressed(KEY_Q)) {
        rotatePiece(&game->active_piece, X);
    }
    if (IsKeyPressed(KEY_E)) {
        rotatePiece(&game->active_piece, Y);
    }
    if (IsKeyPressed(KEY_R)) {
        rotatePiece(&game->active_piece, Z);
    }

    // Hard drop (Spacebar)
    if (IsKeyPressed(KEY_SPACE)) {
        while (canMove(game->active_piece, 0, -1, 0)) {
            game->active_piece.gy -= 1;
        }
        game->fall_timer = game->fall_interval;
    }

    // Gravity & Floor Locking
    game->fall_timer += GetFrameTime();
    if (game->fall_timer >= game->fall_interval) {
        game->fall_timer -= game->fall_interval;

        if (canMove(game->active_piece, 0, -1, 0)) {
            game->active_piece.gy -= 1;
        } else {
            lockPiece(game->active_piece);
            clearFullLayers();
            game->active_piece = spawnPiece();

            // Game Over check -> Update High Score and return to Menu
            if (!canMove(game->active_piece, 0, 0, 0)) {
                if (SCORE > HIGH_SCORE) {
                    HIGH_SCORE = SCORE;
                }
                game->state = MENU;
            }
        }
    }
}

void updateFrame(Game *game) {
    switch (game->state) {
        case MENU:    updateMenu(game);    break;
        case PLAYING: updatePlaying(game); break;
    }
}
