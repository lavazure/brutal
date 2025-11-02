#ifndef BRUTAL_OPCODES_HPP
#define BRUTAL_OPCODES_HPP

#include <cstdint>

namespace brutal {
namespace opcodes {
// opcode type
typedef const uint8_t opcode;

// Client -> Server
struct client {
    static opcode ping             = 0x00;
    static opcode hello            = 0x01;
    static opcode hello_bot        = 0x02; // For Bots
    static opcode enter_game       = 0x03;
    static opcode leave_game       = 0x04;
    static opcode input            = 0x05;
    static opcode input_brake      = 0x06;
    static opcode area_update      = 0x07;
    static opcode click            = 0x08;
};

// Server -> Client
struct server {
    static opcode pong             = 0x00;
    static opcode map_config       = 0xA0;
    static opcode entered_game     = 0xA1;
    static opcode entity_info_v1   = 0xB4;
    static opcode entity_info_v2   = 0xB3;
    static opcode events           = 0xA4;
    static opcode leaderboard_v1   = 0xA5;
    static opcode leaderboard_v2   = 0xB5;
    static opcode minimap          = 0xA6;
};

// Event Codes
struct events {
    static opcode did_kill         = 0x01;
    static opcode was_killed       = 0x02;
};

// Entity Types
struct entities {
    static opcode item             = 4;
    static opcode player           = 5;
    static opcode collider         = 1;

    // Entity SubTypes
    // PLAYER
    static opcode basic_car        = 0;
    static opcode flail            = 1;
    static opcode chain            = 2;

    // Colliders
    static opcode bouncer          = 0;
    static opcode wall             = 1;
    static opcode danger           = 2;
    static opcode boundary         = 3;
    static opcode noflailzone      = 4;
    static opcode core             = 5;

    // ENTITY_ITEM
    static opcode atom             = 0;
    static opcode energy           = 1;
    static opcode tri_plus         = 2;
    static opcode tri_minus        = 3;
    static opcode red_flail        = 4;

    // Shapes
    static opcode shape_circle      = 0x01;
    static opcode shape_poly       = 0x02;
};

// Kill Reasons
struct kill_reasons {
    static opcode left_screen      = 0;
    static opcode killed           = 1;
    static opcode sentinel         = 2;
    static opcode electrocuted     = 3;
    static opcode suicide          = 4;
    static opcode vortex           = 5;
};

// Core
struct core_stage {
    static opcode idle             = 0x0;
    static opcode charging         = 0x1;
    static opcode warming          = 0x2;
    static opcode vortex           = 0x4;
};

} // namespace opcodes
} // namespace brutal

#endif
