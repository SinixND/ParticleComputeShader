#ifndef IG20250401144844
#define IG20250401144844

#include <raylib.h>

/// Configurations for the application
namespace AppData
{
#if !defined( NOGUI )
    Image const FAVICON{ LoadImage( "assets/favicon/favicon.png" ) };
#endif

    int constexpr FPS_TARGET{ 300 };

    int constexpr EXIT_KEY( KEY_ESCAPE );
};

#endif
