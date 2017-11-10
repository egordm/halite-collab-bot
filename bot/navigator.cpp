//
// Created by egordm on 9-11-2017.
//

#include <sstream>
#include "navigator.h"
#include "defines.h"

namespace bot {

    Navigator::Navigator(Observer &observer) : observer(observer) {}

    hlt::nullable<hlt::Planet> Navigator::planet_between(const hlt::Vector &a, const hlt::Vector &b) {
        for (const auto &planet : observer.getMap().planets) {
            if (planet.pos.dist_line(a, b) <= planet.radius + hlt::constants::SHIP_RADIUS) {
                return std::make_pair(planet, true);
            }
        }
        return std::make_pair(hlt::Planet(), false);
    }

    hlt::Move Navigator::move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles) {
        if (avoid_obstacles) {
            const auto obstacle = planet_between(ship.pos, pos);
            if (obstacle.second) {
                return move_towards(ship, ship.pos.avoid_point(obstacle.first.pos, obstacle.first.radius + hlt::constants::BYPASS_PROXIMITY), avoid_obstacles);
            }
        }

        auto speed = std::min(hlt::constants::MAX_SPEED, SINT(ship.pos.dist(pos)));
        return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(ship.pos.angle_between(pos))));
    }

    hlt::Move Navigator::dock_planet(const hlt::Ship &ship, const hlt::Planet &planet) {
        if (ship.can_dock(planet)) {
            return hlt::Move::dock(ship.entity_id, planet.entity_id);
        }
        return move_towards(ship, planet.pos, true); // TODO: use closest point to
    }

    hlt::Move Navigator::attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel) {
        const auto pos = ship.pos.closest_point(target.pos,
                                                hlt::constants::SHIP_RADIUS * 2); // TODO: crash if health < then enemy
        const auto dist = ship.pos.dist(pos);

        if (dist < hlt::constants::WEAPON_RADIUS) {
            auto speed = std::min(hlt::constants::MAX_SPEED, SINT(target_vel.length()));
            return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(target_vel.angle())));
        }
        return move_towards(ship, pos, true); // TODO: find intersection point and lean slightly towards
    }
}