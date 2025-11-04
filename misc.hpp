#ifndef BRUTAL_MISC_HPP
#define BRUTAL_MISC_HPP

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <type_traits>

#include "entity_types.hpp"
#include "opcodes.hpp"

namespace brutal {

class entity_map {
public:
    entity_ptr& at(uint16_t id) {
        return m_map.at(id);
    }

    entity_ptr& operator[](uint16_t id) {
        return m_map[id];
    }

    void erase(uint16_t id) {
        m_map.erase(id);
    }

    auto begin() { return m_map.begin(); }
    auto end() { return m_map.end(); }

    template <typename T, typename Predicate>
    std::unordered_map<uint16_t, std::shared_ptr<T>> get_map(Predicate&& pred) {
        std::unordered_map<uint16_t, std::shared_ptr<T>> result;
        for (auto& [id, ptr] : m_map) {
            if (ptr && pred(*ptr)) {
                if (auto casted = std::dynamic_pointer_cast<T>(ptr)) {
                    result[id] = casted;
                }
            }
        }
        return result;
    }

    std::unordered_map<uint16_t, ship_ptr> get_ship_map() {
        return get_map<ship>([](const entity& e) {
            return e.type == opcodes::entities::player;
        });
    }

    std::unordered_map<uint16_t, atom_ptr> get_atom_map() {
        return get_map<atom>([](const entity& e) {
            return e.type == opcodes::entities::item &&
                   e.subtype == opcodes::entities::atom;
        });
    }

    std::unordered_map<uint16_t, energy_ptr> get_energy_map() {
        return get_map<energy>([](const entity& e) {
            return e.type == opcodes::entities::item &&
                   e.subtype == opcodes::entities::energy;
        });
    }

    std::unordered_map<uint16_t, tri_ptr> get_tri_map() {
        return get_map<tri>([](const entity& e) {
            return e.type == opcodes::entities::item &&
                   (e.subtype == opcodes::entities::tri_plus || e.subtype == opcodes::entities::tri_minus);
        });
    }

    std::unordered_map<uint16_t, redflail_ptr> get_red_flail_map() {
        return get_map<red_flail_powerup>([](const entity& e) {
            return e.type == opcodes::entities::item &&
                   e.subtype == opcodes::entities::redflail;
        });
    }

    std::unordered_map<uint16_t, collider_ptr> get_collider_map() {
        return get_map<collider>([](const entity& e) {
            return e.type == opcodes::entities::collider;
        });
    }

private:
    std::unordered_map<uint16_t, entity_ptr> m_map;
};

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
