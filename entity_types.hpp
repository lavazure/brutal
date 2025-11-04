#ifndef BRUTAL_ENTITY_TYPES_HPP
#define BRUTAL_ENTITY_TYPES_HPP

#include <memory>

#include "entity.hpp"
#include "atom.hpp"
#include "ship.hpp"
#include "collider.hpp"
#include "energy.hpp"
#include "tri.hpp"
#include "red_flail_powerup.hpp"

namespace brutal {
// Typedef Entity Pointers
typedef std::shared_ptr<entity> entity_ptr;

typedef std::shared_ptr<atom> atom_ptr;

typedef std::shared_ptr<energy> energy_ptr;

typedef std::shared_ptr<collider> collider_ptr;

typedef std::shared_ptr<tri> tri_ptr;

typedef std::shared_ptr<ship> ship_ptr;

typedef std::shared_ptr<red_flail_powerup> red_flail_ptr;

typedef std::shared_ptr<red_flail_powerup> redflail_ptr;
}

#endif // BRUTAL_ENTITY_TYPES_HPP
