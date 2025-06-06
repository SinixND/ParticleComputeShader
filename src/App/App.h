#ifndef IG20240519210141
#define IG20240519210141

#include "Simulation.h"
#include <raylib.h>

struct AppConfig;

class App
{
public:
    Simulation simulation{};

    float dt{ 0 };

private:
    void setupAppEvents();

public:
    void init( AppConfig const& config );
    /// Main app loop
    void run();
    void render();
    void deinit();
};

#endif
