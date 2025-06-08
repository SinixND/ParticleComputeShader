#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include "ThreadPool.h"

int constexpr PARTICLE_COUNT{ 100000 };

class Simulation
{
#if !defined( EMSCRIPTEN )
    ThreadPool threadPool_{};
#endif

public:
    // std::array<Particle, PARTICLE_COUNT> particles{};
    std::vector<Particle> particles{ PARTICLE_COUNT };

public:
    void init();
    void update( float dt );
    void update_multithreaded( float dt );
    void update_gpu( float dt );
    void deinit();
};

#endif
