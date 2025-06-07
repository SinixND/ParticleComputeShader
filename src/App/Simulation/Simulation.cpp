#include "Simulation.h"

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

void updateParticle(
    Particle& particle,
    Vector2 const& mousePosition,
    float dt
)
{
    ParticleSystem::attract(
        particle,
        mousePosition,
        1000
    );

    ParticleSystem::applyFriction(
        particle.velocity,
        0.985
    );

    ParticleSystem::move(
        particle,
        dt
    );
}

void Simulation::update( float dt )
{
    Vector2 const mousePosition{ GetMousePosition() };

    for ( Particle& particle : particles )
    {
        updateParticle(
            particle,
            mousePosition,
            dt
        );
    }
}

#if !defined( EMSCRIPTEN )
void Simulation::update_multithreaded( float dt )
{
    Vector2 const mousePosition{ GetMousePosition() };

    size_t const threadCount{ threadPool.threadCount() };

    for ( size_t i{ 0 }; i < threadCount; ++i )
    {
        size_t const threadNumber{ i };

        threadPool.queueJob(
            [&]()
            {
                for ( size_t particleNumber{ threadNumber * PARTICLE_COUNT / threadCount }; particleNumber < ( ( threadNumber + 1 ) * PARTICLE_COUNT / threadCount ); ++particleNumber )
                {
                    updateParticle( particles[particleNumber], mousePosition, dt );
                }
            }
        );
    }

    threadPool.joinJobs();
}
#endif

void Simulation::deinit()
{
#if !defined( EMSCRIPTEN )
    threadPool.stop();
#endif
}
