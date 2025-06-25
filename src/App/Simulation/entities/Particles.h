#ifndef IG20250625212014
#define IG20250625212014

#include "SimulationData.h"
#include <raylib.h>

struct Particles
{
    //* TODO: Test Vector2
    Vector4 positions[SimulationData::PARTICLE_COUNT];
    Vector4 velocities[SimulationData::PARTICLE_COUNT];
    Vector4 colors[SimulationData::PARTICLE_COUNT];
};

#endif
