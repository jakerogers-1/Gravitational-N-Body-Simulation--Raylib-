#ifndef BASE_H
#define BASE_H

#include "raylib.h"
#include "raymath.h"

#define GRAV_CONST 1.0
#define UNIVERSE_WIDTH 1024
#define UNIVERSE_HEIGHT 512
#define SOFTENING 0.1

typedef struct {
    Vector2 pos;
    Vector2 vel;
    double mass;          
} Body;

Vector2 get_disp_vec(Vector2 r1, Vector2 r2);
Vector2 periodic_add(Vector2 r1, Vector2 r2);

#endif

