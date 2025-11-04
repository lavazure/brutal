#ifndef BRUTAL_MISC_HPP
#define BRUTAL_MISC_HPP

#include <cstdint>
#include <unordered_map>

#include "entity.hpp"

#define get_typed(entity) \
    if ((entity)->type == 5) \
        return *reinterpret_cast<brutal::ship*>(entity); \
    else if ((entity)->type == 1) \
        return *reinterpret_cast<brutal::collider*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 0) \
        return *reinterpret_cast<brutal::atom*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 1) \
        return *reinterpret_cast<brutal::energy*>(entity); \
    else if ((entity)->type == 4 && ((entity)->subtype == 2 || (entity)->subtype == 3)) \
        return *reinterpret_cast<brutal::tri*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 4) \
        return *reinterpret_cast<brutal::red_flail_powerup*>(entity);


namespace brutal {
// Entity Pointer
typedef entity* entity_ptr;

// Entity Compnent System
class entity_map {};

// leaderboard
struct leaderboard_item {
    std::u16string nick;
    uint32_t score;
    uint16_t id;
    uint16_t rank;
};

typedef std::vector<leaderboard_item> leaderboard;

// minimap
struct minimap_object {
    uint8_t x;
    uint8_t y;
    uint8_t r;
};

typedef std::vector<minimap_object> minimap;

} // namespace brutal

#endif // BRUTAL_MISC_HPP
