#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include "SimulationConfig.h"
#include <raylib.h>
#include <rlgl.h>

void Simulation::setupShaders()
{
    //* Init shaders
    // Compute shader
    // char* shaderCode = LoadFileText( config.computeShaderPath );
    // unsigned int shaderData = rlCompileShader( shaderCode, RL_COMPUTE_SHADER );
    // unsigned int computeShader = rlLoadComputeShaderProgram( shaderData );
    // UnloadFileText( shaderCode );

    //* Shader program
    shaderProgram = LoadShader(
        config.vertexShaderPath,
        config.fragmentShaderPath
    );

    //* VAO
    vao = rlLoadVertexArray();
    rlEnableVertexArray( vao );

    //* VBO
    vbo = rlLoadVertexBuffer(
        particles,
        sizeof( particles ),
        true
    );

    //* Connect VBO with vertex shader
    // (location)index: position in shader
    // compSize: n consecutive values of:
    // type: type of comp
    // normalized: bool - should data be normalized
    // stride: size of one vertex;
    // offset: byte offset into VBO

    //* Position
    rlSetVertexAttribute(
        0,
        2,
        RL_FLOAT,
        false,
        8 * sizeof( float ),
        0
    );
    //* Color
    rlSetVertexAttribute(
        1,
        4,
        RL_FLOAT,
        false,
        8 * sizeof( float ),
        0
    );

    rlEnableVertexAttribute( 0 );
    rlEnableVertexAttribute( 1 );

    rlDisableVertexArray();
}

void Simulation::init()
{
    //* Set workgroups and local size
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &csWorkgroups.x);
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &csWorkgroups.y);
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &csWorkgroups.z);
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &csLocalSize.x);
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &csLocalSize.y);
    // glGetInteger_iv(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &csLocalSize.z);

    [[maybe_unused]]
    int renderWidth{ GetRenderWidth() };
    [[maybe_unused]]
    int renderHeight{ GetRenderHeight() };

    // //* Init particles
    for ( size_t i{ 0 }; i < PARTICLE_COUNT; ++i )
    {
        particles[i] =
            // Particle{
            //     { 0, 0 },
            //     { 1, 1, 1, 1 },
            //     { 0, 0 }
            // };
            Particle{
                Vector2{ ( static_cast<float>( snx::RNG::random( 0, 2 * renderWidth ) ) - renderWidth ) / renderWidth, ( static_cast<float>( snx::RNG::random( 0, 2 * renderHeight ) ) - renderHeight ) / renderHeight },
                Vector4{
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    snx::RNG::random( 0, 255 ) / 255.0f,
                    .2f
                },
                Vector2{ static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f, static_cast<float>( snx::RNG::random( -100, 100 ) ) / 100.0f },
            };
    }

    setupShaders();
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

void Simulation::updateMultithreaded(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
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

    threadPool_.joinJobs();
}

void Simulation::updateGPU(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    SetShaderValue(
        shaderProgram,
        // 3,
        GetShaderLocationAttrib( shaderProgram, "mousePosition" ),
        &mousePosition,
        SHADER_UNIFORM_VEC2
    );

    SetShaderValue(
        shaderProgram,
        // 6,
        GetShaderLocationAttrib( shaderProgram, "dt" ),
        &dt,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shaderProgram,
        // 7,
        GetShaderLocationAttrib( shaderProgram, "screenWidth" ),
        &screenWidth,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shaderProgram,
        // 8,
        GetShaderLocationAttrib( shaderProgram, "screenHeight" ),
        &screenHeight,
        SHADER_UNIFORM_FLOAT
    );
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

void Simulation::deinit()
{
#if !defined( EMSCRIPTEN )
    threadPool_.stop();
#endif
}
