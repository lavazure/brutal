#ifndef BRUTAL_ATOM_HPP
#define BRUTAL_ATOM_HPP

#include "entity.hpp"
#include "opcodes.hpp"

#include <vector>
#include <cstdint>
#include <cstddef>
#include <cstring>

namespace brutal
{

/*
    A class representing an atom
    type: 4
    subtype: 0
*/
struct atom : public entity
{
    atom()
        : entity(opcodes::entities::item, opcodes::entities::item_atom)
    {
    }

    uint16_t energy;
    uint16_t hue;
    bool being_deleted;
    bool flail_grabbed;

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

        if (is_full)
        {
            std::memcpy(&hue, data.data() + offset, sizeof(hue));
            offset += sizeof(hue);
        }

        return offset;
    }

    virtual inline size_t delete_network(std::vector<uint8_t>& data, size_t offset) override
    {
        flail_grabbed = static_cast<bool>(data[offset]);
        offset += 1;

        being_deleted = true;

        return offset;
    }
};

} // namespace brutal

#endif // BRUTAL_ATOM_HPP
