#ifndef IG20240519210141
#define IG20240519210141

#include "AppConfigs.h"
#include "Simulation.h"
#include <raylib.h>

struct AppConfig;

class App
{
    AppConfig config{};

public:
    Simulation simulation{};

    int screenWidth{};
    int screenHeight{};
    Vector2 mousePosition{};
    float dt{ 0 };

public:
    void init();
    /// Main app loop
    void run();
    void render();
    void deinit();

private:
    void setupAppEvents();
};

#endif
