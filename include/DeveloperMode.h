#ifndef IG20240513010437
#define IG20240513010437

#include "Singleton.h"

class DeveloperMode : public snx::Singleton<DeveloperMode>
{
#if defined( DEBUG )
    static inline bool debugMode_{ true };
#else
    static inline bool debugMode_{ false };
#endif

public:
    static bool isActive()
    {
        return instance().debugMode_;
    }

    static void toggle() { instance().debugMode_ = !instance().debugMode_; }
};

#endif
