//
// Created by egordm on 9-11-2017.
//

#include <sstream>
#include <utility>
#include <log.hpp>
#include "navigator.h"
#include "defines.h"
#include "path.hpp"


namespace bot {

    Navigator::Navigator(Observer &observer) : observer(observer) {}

    hlt::Move Navigator::dock_planet(const hlt::Ship &ship, const hlt::Planet &planet) {
        if (ship.can_dock(planet)) {
            return hlt::Move::dock(ship.entity_id, planet.entity_id);
        }

        return Path<hlt::Planet>(observer, ship, planet).move(planet.radius + hlt::constants::DOCK_RADIUS - 1);
    }

    hlt::Move Navigator::attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel) {
        // TODO: crash if health < then enemy
        const auto dist = ship.pos.dist(target.pos);

        if (dist < hlt::constants::WEAPON_RADIUS) {
            auto speed = std::min(hlt::constants::MAX_SPEED, SINT(target_vel.length()));
            return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(target_vel.angle())));
        }

        // TODO: find intersection point and lean slightly towards
        return Path<hlt::Ship>(observer, ship, target).move(target.radius + hlt::constants::WEAPON_RADIUS -2);
    }
}