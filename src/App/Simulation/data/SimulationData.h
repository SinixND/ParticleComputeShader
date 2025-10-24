#ifndef IG20250625212711
#define IG20250625212711

namespace SimulationData
{
#if !defined( EMSCRIPTEN )
    int constexpr PARTICLE_COUNT{ 1024 * 1 };
#else
    int constexpr PARTICLE_COUNT{ 1024 };
#endif
    float constexpr MULTIPLIER{ 1000 };
    float constexpr FRICTION{ 0.995 };
}

#endif
