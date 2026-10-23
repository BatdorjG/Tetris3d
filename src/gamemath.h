#ifndef GAMEMATH_H
#define GAMEMATH_H

#include <raylib.h>
#include "units.h"

typedef struct {
    float m[4][4];
} Mat4;

Mat4 Mat4_init(void);
Mat4 Mat4_identity(void);
Mat4 Mat4_rotateX(float angle);
Mat4 Mat4_rotateY(float angle);
Mat4 Mat4_rotateZ(float angle);

Vector3 MatrixTransform(Vector3 pos, Mat4 mat);
float GetRadian(float degree);
float centered(int i, int n);
Vector3 gridToWorld(int gx, int gy, int gz);
Vector3 rotate_camera(Vector3 start_pos, Vector3 target_pos, float angle);

#endif // GAMEMATH_H

