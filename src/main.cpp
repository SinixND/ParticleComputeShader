#include "App.h"
#include "AppConfigs.h"
int main( /* int argc, char** argv */ )
{
    AppConfig config{};

    App app{};

    app.init( config );
    app.run();
    app.deinit();

    return 0;
}

