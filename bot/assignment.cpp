//
// Created by egordm on 10-11-2017.
//

#include <algorithm>
#include "assignment.h"
#include "utils.h"
#include "constants.h"
#include "sorting.h"

hlt::nullable<hlt::Ship> bot::Assignment::get_ship(bot::Observer &observer) {
    return observer.get_ship(observer.my_id, ship_id);
}

bool bot::Assignment::operator==(const bot::Assignment &rhs) const {
    return ship_id == rhs.ship_id && get_type() == rhs.get_type();
}

bool bot::Assignment::operator!=(const bot::Assignment &rhs) const {
    return !(rhs == *this);
}

hlt::nullable<hlt::Planet> bot::ColonizeAssignment::get_target(bot::Observer &observer) {
    return observer.get_planet(target_id);
}

hlt::nullable<hlt::Move> bot::ColonizeAssignment::get_move(bot::Observer &observer, bot::Navigator *navigator) {
    const auto &ret = TargetedAssignment::get_move(observer, navigator);
    if (!ret.second) return ret;
    return std::make_pair(navigator->dock_planet(get_ship(observer).first, get_target(observer).first), true);
}

bool bot::ColonizeAssignment::is_valid(bot::Observer &observer) {
    if (!TargetedAssignment::is_valid(observer)) return false;
    auto target = get_target(observer);
    return !target.first.is_full() && target.first.owner_mask(observer.my_id) != hlt::enemy_mask;
}

unsigned int bot::ColonizeAssignment::max_assignments(Observer &observer) {
    const auto &target = get_target(observer);
    return static_cast<unsigned int>(target.first.docking_spots - target.first.docked_ships.size());
}

hlt::nullable<hlt::Move> bot::AttackShipAssignment::get_move(bot::Observer &observer, bot::Navigator *navigator) {
    const auto &ret = TargetedAssignment::get_move(observer, navigator);
    if (!ret.second) return ret;
    return std::make_pair(navigator->attack_ship(get_ship(observer).first, get_target(observer).first,
                                                 observer.get_velocity(target_id)), true);

}

hlt::nullable<hlt::Ship> bot::AttackShipAssignment::get_target(bot::Observer &observer) {
    return observer.get_ship(target_id);
}

hlt::nullable<hlt::Ship> bot::AttackPlanetAssignment::get_target(bot::Observer &observer) {
    const auto &planet = observer.get_planet(target_id);
    if (!planet.second || planet.first.owner_mask(observer.my_id) != hlt::enemy_mask) {
        return std::make_pair(hlt::Ship(), false);
    }

    // Find the weakest enemy
    std::vector<hlt::Ship> weakest;
    for (const auto &ship_id : planet.first.docked_ships) {
        const auto &enemy = observer.get_ship(planet.first.owner_id, ship_id);
        if (!enemy.second) continue;

        if (weakest.empty()) weakest.push_back(enemy.first);
        else if (weakest[0].health >= enemy.first.health) {
            if (weakest[0].health != enemy.first.health) weakest.clear();
            weakest.push_back(enemy.first);
        }
    }

    return closest_object(weakest, get_ship(observer).first.pos);
}

hlt::nullable<hlt::Ship> bot::DefendPlanetAssignment::get_target(bot::Observer &observer) {
    const auto &planet = observer.get_planet(target_id);
    const auto &ship = get_ship(observer);
    if (!ship.second || !planet.second || planet.first.owner_mask(observer.my_id) != hlt::friendly_mask)
        return std::make_pair(hlt::Ship(), false);

    const auto &enemies = Observer::filter(observer.ships_around(planet.first.pos, planet.first.radius + constants::DEFEND_RADIUS,
                                              hlt::enemy_mask), hlt::ShipDockingStatus::Undocked);
    if(enemies.empty()) return std::make_pair(hlt::Ship(), false);
    const auto &enemy = std::min_element(enemies.begin(), enemies.end(), sorting::SortByDistance(ship.first.pos));

    return std::make_pair(*enemy, true);
}
