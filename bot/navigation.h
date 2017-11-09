//
// Created by egordm on 8-11-2017.
//

#ifndef MYBOT_NAVIGATION_H
#define MYBOT_NAVIGATION_H

#include <move.hpp>
#include <entities.hpp>
#include <map.hpp>
#include <algorithm>

namespace bot {
    namespace navigation {
        static std::vector<const hlt::Entity *> obj_between(const hlt::Map& map, const hlt::Vector& start, const hlt::Vector& target) {
            std::vector<const hlt::Entity *> ret;

            /*for (const hlt::Planet& planet : map.planets) {
                check_and_add_entity_between(entities_found, start, target, planet);
            }

            for (const auto& player_ship : map.ships) {
                for (const Ship& ship : player_ship.second) {
                    check_and_add_entity_between(entities_found, start, target, ship);
                }
            }*/

            return ret;
        }

        hlt::Move move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles) {
            if(avoid_obstacles) {
                // TODO: get first object on the path
                // TODO: find the right tangent and navigate thereto
            }

            auto dist = ship.pos.dist(pos);
            return hlt::Move::thrust(ship.entity_id,
                                     std::min(hlt::constants::MAX_SPEED, static_cast<const int &>(dist)),
                                     static_cast<const int>(pos.angle_between(pos)));
        }
    }
}



#endif //MYBOT_NAVIGATION_H
