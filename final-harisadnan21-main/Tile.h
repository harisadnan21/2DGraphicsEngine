#ifndef Tile_DEFINED
#define Tile_DEFINED
#include <cassert>
#include <cmath>

static float tile_repeat(float t){
    float x= t - std::floor(t);
    assert(x >= 0 && x < 1);
    return x;
}

static float tile_mirror(float t){
#if 1
    float x = t * 0.5f;
    x = x - std::floor(x);
    if (x >= 0.5f){
        x = 1- x;
    }
    x = x * 1.999999f;
#else
    float x = t -1;
    x = x - floorf(x * 0.5f) * 2 -1;
    x = std::abs(x);
#endif
    assert(x>= 0 && x < 1);
    return x;
}

#endif
