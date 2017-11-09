//
// Created by egordm on 8-11-2017.
//

#include <log.hpp>
#include <sstream>
#include "navigation.h"
#include "defines.h"

namespace bot {
    namespace navigation {

        hlt::possibly<hlt::Planet> planet_between(const hlt::Map &map, const hlt::Vector &a, const hlt::Vector &b) {
            for (const auto &planet : map.planets) {
                if (planet.pos.dist_line(a, b) <= planet.radius + hlt::constants::SHIP_RADIUS) {
                    return std::make_pair(planet, true);
                }
            }
            return std::make_pair(hlt::Planet(), false);
        }

        hlt::Move
        move_towards(const hlt::Map &map, const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles) {
            if (avoid_obstacles) {
                const auto obstacle = planet_between(map, ship.pos, pos);
                if (obstacle.second) {
                    const auto tangents = ship.pos.tangents(obstacle.first.pos,
                                                            obstacle.first.radius + hlt::constants::BYPASS_PROXIMITY);
                    const auto closest =
                            pos.dist(tangents.first) < pos.dist(tangents.second) ? tangents.first : tangents.second;
                    return move_towards(map, ship, closest, avoid_obstacles);
                }
            }

            auto speed = std::min(hlt::constants::MAX_SPEED, SINT(ship.pos.dist(pos)));
            return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(ship.pos.angle_between(pos))));
        }

        hlt::Move dock_planet(const hlt::Map &map, const hlt::Ship &ship, const hlt::Planet &planet) {
            if (planet.pos.dist(ship.pos) < planet.radius + hlt::constants::DOCK_RADIUS) {
                return hlt::Move::dock(ship.entity_id, planet.entity_id);
            }
            return move_towards(map, ship, planet.pos, true); // TODO: use closest point to
        }

        hlt::Move attack_ship(const hlt::Map &map, const hlt::Ship &ship, const hlt::Ship &target,
                              const hlt::Vector &target_vel) {
            const auto dist = ship.pos.dist(target.pos);
            if (dist < target.radius + hlt::constants::WEAPON_RADIUS) {
                return hlt::Move::thrust(ship.entity_id, SINT(target_vel.length()),
                                         SINT(hlt::rad_to_deg(target_vel.angle())));
            }
            return move_towards(map, ship, target.pos, true); // TODO: find intersection point and lean slightly towards
        }
    }
}