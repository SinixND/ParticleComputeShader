#ifndef IG20250606152423
#define IG20250606152423

#include <raylib.h>

struct Particle
{
    Vector2 position{ 0, 0 };
    Vector2 velocity{ 0, 0 };
    Color color{
        255,
        255,
        255,
        50
    };
};

#endif
