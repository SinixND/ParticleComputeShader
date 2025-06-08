#include "App.h"

#include "AppConfigs.h"
#include "AppData.h"
#include "ColorData.h"
#include "DeveloperMode.h"
#include "EventDispatcher.h"
#include "EventId.h"
#include "ParticleSystem.h"
#include <raylib.h>

#if defined( EMSCRIPTEN )
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif

void setupRaylib( [[maybe_unused]] AppConfig const& config )
{
#if !defined( NOGUI )
    //* Raylib flags
    SetConfigFlags( FLAG_WINDOW_RESIZABLE );

    if ( config.vSync )
    {
        SetConfigFlags( FLAG_VSYNC_HINT );
    }

    //* Initialize window
    InitWindow(
        config.windowWidth,
        config.windowHeight,
        "Particle Simulation"
    );

    //* Raylib Settings
    SetWindowIcon( AppData::FAVICON );

#if defined( EMSCRIPTEN )
    MaximizeWindow();
#endif
#endif

    SetTargetFPS( AppData::FPS_TARGET );

    SetExitKey( AppData::EXIT_KEY );
}

void setupFrameworks( AppConfig const& config )
{
    setupRaylib( config );
}

void updateFullscreenState()
{
#if !defined( NOGUI )
    if ( IsKeyPressed( KEY_F11 ) )
    {
        if ( IsWindowMaximized() )
        {
            RestoreWindow();
        }

        else
        {
            MaximizeWindow();
        }
    }

    if ( IsWindowResized() )
    {
        snx::EventDispatcher::notify( EventId::WINDOW_RESIZED );
    }
#endif
}

void updateDeveloperMode()
{
    if ( IsKeyPressed( KEY_F1 ) )
    {
        DeveloperMode::toggle();
    }
}

void App::render()
{
#if !defined( NOGUI )
    BeginDrawing();
    ClearBackground( ColorData::BG );

    if ( DeveloperMode::isActive() )
    {
        DrawFPS( 0, 0 );
    }

    for ( Particle const& particle : simulation.particles )
    {
        ParticleSystem::drawParticle( particle );
    }

    EndDrawing();
#endif
}

/// @brief Void argument in function signature needed for emscripten
void updateApp( void* arg )
{
    App& app = *(App*)arg;

    updateFullscreenState();
    updateDeveloperMode();

    app.dt = GetFrameTime();

    app.simulation.update( app.dt );
    // app.simulation.update_multithreaded( app.dt );

    app.render();
}

void App::setupAppEvents()
{
    snx::EventDispatcher::addListener(
        EventId::WINDOW_RESIZED,
        [&]()
        {
            simulation.init();
        },
        true
    );
}

void App::init( AppConfig const& config )
{
    setupFrameworks( config );

    setupAppEvents();

    simulation.init();
}

void App::run()
{
#if defined( EMSCRIPTEN )
    emscripten_set_main_loop_arg(
        updateApp,
        this,
        AppData::FPS_TARGET /*FPS*/,
        1 /*Simulate infinite loop*/
    );
#elif defined( NOGUI )
    while ( !( IsKeyDown( AppData::EXIT_KEY ) ) )
    {
        updateApp( this );
    }
#else
    while ( !( WindowShouldClose() ) )
    {
        updateApp( this );
    }
#endif
}

void App::deinit()
{
    simulation.deinit();

#if !defined( NOGUI )
    //* Close window and opengl context
    CloseWindow();
#endif
}
