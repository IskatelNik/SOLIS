#ifndef EVENT_H
#define EVENT_H

#include <string>

namespace solis {

struct EventChoice {
    std::string text;
    float heat_change;
    int ideology_change;
    std::string result_text;
};

struct Event {
    std::string id;
    int level;
    int order_index; // For linear progression: 0, 1, 2...
    std::string title;
    std::string description;
    EventChoice choice_ficio;
    EventChoice choice_finesa;
};

} // namespace solis

#endif // EVENT_H
