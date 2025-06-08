#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include "ThreadPool.h"
#include <array>

int constexpr PARTICLE_COUNT{ 50000 };

class Simulation
{
#if !defined( EMSCRIPTEN )
    ThreadPool threadPool_{};
#endif

public:
    std::array<Particle, PARTICLE_COUNT> particles{};

public:
    void init();
    void update( float dt );
    void update_multithreaded( float dt );
    void update_gpu( float dt );
    void deinit();
};

#endif
