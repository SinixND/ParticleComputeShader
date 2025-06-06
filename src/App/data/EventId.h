#ifndef IG20240315181752
#define IG20240315181752

#include <string>
#include <unordered_map>

enum class EventId
{
    NONE = 0,
    WINDOW_RESIZED,
};

inline std::unordered_map<EventId, std::string> const eventIdToName{
    { EventId::NONE, "None" },
    { EventId::WINDOW_RESIZED, "Window resized" },
};

#endif
