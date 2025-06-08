#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include <raylib.h>

void Simulation::init()
{
    for ( size_t i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        particles[i] =
            Particle{
                { static_cast<float>( snx::RNG::random( 0, GetRenderWidth() ) ),
                  static_cast<float>( snx::RNG::random( 0, GetRenderHeight() ) ) },
                { static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f,
                  static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f },
                Color{
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    (unsigned char)snx::RNG::random( 0, 255 ),
                    50
                }
            };
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
        0.99
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

    Vector2 mousePosition{ 0, 0 };
    if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    {
        mousePosition = GetMousePosition();
    }

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

    Vector2 mousePosition{ 0, 0 };
    if ( IsMouseButtonDown( MOUSE_LEFT_BUTTON ) )
    {
        mousePosition = GetMousePosition();
    }

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
