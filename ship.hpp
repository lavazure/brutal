#ifndef BRUTAL_SHIP_HPP
#define BRUTAL_SHIP_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "entity.hpp"
#include "opcodes.hpp"
#include "utils.hpp"

namespace brutal {

/*
    A class representing the flail
*/
struct flail_type {
    float x;
    float y;
    float angle;
    float radius;
};

/*
    A class representing a chain segment
*/
struct chain_segment {
    float x;
    float y;
};

/*
    A class representing the player
*/
struct ship : public entity {
    explicit ship()
        : entity(opcodes::entities::player, 0),
          transfer_energy(0),
          energy(0),
          hue(0),
          attached(true),
          attracting(false),
          invulnerable(false),
          shock(false),
          decay(false),
          still(false),
          inside(false),
          charging(false),
          flash_flail_value(0.0f),
          car_enabled(0),
          car_disabled(1),
          being_deleted(false),
          red_flail(false),
          red_flail_deployed(false),
          red_flail_time(0) {}

    uint16_t transfer_energy;
    uint32_t energy;

    // Chain
    std::vector<chain_segment> chain_segments;
    // Flail
    flail_type flail;

    uint16_t hue;
    bool attached;
    bool attracting;
    bool invulnerable;
    bool shock;
    bool decay;
    bool still;
    bool inside;
    bool charging;

    float flash_flail_value;

    uint8_t car_enabled;
    uint8_t car_disabled;

    bool being_deleted;

    uint8_t flag_flail_attached = 0x01;
    uint8_t flag_flail_attracting = 0x02;
    uint8_t flag_invulnerable = 0x04;
    uint8_t flag_shock = 0x08;
    uint8_t flag_decay = 0x10;
    uint8_t flag_still = 0x20;
    uint8_t flag_inside = 0x40;
    uint8_t flag_charging = 0x80;
    uint16_t flag_red_flail = 0x100;
    uint16_t flag_red_flail_deployed = 0x200;

    // Red Flail
    bool red_flail;
    bool red_flail_deployed;
    uint8_t red_flail_time;

    inline size_t update_chain_flail(std::vector<uint8_t>& data, size_t offset, bool is_full) {
        uint8_t num_segments = data[offset++];
        for (size_t i = 0; i < num_segments; ++i) {
            if (is_full && i >= chain_segments.size()) {
                chain_segments.push_back({0.0f, 0.0f});
            }

            float cur_x;
            float cur_y;

            std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
            offset += sizeof(cur_x);

            std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
            offset += sizeof(cur_y);

            chain_segment& segment = chain_segments[i];
            segment.x = cur_x * 10.0f;
            segment.y = -cur_y * 10.0f;
        }

        return offset;
    }

    inline size_t update_network_flail(std::vector<uint8_t>& data, size_t offset, bool is_full, uint8_t op) {
        float cur_x;
        float cur_y;
        float cur_angle;

        float flail_radius;

        std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
        offset += sizeof(cur_x);
        std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
        offset += sizeof(cur_y);
        std::memcpy(&cur_angle, data.data() + offset, sizeof(cur_angle));
        offset += sizeof(cur_angle);

        std::memcpy(&energy, data.data() + offset, sizeof(energy));
        offset += sizeof(energy);

        flail_radius = utils::energy_to_radius(energy);

        uint16_t flags = 0;
        if (op == 0xB3)  // OPCODE_ENTITY_INFO_V2
        {
            flags = data[offset++];
        } else {
            std::memcpy(&flags, data.data() + offset, sizeof(flags));
            offset += sizeof(flags);
        }

        attached = flags & flag_flail_attached;
        attracting = flags & flag_flail_attracting;
        invulnerable = flags & flag_invulnerable;
        shock = flags & flag_shock;
        decay = flags & flag_decay;
        still = flags & flag_still;
        inside = flags & flag_inside;
        charging = flags & flag_charging;

        if (op != 0xB3) {
            red_flail = flags & flag_red_flail;
            red_flail_deployed = flags & flag_red_flail_deployed;

            if (red_flail_deployed) {
                red_flail_time = data[offset++];
            }
        }

        flail.x = cur_x * 10.0f;
        flail.y = -cur_y * 10.0f;
        flail.angle = -cur_angle;
        flail.radius = flail_radius * 10.0f;

        return offset;
    }

    size_t update_network(std::vector<uint8_t>& data, size_t offset, bool is_full, uint8_t op) override {
        float cur_x;
        float cur_y;
        float cur_angle;

        transfer_energy = data[offset++];
        std::memcpy(&cur_x, data.data() + offset, sizeof(cur_x));
        offset += sizeof(cur_x);
        std::memcpy(&cur_y, data.data() + offset, sizeof(cur_y));
        offset += sizeof(cur_y);
        std::memcpy(&cur_angle, data.data() + offset, sizeof(cur_angle));
        offset += sizeof(cur_angle);

        x = cur_x * 10.0f;
        y = -cur_y * 10.0f;
        angle = cur_angle;

        offset = update_chain_flail(data, offset, is_full);
        offset = update_network_flail(data, offset, is_full, op);

        if (is_full) {
            std::memcpy(&hue, data.data() + offset, sizeof(hue));
            offset += sizeof(hue);
        }

        return offset;
    }

    size_t delete_network(std::vector<uint8_t>& data, size_t offset) override {
        being_deleted = true;
        return offset;
    }
};

}  // namespace brutal

#endif  // BRUTAL_SHIP_HPP
