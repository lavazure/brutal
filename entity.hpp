#ifndef BRUTAL_ENTITY_HPP
#define BRUTAL_ENTITY_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "handlers.hpp"
#include "utils.hpp"

namespace brutal {
// all entities
struct entity {
    entity(uint8_t type, uint8_t subtype) : type(type), subtype(subtype), x(0.0f), y(0.0f), angle(0.0f) {}

    uint8_t type;
    uint8_t subtype;
    uint16_t id;
    std::u16string nick;
    float x;
    float y;
    float angle;

    uint8_t kill_reason;
    uint16_t killed_by_id;

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool is_full, uint8_t) {
        return update_network(data, offset, is_full);
    }

    virtual size_t update_network(std::vector<uint8_t>& data, size_t offset, bool is_full) {
        return update_network(data, offset);
    }

    virtual size_t update_network(std::vector<uint8_t>&, size_t offset) { return offset; }

    virtual size_t delete_network(std::vector<uint8_t>&, size_t offset) { return offset; }

    void set_create_handler(entity_create_handler callback) { m_create_handler = callback; }

    void set_update_handler(entity_update_handler callback) { m_update_handler = callback; }

    void set_delete_handler(entity_delete_handler callback) { m_delete_handler = callback; }

    entity_create_handler m_create_handler;
    entity_update_handler m_update_handler;
    entity_delete_handler m_delete_handler;
};

}  // namespace brutal

#endif // BRUTAL_ENTITY_HPP
