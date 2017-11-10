//
// Created by egordm on 9-11-2017.
//

#include "commander.h"
#include "utils.h"
#include "constants.h"


bot::Commander::Commander(bot::Observer &observer, bot::Navigator &navigator) : observer(observer),
                                                                                navigator(navigator) {}

std::vector<hlt::Move> bot::Commander::command() {
    for (int i = static_cast<int>(assignments.size() - 1); i >= 0; --i) {
        if (!valid_assignment(assignments[i])) assignments.erase(assignments.begin() + i);
    }

    for (const auto &ship : observer.my_ships()) {
        if(get_assignment(ship.entity_id).second) continue;
        if (ship.docking_status != hlt::ShipDockingStatus::Undocked) continue;
        assignments.push_back(assign(ship));
    }

    std::vector<hlt::Move> ret;
    for (const auto &ass : assignments) {
        ret.push_back(produce_move(ass));
    }

    return ret;
}

bot::Assignment bot::Commander::assign(const hlt::Ship &ship) {
    auto empty_planets = observer.get_planets(hlt::empty_mask + hlt::friendly_mask); // TODO: do what cody did
    while (!empty_planets.empty()) {
        auto planet = closest_object(empty_planets, ship.pos);
        if (!planet.second) continue;
        bot::erase(empty_planets, planet.first);

        Assignment ass(ship.entity_id, planet.first.entity_id, Assignment::Type::ColonizePlanet);
        if (valid_assignment(ass)) return ass;
    }

    auto enemy_planets = observer.get_planets(hlt::empty_mask); // TODO: do what cody did
    while (!enemy_planets.empty()) {
        auto planet = closest_object(enemy_planets, ship.pos);
        if (!planet.second) continue;
        bot::erase(enemy_planets, planet.first);

        Assignment ass(ship.entity_id, planet.first.entity_id, Assignment::Type::AttackPlanet);
        if (valid_assignment(ass)) return ass;
    }

    auto enemy = closest_object(observer.get_enemies(), ship.pos);
    if (enemy.second) return {ship.entity_id, enemy.first.entity_id, Assignment::Type::AttackShip};

    return {0, 0, Assignment::Type::NoOp};
}

hlt::Move bot::Commander::produce_move(const bot::Assignment &assignment) {
    auto ship = observer.get_ship(observer.my_id, assignment.ship_id);
    if (!ship.second) return hlt::Move::noop();

    switch (assignment.type) {
        case Assignment::ColonizePlanet: {
            const auto &planet = observer.get_planet(assignment.target_id);
            if (!planet.second) break;

            return navigator.dock_planet(ship.first, planet.first);
        }
        case Assignment::DefendPlanet:break;
        case Assignment::AttackPlanet: {
            const auto &planet = observer.get_planet(assignment.target_id);
            if(!planet.second) break;

            const auto &target = attack_planet(ship.first, planet.first);
            if (!target.second) break;
            return navigator.attack_ship(ship.first, target.first, observer.get_velocity(assignment.target_id));
        }
        case Assignment::DefendShip:break;
        case Assignment::AttackShip: {
            auto enemy = observer.get_ship(assignment.target_id);
            if (!enemy.second) break;
            return navigator.attack_ship(ship.first, enemy.first, observer.get_velocity(assignment.ship_id));
        }
    }

    return hlt::Move::noop();
}

hlt::nullable<hlt::Ship> bot::Commander::attack_planet(const hlt::Ship &ship, const hlt::Planet &planet) {
    std::vector<hlt::Ship> weakest;
    for (const auto &ship_id : planet.docked_ships) {
        const auto &enemy = observer.get_ship(planet.owner_id, ship_id);
        if (!enemy.second) continue;

        if (weakest.empty()) weakest.push_back(enemy.first);
        else if (weakest[0].health >= enemy.first.health) {
            if (weakest[0].health != enemy.first.health) weakest.clear();
            weakest.push_back(enemy.first);
        }
    }

    return closest_object(weakest, ship.pos);
}

bool bot::Commander::valid_assignment(bot::Assignment ass) {
    auto ship = observer.get_ship(observer.my_id, ass.ship_id);
    if (!ship.second || !ship.first.is_alive()) return false;

    if (ass.type == Assignment::ColonizePlanet) {
        const auto &planet = observer.get_planet(ass.target_id);
        if(!planet.second) return false;
        if (!planet.first.is_alive() || planet.first.owner_mask(observer.my_id) == hlt::enemy_mask) return false;
        return (!planet.first.is_full());
    } else if (ass.type == Assignment::AttackPlanet) {
        const auto &planet = observer.get_planet(ass.target_id);
        if(!planet.second) return false;
        return (planet.first.health > 0 && planet.first.owner_mask(observer.my_id) == hlt::enemy_mask);
    } else if (ass.type == Assignment::AttackShip) {
        const auto &ship = observer.get_ship(ass.target_id);
        return (ship.second || ship.first.health > 0);
    }

    return true;
}

hlt::nullable<bot::Assignment> bot::Commander::get_assignment(const hlt::EntityId &ship_id) {
    // TODO: assignement map instea
    for (const auto &ass : assignments) if (ass.ship_id == ship_id) return std::make_pair(ass, true);
    return std::make_pair(Assignment(0,0, Assignment::Type::NoOp), false); // TODO empty constructor
}

