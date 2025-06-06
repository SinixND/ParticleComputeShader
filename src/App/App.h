#ifndef IG20240519210141
#define IG20240519210141

#include <raylib.h>

struct AppConfig;

class App
{
public:
    float dt{ 0 };

private:
    void setupAppEvents();

public:
    void init( AppConfig const& config );
    /// Main app loop
    void run();
    void deinit();
};

#endif
