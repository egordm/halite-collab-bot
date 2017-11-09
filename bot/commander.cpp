//
// Created by egordm on 9-11-2017.
//

#include "commander.h"

bot::Commander::Commander(const bot::Observer &observer, const bot::Navigator &navigator) : observer(observer),
                                                                                            navigator(navigator) {}

hlt::Move bot::Commander::command(const hlt::Ship &ship) {
    if(ship.docking_status != hlt::ShipDockingStatus::Undocked) return hlt::Move::noop();
    auto target = observer.closest_planet(ship.pos, bot::empty_mask);

    if(target.second) return navigator.dock_planet(ship, target.first);

    return hlt::Move::noop();
}