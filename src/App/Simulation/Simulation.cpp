#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include <raylib.h>

void Simulation::init()
{
    //* Init particles
    for ( size_t i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        particles[i] =
            Particle{
                { static_cast<float>( snx::RNG::random( 0, GetRenderWidth() ) ),
                  static_cast<float>( snx::RNG::random( 0, GetRenderHeight() ) ) },
                { static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f,
                  static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f },
                Vector4{
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    .2f
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
        MULTIPLIER
    );

    ParticleSystem::applyFriction(
        particle.velocity,
        FRICTION
    );

    ParticleSystem::move(
        particle,
        screenWidth,
        screenHeight,
        dt
    );
}

void Simulation::updateSingleCore(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    for ( int i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        updateParticle(
            particles[i],
            screenWidth,
            screenHeight,
            mousePosition,
            dt
        );
    }
}

void Simulation::update(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    switch ( state )
    {
        default:
        case State::SINGLE_CORE:
        {
            updateSingleCore(
                screenWidth,
                screenHeight,
                mousePosition,
                dt
            );

            break;
        }

        case State::MULTITHREAD:
        {
            updateMultithreaded(
                screenWidth,
                screenHeight,
                mousePosition,
                dt
            );

            break;
        }

        case State::GPU:
        {
            updateGPU(
                screenWidth,
                screenHeight,
                mousePosition,
                dt
            );

            break;
        }
    }
}

void Simulation::updateMultithreaded(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
#if !defined( EMSCRIPTEN )
    /**
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
    **/
    for ( size_t n{ 0 }; n < PARTICLE_COUNT; ++n )
    {
        threadPool_.queueJob(
            [=, this]()
            {
                updateParticle(
                    particles[n],
                    screenWidth,
                    screenHeight,
                    mousePosition,
                    dt
                );
            }
        );
    }
#endif
}

void Simulation::updateGPU(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    SetShaderValue(
        shader,
        // 3,
        GetShaderLocationAttrib( shader, "mousePosition" ),
        &mousePosition,
        SHADER_UNIFORM_VEC2
    );

    SetShaderValue(
        shader,
        // 6,
        GetShaderLocationAttrib( shader, "dt" ),
        &dt,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shader,
        // 7,
        GetShaderLocationAttrib( shader, "screenWidth" ),
        &screenWidth,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shader,
        // 8,
        GetShaderLocationAttrib( shader, "screenHeight" ),
        &screenHeight,
        SHADER_UNIFORM_FLOAT
    );
}

void Simulation::deinit()
{
#if !defined( EMSCRIPTEN )
    threadPool_.stop();
#endif
}
