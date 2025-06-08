#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include "ThreadPool.h"
#include <vector>

class Simulation
{
#if !defined( EMSCRIPTEN )
    ThreadPool threadPool_{};
#endif

public:
    std::vector<Particle> particles{};

public:
    void init();
    void update( float dt );
    void update_multithreaded( float dt );
    void update_gpu( float dt );
    void deinit();
};

#endif
