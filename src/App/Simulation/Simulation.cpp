#include "Simulation.h"

#include "Particle.h"
#include "ParticleSystem.h"
#include "RNG.h"
#include "SimulationConfig.h"
#include "SimulationData.h"
#include <cstddef>
#include <raylib.h>
#include <rlgl.h>

void Simulation::setupVS()
{
    //* Init shaders
    //* Shader program
    shaderProgram = LoadShader(
        config.particleVSPath,
        config.minimalFSPath
    );

    //* VAO
    vao = rlLoadVertexArray();
    rlEnableVertexArray( vao );

    //* VBO
    vbo = rlLoadVertexBuffer(
        vertices,
        sizeof( vertices ),
        false
    );

    //* Connect VBO with vertex shader
    // (location)index: position in shader
    // compSize: n consecutive values of:
    // type: type of comp
    // normalized: bool - should data be normalized
    // stride: size of one vertex;
    // offset: byte offset into VBO

    // NOTE: Unnecessary? Will be fetched from ssbo
    // rlSetVertexAttribute(
    //     0,
    //     2,
    //     RL_FLOAT,
    //     false,
    //     8 * sizeof( float ),
    //     ( offsetof( Particle, position ) )
    // );
    // rlEnableVertexAttribute( 0 );

    // rlSetVertexAttribute(
    //     1,
    //     4,
    //     RL_FLOAT,
    //     false,
    //     8 * sizeof( float ),
    //     ( offsetof( Particle, color ) )
    // );
    // rlEnableVertexAttribute( 1 );

    rlDisableVertexArray();
}

