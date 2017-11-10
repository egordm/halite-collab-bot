//
// Created by egordm on 9-11-2017.
//

#include <sstream>
#include <log.hpp>
#include <utility>
#include "navigator.h"
#include "defines.h"
#include "utils.h"
#include "constants.h"

namespace bot {

    Navigator::Navigator(Observer &observer) : observer(observer) {}

    hlt::Move Navigator::dock_planet(const hlt::Ship &ship, const hlt::Planet &planet) {
        if (ship.can_dock(planet)) {
            return hlt::Move::dock(ship.entity_id, planet.entity_id);
        }
        auto dock_pos = ship.pos.closest_point(planet.pos, planet.radius + hlt::constants::DOCK_RADIUS - 0.1);

        NavEnv env(observer, ship);
        env.remove(ship);
        env.remove(planet);
        return env.move_towards(ship, dock_pos, true); // TODO: use closest point to
    }

    hlt::Move Navigator::attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel) {
        const auto pos = ship.pos.closest_point(target.pos,
                                                hlt::constants::SHIP_RADIUS); // TODO: crash if health < then enemy
        const auto dist = ship.pos.dist(pos);

        if (dist < hlt::constants::WEAPON_RADIUS - hlt::constants::SHIP_RADIUS) {
            auto speed = std::min(hlt::constants::MAX_SPEED, SINT(target_vel.length()));
            return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(target_vel.angle())));
        }

        NavEnv env(observer, ship);
        env.remove(ship);
        env.remove(target);
        return env.move_towards(ship, pos, true); // TODO: find intersection point and lean slightly towards
    }

    NavEnv::NavEnv() = default;

    NavEnv::NavEnv(std::vector<hlt::Planet> planets, std::vector<hlt::Ship> ships) : planets(std::move(planets)),
                                                                                     ships(std::move(ships)) {}

    NavEnv::NavEnv(Observer &observer, const hlt::Ship &ship) : planets(observer.getMap().planets),
                                                                ships(observer.ships_around(ship.pos,
                                                                                            constants::SHIP_AVOID_PROXIMITY,
                                                                                            hlt::all_mask)) {

    }

    void NavEnv::remove(const hlt::Ship &ship) {
        bot::erase(ships, ship);
    }

    void NavEnv::remove(const hlt::Planet &planet) {
        bot::erase(planets, planet);
    }

    template<class T>
    hlt::Move NavEnv::avoid(const hlt::Ship &ship, const T &target, int max_correctons) {
        auto radius = target.radius + ship.radius + 0.1; // + Epsilon

        if (ship.pos.in_radius(target.pos, radius)) {
            return move_towards(ship, ship.pos.closest_point(target.pos, radius), false);
        }

        // TODO: min
        const auto tangents = ship.pos.tangents(target.pos, radius);
        /* const auto closest =
                 ship.pos.dist_sq(tangents.first) < ship.pos.dist_sq(tangents.second) ? tangents.first : tangents.second;*/
        const auto closest = tangents.first;

        std::stringstream ss;
        ss << "Correct" << ship.pos << " Avoid " << target.entity_id << " at" << target.pos << " via " << closest;
        hlt::Log::log(ss.str());

        return move_towards(ship, closest, true, max_correctons - 1);
    }

    hlt::Move NavEnv::move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles) {
        return move_towards(ship, pos, avoid_obstacles, 5);
    }

    hlt::Move
    NavEnv::move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles, int max_correctons) {
        if (max_correctons == 0) return hlt::Move::noop();

        if (avoid_obstacles) {
            const auto pobs = obj_between(planets, ship.pos, pos);
            if (pobs.second) return avoid(ship, pobs.first, max_correctons);

            const auto sobs = obj_between(ships, ship.pos, pos);
            if (sobs.second) {
                return avoid(ship, sobs.first, max_correctons);
            }
        }

        auto speed = std::min(hlt::constants::MAX_SPEED, std::max(2, SINT(ship.pos.dist(pos))));
        return hlt::Move::thrust(ship.entity_id, speed, SINT(hlt::rad_to_deg(ship.pos.angle_between(pos))));
    }
}