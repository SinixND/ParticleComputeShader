#ifndef IG20240529010632
#define IG20240529010632

#include <raylib.h>

struct AppConfig
{
    int windowWidth{ 800 };
    int windowHeight{ 940 };

    char const* const vertexShaderPath{ "assets/shaders/vertexShader.vert" };
    char const* const fragmentShaderPath{ "assets/shaders/fragmentShader.frag" };

    bool showFPS{ true };
    bool vSync{ false };
};

#endif
