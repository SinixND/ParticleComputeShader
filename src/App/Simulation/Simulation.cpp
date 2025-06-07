#include "Simulation.h"

#include "EventDispatcher.h"
#include "EventId.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include <raylib.h>

int constexpr PARTICLE_COUNT{ 100000 };

void Simulation::init()
{
    particles.reserve( PARTICLE_COUNT );
    particles.clear();

    for ( size_t i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        particles.push_back(
            Particle{
                { static_cast<float>( snx::RNG::random( 0, GetRenderWidth() ) ),
                  static_cast<float>( snx::RNG::random( 0, GetRenderHeight() ) ) },
                { static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f,
                  static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f }
            }
        );
    }
}

void Simulation::update( float dt )
{
    Vector2 mousePosition{ GetMousePosition() };

    for ( Particle& particle : particles )
    {
        ParticleSystem::attract(
            particle,
            mousePosition,
            1000
        );

        ParticleSystem::applyFriction(
            particle.velocity,
            0.99
        );

        ParticleSystem::move(
            particle,
            dt
        );
    }
}
