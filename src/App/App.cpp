#include "App.h"

#include "AppConfigs.h"
#include "AppData.h"
#include "ColorData.h"
#include "DeveloperMode.h"
#include "EventDispatcher.h"
#include "EventId.h"
#include <iostream>
#include <ncurses.h>
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

void setupNcurses()
{
#if defined( NOGUI )
    initscr();
    keypad( stdscr, TRUE );
#endif
}

void setupFrameworks( AppConfig const& config )
{
    setupRaylib( config );
    setupNcurses();
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

void update_temp( [[maybe_unused]] float dt )
{
    static int ch{};
    clear();
    printw( "%c\n", ch );
    printw( "%f\n", dt );
    refresh();
    ch = (int)getch();
}

void render_temp()
{
#if !defined( NOGUI )
    BeginDrawing();
    ClearBackground( ColorData::BG );

    if ( DeveloperMode::isActive() )
    {
        DrawFPS( 0, 0 );
    }

    EndDrawing();
#endif
}

/// @brief Void argument in function signature needed for emscripten
void updateApp( void* arg )
{
    App& app = *(App*)arg;

    updateFullscreenState();

    app.dt = GetFrameTime();

    //* TODO: Update
    update_temp( app.dt );

    //* TODO: Render
    render_temp();
}

void App::setupAppEvents()
{
    snx::EventDispatcher::addListener(
        EventId::WINDOW_RESIZED,
        [&]() {},
        true
    );
}

void App::init( AppConfig const& config )
{
    setupFrameworks( config );

    setupAppEvents();
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
    //* Close window and opengl context
#if !defined( NOGUI )
    CloseWindow();
#else
    endwin();
#endif
}
