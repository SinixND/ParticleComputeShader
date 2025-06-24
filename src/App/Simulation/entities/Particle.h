#ifndef IG20250606152423
#define IG20250606152423

#include <raylib.h>

struct Particle
{
    Vector2 position{ 0.0f, 0.0f };
    Vector4 color{ 1.0f, 1.0f, 1.0f, 0.1f };
    Vector2 velocity{ 0.0f, 0.0f };
};

#endif
