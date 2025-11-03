#ifndef BRUTAL_MISC_HPP
#define BRUTAL_MISC_HPP

#include <cstdint>

namespace brutal {
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
