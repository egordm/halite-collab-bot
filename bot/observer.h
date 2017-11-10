//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include "../hlt/map.hpp"

namespace bot {
    class Observer {
    private:
        hlt::Map &map;
        hlt::entity_map<hlt::Vector> velocities;
    public:
        const hlt::PlayerId my_id;
        unsigned int step;

        Observer(hlt::PlayerId id, hlt::Map &map);

        void observe(const hlt::Map &new_map);

        const hlt::Vector get_velocity(hlt::EntityId entity_id) const;

        const hlt::Map &getMap() const;

        std::vector<hlt::Ship> my_ships();

        std::vector<hlt::Ship> get_enemies() const;

        std::vector<hlt::Ship> all_ships() const;

        std::vector<hlt::Ship> ships_around(const hlt::Vector &p, double radius, unsigned short owner_mask) const;

        hlt::nullable<hlt::Ship> get_ship(hlt::PlayerId player_id, hlt::EntityId ship_id);

        hlt::nullable<hlt::Ship> get_ship(hlt::EntityId ship_id);

        std::vector<hlt::Planet> get_planets(unsigned short owner_mask) const;

        hlt::nullable<hlt::Planet> get_planet(hlt::EntityId planet_id);
    };
}

#endif //MYBOT_OBSERVER_H
