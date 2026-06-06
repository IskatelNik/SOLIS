#ifndef STATUS_EFFECT_H
#define STATUS_EFFECT_H

#include <string>

namespace solis {

struct StatusEffect {
    std::string type;
    float value;
    int duration_turns;
};

} // namespace solis

#endif // STATUS_EFFECT_H
