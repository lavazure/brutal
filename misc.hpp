#ifndef BRUTAL_MISC_HPP
#define BRUTAL_MISC_HPP

#include <cstdint>
#include <unordered_map>

#include "entity_types.hpp"
#include "opcodes.hpp"

namespace brutal {

class entity_map {
public:
    entity_ptr& operator[](uint16_t id) {
        return m_map[id];
    }

    template <typename T>
    std::unordered_map<uint16_t, std::shared_ptr<T>> get_map(std::function<bool(const entity_ptr&)> predicate) {
        std::unordered_map<uint16_t, std::shared_ptr<T>> result;
        for (auto it = m_map.begin(); it != m_map.end(); ++it) {
            if (predicate(it->second)) {
                result[it->first] = std::static_pointer_cast<T>(it->second);
            }
        }
        return result;
    }

    std::unordered_map<uint16_t, ship_ptr> get_ship_map() {
        return get_map<ship>([](const entity_ptr& e) {
            return e->type == opcodes::entities::player;
        });
    }

    std::unordered_map<uint16_t, atom_ptr> get_atom_map() {
        return get_map<atom>([](const entity_ptr& e) {
            return e->type == opcodes::entities::item && e->subtype == opcodes::entities::atom;
        });
    }

    std::unordered_map<uint16_t, energy_ptr> get_energy_map() {
        return get_map<energy>([](const entity_ptr& e) {
            return e->type == opcodes::entities::item && e->subtype == opcodes::entities::energy;
        });
    }

    std::unordered_map<uint16_t, tri_ptr> get_tri_map() {
        return get_map<tri>([](const entity_ptr& e) {
            return e->type == opcodes::entities::item &&
                   (e->subtype == opcodes::entities::tri_plus || e->subtype == opcodes::entities::tri_minus);
        });
    }

    std::unordered_map<uint16_t, redflail_ptr> get_redflail_map() {
        return get_map<red_flail_powerup>([](const entity_ptr& e) {
            return e->type == opcodes::entities::item && e->subtype == opcodes::entities::redflail;
        });
    }

    std::unordered_map<uint16_t, redflail_ptr> get_red_flail_map() {
        return get_redflail_map();
    }

    std::unordered_map<uint16_t, collider_ptr> get_collider_map() {
        return get_map<collider>([](const entity_ptr& e) {
            return e->type == opcodes::entities::collider;
        });
    }

    void erase(uint16_t id) {
        m_map.erase(id);
    }

    std::unordered_map<uint16_t, entity_ptr>::iterator begin() {
        return m_map.begin();
    }

    std::unordered_map<uint16_t, entity_ptr>::iterator end() {
        return m_map.end();
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
