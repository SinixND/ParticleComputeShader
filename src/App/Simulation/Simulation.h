#ifndef IG20250606152339
#define IG20250606152339

#include "Particle.h"
#include "Particles.h"
#include "SimulationConfig.h"
#include "ThreadPool.h"
#include "data/SimulationData.h"
#include <raylib.h>

enum class State
{
    SINGLE_CORE,
    GPUVS,
    GPUCS,
    MULTITHREAD,
};

struct int3
{
    int x, y, z;
};

class Simulation
{
    SimulationConfig config{};
    // int3 csWorkgroups{};
    // int3 csLocalSize{};

#if !defined( EMSCRIPTEN )
    ThreadPool threadPool_{};
#endif

public:
    Particle particlesAoS[SimulationData::PARTICLE_COUNT];
    Particles particlesSoA;
    //* Can be empty, data comes from ssbo
    // Vector3 vertices[SimulationData::PARTICLE_COUNT];
    Vector3 vertices[1];

    //* Shader stuff
    unsigned int positionSSBO;
    unsigned int velocitySSBO;
    unsigned int colorSSBO;

    Shader shaderProgram{};
    unsigned int vao{};
    unsigned int vbo{};

    unsigned int computeShader{};

    float debugBuffer[32];

    //* Set to desired computation method
#if defined( EMSCRIPTEN )
    State state{ State::SINGLE_CORE };
#else
    State state{ State::GPUCS };
#endif

public:
    void init();
    void update(
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );
    void deinit();

private:
    void updateSingleCore(
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );

    void updateMultithreaded(
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );

    void updateVS(
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );

    void updateCS(
        int screenWidth,
        int screenHeight,
        Vector2 mousePosition,
        float dt
    );

    void setupVS();
    void setupCS();
};

#endif
