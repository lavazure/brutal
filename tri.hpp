#ifndef BRUTAL_TRI_HPP
#define BRUTAL_TRI_HPP

#include "entity.hpp"
#include "opcodes.hpp"

#include <vector>
#include <cstdint>
#include <cstring>

namespace brutal
{

/*
    A class representing a sentinel
    type: 4
    subtype: +2, -3
*/
struct tri : public entity
{
    explicit tri(uint8_t subtype)
        : positive(true), entity(opcodes::entities::item, subtype)
    {
    }

    bool being_deleted;
    bool flail_grabbed;
    bool positive;
    uint16_t energy;
    float impulse;
    uint16_t hue;

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool is_full) override
    {
        float cur_x;
        float cur_y;
        float cur_angle;

        std::memcpy(&energy, data.data() + offset, sizeof(energy));
        offset += sizeof(energy);

        std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
        offset += sizeof(cur_x);

        std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
        offset += sizeof(cur_y);

        std::memcpy(&cur_angle, data.data() + offset, sizeof(cur_angle));
        offset += sizeof(cur_angle);

        x = cur_x * 10.0f;
        y = -cur_y * 10.0f;
        angle = cur_angle;

        uint8_t impulse_flag = data[offset];
        if (impulse_flag)
        {
            impulse = 1.0f;
        }
        offset += 1;

        if (is_full)
        {
            positive = static_cast<bool>(data[offset]);
            offset += 1;

            hue = positive ? 116 : 0;
        }

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

#endif // BRUTAL_TRI_HPP
