#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include <vector>

class Simulation
{
private:
    std::vector<Particle> particles{};

public:
    void init();
    void update( float dt );
};

#endif
