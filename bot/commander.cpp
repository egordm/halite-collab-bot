//
// Created by egordm on 9-11-2017.
//

#include "commander.h"
#include "utils.h"


bot::Commander::Commander(bot::Observer &observer, bot::Navigator &navigator) : observer(observer),
                                                                                navigator(navigator) {}

hlt::Move bot::Commander::command(const hlt::Ship &ship) {
    if (ship.docking_status != hlt::ShipDockingStatus::Undocked) return hlt::Move::noop();
    auto planet = closest_object(observer.get_planets(hlt::empty_mask), ship.pos);

    if (planet.second) return navigator.dock_planet(ship, planet.first);

/*    auto enemy = closest_object(observer.get_enemies(), ship.pos); //TODO: overflow ex
    if (enemy.second) return navigator.attack_ship(ship, enemy.first, observer.get_velocity(enemy.first.entity_id));*/

    return hlt::Move::noop();
}

hlt::Move bot::Commander::produce_move(const bot::Assignment &assignment) {
    auto ship = observer.get_ship(observer.my_id, assignment.ship_id);
    if (!ship.second) return hlt::Move::noop();

    switch (assignment.type) {
        case Assignment::ColonizePlanet:
            return navigator.dock_planet(ship.first, observer.get_planet(assignment.target_id));
        case Assignment::DefendPlanet:break;
        case Assignment::AttackPlanet:break;
        case Assignment::DefendShip:break;
        case Assignment::AttackShip:auto enemy = observer.get_ship(assignment.target_id);
            if (!enemy.second) break;
            return navigator.attack_ship(ship.first, enemy.first, observer.get_velocity(assignment.ship_id));
    }

    return hlt::Move::noop();
}
