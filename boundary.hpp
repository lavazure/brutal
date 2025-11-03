#ifndef BRUTAL_BOUNDARY_HPP
#define BRUTAL_BOUNDARY_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include "opcodes.hpp"
#include "entity.hpp"

namespace brutal {
/*
    A class representing the boundary
    type: 1
    subtype: 3
*/
struct boundary : public entity {
    boundary() : entity(opcodes::entities::collider, opcodes::entities::boundary) {}

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool is_full) override { return offset; }

    virtual size_t delete_network(std::vector<uint8_t>& data, size_t offset) override { return offset; }
};

}  // namespace brutal

#endif  // BRUTAL_BOUNDARY_HPP
