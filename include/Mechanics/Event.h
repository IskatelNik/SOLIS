#ifndef EVENT_H
#define EVENT_H

#include <string>

namespace solis {

struct EventChoice {
    std::string text;
    float heat_change;
    int ideology_change;
};

struct Event {
    std::string id;
    int level;
    int order_index; // For linear progression: 0, 1, 2...
    std::string preview_text;
    std::string description;
    std::vector<EventChoice> choices;
};

} // namespace solis

#endif // EVENT_H