void Simulation::setupCS()
{
    //* Compute shader
    char* shaderCode = LoadFileText( config.particleCSPath );
    unsigned int shaderData = rlCompileShader( shaderCode, RL_COMPUTE_SHADER );
    computeShader = rlLoadComputeShaderProgram( shaderData );
    UnloadFileText( shaderCode );

    //* Shader program
    shaderProgram = LoadShader(
        config.minimalVSPath,
        config.minimalFSPath
    );

    //* VAO
    vao = rlLoadVertexArray();
    rlEnableVertexArray( vao );

    //* VBO
    vbo = rlLoadVertexBuffer(
        vertices,
        sizeof( vertices ),
        false
    );
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

    // //* Init particles
    for ( size_t i{ 0 }; i < SimulationData::PARTICLE_COUNT; ++i )
    {
        Vector2 position{
            ( 1.0f * snx::RNG::random( -SimulationData::PARTICLE_COUNT, SimulationData::PARTICLE_COUNT ) / SimulationData::PARTICLE_COUNT ),
            ( 1.0f * snx::RNG::random( -SimulationData::PARTICLE_COUNT, SimulationData::PARTICLE_COUNT ) / SimulationData::PARTICLE_COUNT )
        };

        Vector2 velocity{ snx::RNG::random( -100, 100 ) / 100.0f, snx::RNG::random( -100, 100 ) / 100.0f };

        Vector4 color{
            snx::RNG::random( 0, 255 ) / 255.0f,
            snx::RNG::random( 0, 255 ) / 255.0f,
            snx::RNG::random( 0, 255 ) / 255.0f,
            .2f
        };

        particlesAoS[i] =
            Particle{
                position,
                velocity,
                color
            };

        particlesSoA.positions[i] = {
            position.x,
            position.y,
            0,
            0
        };

        particlesSoA.velocities[i] = {
            velocity.x,
            velocity.y,
            0,
            0
        };

        particlesSoA.colors[i] = color;
    }

    positionSSBO = rlLoadShaderBuffer(
        SimulationData::PARTICLE_COUNT * sizeof( Vector4 ),
        particlesSoA.positions,
        RL_DYNAMIC_COPY
    );

    velocitySSBO = rlLoadShaderBuffer(
        SimulationData::PARTICLE_COUNT * sizeof( Vector4 ),
        particlesSoA.velocities,
        RL_DYNAMIC_COPY
    );

    colorSSBO = rlLoadShaderBuffer(
        SimulationData::PARTICLE_COUNT * sizeof( Vector4 ),
        particlesSoA.colors,
        RL_DYNAMIC_COPY
    );

    setupVS();
    setupCS();
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

#if !defined( EMSCRIPTEN )
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
#endif

        case State::GPUVS:
        {
            updateVS(
                screenWidth,
                screenHeight,
                mousePosition,
                dt
            );

            break;
        }

        case State::GPUCS:
        {
            updateCS(
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
        SimulationData::MULTIPLIER
    );

    ParticleSystem::applyFriction(
        particle.velocity,
        SimulationData::FRICTION
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
    for ( int i{ 0 }; i < SimulationData::PARTICLE_COUNT; ++i )
    {
        updateParticle(
            particlesAoS[i],
            screenWidth,
            screenHeight,
            mousePosition,
            dt
        );
    }
}

#if !defined( EMSCRIPTEN )
void Simulation::updateMultithreaded(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    /**
    size_t const threadCount{ threadPool_.threadCount() };
    float const particlesPerThread{ 1.0f * SimulationData::PARTICLE_COUNT / threadCount };

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

    for ( size_t n{ 0 }; n < SimulationData::PARTICLE_COUNT; ++n )
    {
        threadPool_.queueJob(
            [=, this]()
            {
                updateParticle(
                    particlesAoS[n],
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
#endif

void Simulation::updateVS(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    //* Calls rlSetUniform()
    SetShaderValue(
        shaderProgram,
        GetShaderLocationAttrib( shaderProgram, "mousePosition" ),
        &mousePosition,
        SHADER_UNIFORM_VEC2
    );

    SetShaderValue(
        shaderProgram,
        GetShaderLocationAttrib( shaderProgram, "dt" ),
        &dt,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shaderProgram,
        GetShaderLocationAttrib( shaderProgram, "screenWidth" ),
        &screenWidth,
        SHADER_UNIFORM_FLOAT
    );

    SetShaderValue(
        shaderProgram,
        GetShaderLocationAttrib( shaderProgram, "screenHeight" ),
        &screenHeight,
        SHADER_UNIFORM_FLOAT
    );

    rlBindShaderBuffer( positionSSBO, 0 );
    rlBindShaderBuffer( velocitySSBO, 1 );
    rlBindShaderBuffer( colorSSBO, 2 );

    // rlReadShaderBuffer( positionSSBO, debugBuffer, 32 * sizeof( float ), 0 );
    // rlReadShaderBuffer( velocitySSBO, debugBuffer, 32 * sizeof( float ), 0 );
    // rlReadShaderBuffer( colorSSBO, debugBuffer, 32 * sizeof( float ), 0 );
}

void Simulation::updateCS(
    int screenWidth,
    int screenHeight,
    Vector2 mousePosition,
    float dt
)
{
    rlEnableShader( computeShader );

    rlSetUniform(
        GetShaderLocationAttrib( shaderProgram, "mousePosition" ),
        &mousePosition,
        SHADER_UNIFORM_VEC2,
        1
    );

    rlSetUniform(
        GetShaderLocationAttrib( shaderProgram, "dt" ),
        &dt,
        SHADER_UNIFORM_FLOAT,
        1
    );

    rlSetUniform(
        GetShaderLocationAttrib( shaderProgram, "screenWidth" ),
        &screenWidth,
        SHADER_UNIFORM_FLOAT,
        1
    );

    rlSetUniform(
        GetShaderLocationAttrib( shaderProgram, "screenHeight" ),
        &screenHeight,
        SHADER_UNIFORM_FLOAT,
        1
    );

    rlBindShaderBuffer( positionSSBO, 0 );
    rlBindShaderBuffer( velocitySSBO, 1 );
    rlBindShaderBuffer( colorSSBO, 2 );

    rlComputeShaderDispatch( SimulationData::PARTICLE_COUNT / 1024, 1, 1 );

    rlDisableShader();
}

