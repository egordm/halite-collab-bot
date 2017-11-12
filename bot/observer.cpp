//
// Created by egordm on 9-11-2017.
//

#include <algorithm>
#include "observer.h"
#include "constants.h"
#include "sorting.h"

namespace bot {
    Observer::Observer(hlt::PlayerId id, hlt::Map &map) : map(map), my_id(id) {}

    void Observer::observe(const hlt::Map &new_map) {
        for (const auto &kv : new_map.ships) {
            for (const auto &ship : kv.second) {
                const auto &prev_ship = get_ship(kv.first, ship.entity_id);
                if (prev_ship.second) velocities.insert(std::make_pair(ship.entity_id, ship.pos - prev_ship.first.pos));
            }
        }

        map = new_map;
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

    std::vector<hlt::Ship> Observer::all_ships() const {
        std::vector<hlt::Ship> ret;
        for (auto &key : map.ships) {
            ret.insert(ret.end(), key.second.begin(), key.second.end());
        }
        return ret;
    }

    std::vector<hlt::Ship>
    Observer::ships_around(const hlt::Vector &p, double radius, unsigned short owner_mask) const {
        std::vector<hlt::Ship> ret;
        for (auto &key : map.ships) {
            for (const auto &ship : key.second) {
                if (owner_mask & ship.owner_mask(my_id) == 0) continue;
                if (ship.pos.dist(p) < radius) ret.push_back(ship);
            }
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

    hlt::nullable<hlt::Planet> Observer::get_planet(hlt::EntityId planet_id) {
        if (map.planet_map.find(planet_id) == map.planet_map.end())
            return std::make_pair(hlt::Planet(), false);
        return std::make_pair(map.get_planet(planet_id), true);
    }

    hlt::nullable<hlt::Ship> Observer::get_ship(hlt::EntityId ship_id) {
        for (const auto &kv : map.ship_map) {
            if (kv.second.find(ship_id) != kv.second.end()) {
                return std::make_pair(map.ships.at(kv.first).at(map.ship_map.at(kv.first).at(ship_id)), true);
            }
        }
        return std::make_pair(hlt::Ship(), false);
    }

    /**
     * Definition of attacked: enemy ships is undocked and within defend range
     * @return
     */
    std::vector<hlt::Planet> Observer::get_attacked_planets() {
        std::vector<hlt::Planet> ret;
        for (const auto &planet : get_planets(hlt::friendly_mask)) {
            if (!filter(ships_around(planet.pos, planet.radius + constants::DEFEND_RADIUS, hlt::enemy_mask),
                        hlt::ShipDockingStatus::Undocked).empty())
                ret.push_back(planet);
        }
        return ret;
    }

    std::vector<hlt::Ship> Observer::filter(std::vector<hlt::Ship> v, hlt::ShipDockingStatus status) {
        std::vector<hlt::Ship> ret;
        std::back_insert_iterator<std::vector<hlt::Ship> > back_it(ret);
        std::copy_if(v.begin(), v.end(), back_it, sorting::IsDockStatus(status));
        return ret;
    }
}