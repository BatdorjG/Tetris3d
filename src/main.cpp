#include <time.h>
#include <raylib.h>
#include "units.h"
#include "gamemath.h"
#include "board.h"
#include "game.h"
#include "render.h"

int main(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    srand((unsigned int)time(NULL));

    Mesh unitCubeMesh = GenMeshCube(SIZE, SIZE, SIZE);
    Model unitCubeModel = LoadModelFromMesh(unitCubeMesh);

    Game game = {
        .state = MENU,
        .menu_selection = 0,
        .should_quit = false,
        .camera = {
            .position = { 7.0f, 10.0f, 7.0f },
            .target = { 0.0f, 0.0f, 0.0f },
            .up = { 0.0f, 1.0f, 0.0f },
            .fovy = 60.0f,
            .projection = CAMERA_PERSPECTIVE
        },
        .unit_cube_model = unitCubeModel,
        .active_piece = spawnPiece(),
        .fall_interval = 1.0f,
        .fall_timer = 0.0f,
        .radian = GetRadian(5.0f)
    };

    while (!WindowShouldClose() && !game.should_quit) {
        updateFrame(&game);
        drawFrame(&game);
    }

    UnloadModel(unitCubeModel);
    CloseWindow();

    return 0;
}
