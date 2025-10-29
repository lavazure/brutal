#ifndef BRUTAL_RED_FLAIL_POWERUP_HPP
#define BRUTAL_RED_FLAIL_POWERUP_HPP

#include "entity.hpp"
#include "opcodes.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace brutal
{

/*
    A class representing the red flail powerup
    type: 4
    subtype: 4
*/
struct red_flail_powerup : public entity
{
    red_flail_powerup()
        : entity(opcodes::entities::item, opcodes::entities::item_redflail)
    {}

    uint16_t energy;
    uint16_t hue;

    bool being_deleted;
    bool flail_grabbed;

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool /*is_full*/) override
    {
        float cur_x;
        float cur_y;

        std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
        offset += sizeof(cur_x);

        std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
        offset += sizeof(cur_y);

        x = cur_x * 10.0f;
        y = -cur_y * 10.0f;

        return offset;
    }

    virtual size_t delete_network(std::vector<uint8_t>& data, size_t offset) override
    {
        flail_grabbed = static_cast<bool>(data[offset]);
        offset += 1;
        
        being_deleted = true;
        return offset;
    }
};

} // namespace brutal

#endif // BRUTAL_RED_FLAIL_POWERUP_HPP
