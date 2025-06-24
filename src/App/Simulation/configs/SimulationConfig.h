#ifndef IG20250623103155
#define IG20250623103155

struct SimulationConfig
{
    char const* const vertexShaderPath{ "assets/shaders/particles.vert" };
    char const* const fragmentShaderPath{ "assets/shaders/particles.frag" };
    char const* const computeShaderPath{ "assets/shaders/particles.glsl" };
};

#endif

