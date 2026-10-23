#include "gamemath.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Mat4 Mat4_init(void) {
    Mat4 mat;
    memset(&mat, 0, sizeof(Mat4));
    return mat;
}

Mat4 Mat4_identity(void) {
    Mat4 mat = Mat4_init();
    for (int i = 0; i < 4; i++) {
        mat.m[i][i] = 1.0f;
    }
    return mat;
}

Mat4 Mat4_rotateX(float angle) {
    Mat4 mat = Mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 Mat4_rotateY(float angle) {
    Mat4 mat = Mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 Mat4_rotateZ(float angle) {
    Mat4 mat = Mat4_identity();
    float c = cosf(angle);
    float s = sinf(angle);
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

Vector3 MatrixTransform(Vector3 pos, Mat4 mat) {
    return (Vector3){
        .x = mat.m[0][0] * pos.x + mat.m[0][1] * pos.y + mat.m[0][2] * pos.z + mat.m[0][3],
        .y = mat.m[1][0] * pos.x + mat.m[1][1] * pos.y + mat.m[1][2] * pos.z + mat.m[1][3],
        .z = mat.m[2][0] * pos.x + mat.m[2][1] * pos.y + mat.m[2][2] * pos.z + mat.m[2][3]
    };
}

float GetRadian(float degree) {
    return degree * ((float)M_PI / 180.0f);
}

float centered(int i, int n) {
    return (float)i - (float)(n - 1) / 2.0f;
}

Vector3 gridToWorld(int gx, int gy, int gz) {
    return (Vector3){
        .x = centered(gx, BOARD_X) * SIZE,
        .y = centered(gy, BOARD_Y) * SIZE,
        .z = centered(gz, BOARD_Z) * SIZE
    };
}

Vector3 rotate_camera(Vector3 start_pos, Vector3 target_pos, float angle) {
    float dx = start_pos.x - target_pos.x;
    float dz = start_pos.z - target_pos.z;
    Mat4 rot_mat = Mat4_rotateY(angle);

    Vector3 rotated = MatrixTransform((Vector3){ dx, start_pos.y, dz }, rot_mat);
    return (Vector3){
        .x = rotated.x + target_pos.x,
        .y = rotated.y,
        .z = rotated.z + target_pos.z
    };
}
