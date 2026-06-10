#ifndef ARTIFACT_H
#define ARTIFACT_H

#include <string>

namespace solis {

struct Artifact {
    std::string id;
    std::string name;
    std::string description;
    std::string modifier_type; // e.g., "heat_gain_mult", "damage_mult"
    float value;
};

} // namespace solis

#endif // ARTIFACT_H
