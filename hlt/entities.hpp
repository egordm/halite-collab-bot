
#ifndef MYBOT_ENTITIES_H
#define MYBOT_ENTITIES_H

#include <vector>
#include "vector.hpp"
#include "types.hpp"

namespace hlt {
    struct Entity {
        Entity() = default;

        EntityId entity_id{};
        PlayerId owner_id{};
        Vector pos{};
        int health{};
        double radius{};

        bool is_alive() const {
            return health > 0;
        }
    };

    enum class ShipDockingStatus {
        Undocked = 0,
        Docking = 1,
        Docked = 2,
        Undocking = 3,
    };

    struct Planet : Entity {
        bool owned;

        /// The remaining resources.
        int remaining_production;

        /// The currently expended resources.
        int current_production;

        /// The maximum number of ships that may be docked.
        unsigned int docking_spots;

        /// Contains IDs of all ships in the process of docking or undocking,
        /// as well as docked ships.
        std::vector<EntityId> docked_ships;

        bool is_full() const {
            return docked_ships.size() == docking_spots;
        }
    };

    struct Ship : Entity {
        /// The turns left before the ship can fire again.
        int weapon_cooldown{};

        ShipDockingStatus docking_status;

        /// The number of turns left to complete (un)docking.
        int docking_progress{};

        EntityId docked_planet{};

        bool can_dock(const Planet &planet) const {
            return pos.dist(planet.pos) <= (constants::SHIP_RADIUS + constants::DOCK_RADIUS + planet.radius);
        }
    };


}


#endif //MYBOT_ENTITIES_H
