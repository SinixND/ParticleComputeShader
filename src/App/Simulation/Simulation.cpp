#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include <raylib.h>

int constexpr PARTICLE_COUNT{ 10000 };

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
    int screenWidth,
    int screenHeight,
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
        screenWidth,
        screenHeight,
        dt
    );
}

void Simulation::update( float dt )
{
    int screenWidth{ GetScreenWidth() };
    int screenHeight{ GetScreenHeight() };

    Vector2 const mousePosition{ GetMousePosition() };

    for ( Particle& particle : particles )
    {
        updateParticle(
            particle,
            screenWidth,
            screenHeight,
            mousePosition,
            dt
        );
    }
}

#if !defined( EMSCRIPTEN )
void Simulation::update_multithreaded( float dt )
{
    int screenWidth{ GetScreenWidth() };
    int screenHeight{ GetScreenHeight() };
    Vector2 const mousePosition{ GetMousePosition() };

    size_t const threadCount{ threadPool_.threadCount() };
    float const particlesPerThread{ 1.0f * PARTICLE_COUNT / threadCount };

    for ( size_t threadNumber{ 0 }; threadNumber < threadCount; ++threadNumber )
    {
        size_t firstParticle{ (size_t)( threadNumber * particlesPerThread ) };
        size_t lastParticle{ (size_t)( ( threadNumber + 1 ) * particlesPerThread ) };

        threadPool_.queueJob(
            [=, this]()
            {
                for ( size_t particleNumber{ firstParticle }; particleNumber < lastParticle; ++particleNumber )
                {
                    updateParticle(
                        particles[particleNumber],
                        screenWidth,
                        screenHeight,
                        mousePosition,
                        dt
                    );
                }
            }
        );
    }

    threadPool_.joinJobs();
}
#endif

void Simulation::deinit()
{
#if !defined( EMSCRIPTEN )
    threadPool_.stop();
#endif
}
