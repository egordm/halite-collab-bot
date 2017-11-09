//
// Created by egordm on 9-11-2017.
//

#include "navigator.h"
#include "defines.h"

namespace bot {

    Navigator::Navigator(const Observer &observer) : observer(observer) {}

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
                const auto tangents = ship.pos.tangents(obstacle.first.pos,
                                                        obstacle.first.radius + hlt::constants::BYPASS_PROXIMITY);
                const auto closest =
                        pos.dist(tangents.first) < pos.dist(tangents.second) ? tangents.first : tangents.second;
                return move_towards(ship, closest, avoid_obstacles);
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
        const auto dist = ship.pos.dist(target.pos);
        if (dist < target.radius + hlt::constants::WEAPON_RADIUS) {
            return hlt::Move::thrust(ship.entity_id, SINT(target_vel.length()),
                                     SINT(hlt::rad_to_deg(target_vel.angle())));
        }
        return move_towards(ship, target.pos, true); // TODO: find intersection point and lean slightly towards
    }
}