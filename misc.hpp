#ifndef BRUTAL_MISC_HPP
#define BRUTAL_MISC_HPP

#include <cstdint>
#include <unordered_map>

#include "entity_types.hpp"
#include "opcodes.hpp"

#define get_typed(entity) \
    if ((entity)->type == 5) \
        return *reinterpret_cast<brutal::ship*>(entity); \
    else if ((entity)->type == 1) \
        return *reinterpret_cast<brutal::collider*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 0) \
        return *reinterpret_cast<brutal::atom*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 1) \
        return *reinterpret_cast<brutal::energy*>(entity); \
    else if ((entity)->type == 4 && ((entity)->subtype == 2 || (entity)->subtype == 3)) \
        return *reinterpret_cast<brutal::tri*>(entity); \
    else if ((entity)->type == 4 && (entity)->subtype == 4) \
        return *reinterpret_cast<brutal::red_flail_powerup*>(entity);


namespace brutal {
// Entity Compnent System
class entity_map {
public:
    entity_ptr& operator[](uint16_t id) {
        return m_map[id];
    }

    std::unordered_map<uint16_t, ship_ptr> get_ship_map() {
        std::unordered_map<uint16_t, ship_ptr> ship_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::ship) return;
            ship_map[pair.first] = pair.second;
        }

       return ship_map;
    }

    std::unordered_map<uint16_t, atom_ptr> get_atom_map() {
        std::unordered_map<uint16_t, atom_ptr> atom_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::item || pair.second->subtype != opcodes::entities::atom) return;
            atom_map[pair.first] = pair.second;
        }

       return atom_map;
    }

    std::unordered_map<uint16_t, energy_ptr> get_energy_map() {
        std::unordered_map<uint16_t, energy_ptr> energy_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::item || pair.second->subtype != opcodes::entities::energy) return;
            energy_map[pair.first] = pair.second;
        }

       return energy_map;
    }

    std::unordered_map<uint16_t, collider_ptr> get_collider_map() {
        std::unordered_map<uint16_t, collider_ptr> collider_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::collider) return;
            collider_map[pair.first] = pair.second;
        }

       return collider_map;
    }

    std::unordered_map<uint16_t, tri_ptr> get_tri_map() {
        std::unordered_map<uint16_t, tri_ptr> tri_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::item || (pair.second->subtype != opcodes::entities::tri_plus && pair.second->subtype != opcodes::entities::tri_minus)) return;
            tri_map[pair.first] = pair.second;
        }

       return tri_map;
    }

    std::unordered_map<uint16_t, redflail_ptr> get_redflail_map() {
        std::unordered_map<uint16_t, redflail_ptr> redflail_map;

        for (auto& pair: m_map) {
            if(pair.second->type != opcodes::entites::item || pair.second->subtype != opcodes::entities::redflail) return;
            redflail_map[pair.first] = pair.second;
        }

       return redflail_map;
    }

    std::unordered_map<uint16_t, redflail_ptr> get_red_flail_map() {
        return get_redflail_map();
    }

    void erase(uint16_t id) {
        auto it = m_map.find(id);
        if(it != m_map.end()) m_map.erase(it);
    }

    std

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
