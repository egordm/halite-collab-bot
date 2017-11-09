//
// Created by egordm on 9-11-2017.
//

#include "observer.h"

namespace bot {
    Observer::Observer(hlt::PlayerId id, hlt::Map &map) : my_id(id), map(map) {}

    void Observer::observe(const hlt::Map &new_map) {
        for (unsigned int i = 0; i < new_map.ships.size(); ++i) {
            for (unsigned int j = 0; j < new_map.ships.at(i).size(); ++j) {
                const auto &ship = new_map.ships.at(i)[j];
                velocities.insert(std::make_pair(ship.entity_id, ship.pos - this->map.ships[i][j].pos));
            }
        }

        this->map = new_map;
        step++;
    }

    const hlt::Map &Observer::getMap() const {
        return map;
    }

    hlt::nullable<hlt::Planet> Observer::closest_planet(hlt::Vector pos) {
        return closest_planet(pos, hlt::empty_mask);
    }

    hlt::nullable<hlt::Planet> Observer::closest_planet(hlt::Vector pos, unsigned short owner_mask) const {
        double dist = 10000000;
        hlt::nullable<hlt::Planet> ret;
        for (const auto &planet : map.planets) {
            if ((owner_mask & planet.owner_mask(my_id)) == 0) continue;

            double d = pos.dist_sq(planet.pos);
            if (d < dist) {
                dist = d;
                ret.first = planet;
                ret.second = true;
            }
        }
        return ret;
    }

    const std::vector<hlt::Ship> Observer::my_ships() {
        return map.ships[my_id];
    }
}