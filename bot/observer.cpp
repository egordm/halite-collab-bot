//
// Created by egordm on 9-11-2017.
//

#include "observer.h"

namespace bot {
    Observer::Observer(hlt::PlayerId id, hlt::Map &map) : map(map), my_id(id) {}

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

    std::vector<hlt::Ship> Observer::my_ships() {
        return map.ships[my_id];
    }

    std::vector<hlt::Planet> Observer::get_planets(unsigned short owner_mask) const {
        std::vector<hlt::Planet> ret;
        for (auto &planet : map.planets) {
            if ((owner_mask & planet.owner_mask(my_id)) != 0) ret.push_back(planet);
        }
        return ret;
    }

    std::vector<hlt::Ship> Observer::get_enemies() const {
        std::vector<hlt::Ship> ret;
        for (auto &key : map.ships) {
            if (key.first != my_id) ret.insert(ret.end(), key.second.begin(), key.second.end());
        }
        return ret;
    }

    const hlt::Vector Observer::get_velocity(hlt::EntityId entity_id) const {
        if (velocities.find(entity_id) == velocities.end()) return hlt::Vector(); // TODO: mb unneeded.
        return velocities.at(entity_id);
    }

    hlt::nullable<hlt::Ship> Observer::get_ship(const hlt::PlayerId player_id, const hlt::EntityId ship_id) {
        if (map.ship_map.at(player_id).find(ship_id) == map.ship_map.at(player_id).end())
            return std::make_pair(hlt::Ship(), false);
        return std::make_pair(map.get_ship(player_id, ship_id), true);
    }

    hlt::Planet Observer::get_planet(hlt::EntityId planet_id) {
        return map.get_planet(planet_id);
    }

    hlt::nullable<hlt::Ship> Observer::get_ship(hlt::EntityId ship_id) {
        for (const auto &kv : map.ship_map) {
            if (kv.second.find(ship_id) != kv.second.end())
                return std::make_pair(map.ships.at(kv.first).at(ship_id), true);
        }
        return std::make_pair(hlt::Ship(), false);
    }
}