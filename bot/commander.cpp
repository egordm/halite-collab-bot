//
// Created by egordm on 9-11-2017.
//

#include "commander.h"
#include <sstream>
#include <log.hpp>


bot::Commander::Commander(bot::Observer &observer, bot::Navigator &navigator) : observer(observer),
                                                                                            navigator(navigator) {}

hlt::Move bot::Commander::command(const hlt::Ship &ship) {
    if(ship.docking_status != hlt::ShipDockingStatus::Undocked) return hlt::Move::noop();
    auto target = observer.closest_planet(ship.pos, hlt::empty_mask);

    if(target.second) return navigator.dock_planet(ship, target.first);

    return hlt::Move::noop();
}