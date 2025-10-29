#ifndef BRUTAL_COLLIDER_HPP
#define BRUTAL_COLLIDER_HPP

#include "entity.hpp"
#include "opcodes.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace brutal
{

/*
    A class representing a collider
    type: 4
*/
struct collider : public entity
{
    explicit collider(uint8_t subtype)
        : entity(opcodes::entities::collider, subtype),
          shape_index(0),
          pulsing(false),
          pulse_value(0.0f),
          hit_value(0.0f),
          count(0),
          core_stage(0),
          core_rotation(0.0f)
    {
    }

    uint8_t shape_index;
    bool pulsing;
    float pulse_value;
    float hit_value;
    uint8_t count;
    uint8_t core_stage;
    float core_rotation;

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool /*is_full*/) override
    {
        float cur_x;
        float cur_y;
        float cur_angle;

        std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
        offset += sizeof(cur_x);

        std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
        offset += sizeof(cur_y);

        std::memcpy(&cur_angle, data.data() + offset, sizeof(cur_angle));
        offset += sizeof(cur_angle);

        shape_index = data[offset];
        offset += 1;

        if (subtype == opcodes::entities::bouncer)
        {
            uint8_t did_hit = data[offset++];
            if (did_hit)
            {
                hit_value = 1.0f;
            }
            count = data[offset++];
        }
        else if (subtype == opcodes::entities::core)
        {
            uint8_t flags = data[offset++];
            core_stage = flags & static_cast<uint8_t>(~0x8);
            uint8_t pulse_flag = flags & 0x8;
            pulsing = (pulse_flag != 0);

            std::memcpy(&core_rotation, data.data() + offset, sizeof(core_rotation));
            offset += sizeof(core_rotation);
        }

        x = cur_x;
        y = -cur_y;
        angle = cur_angle;

        return offset;
    }

    virtual size_t delete_network(std::vector<uint8_t>& /*data*/, size_t offset) override
    {
        return offset;
    }
};

} // namespace brutal

#endif // BRUTAL_COLLIDER_HPP
