//
// Created by egordm on 8-11-2017.
//

#ifndef MYBOT_NAVIGATION_H
#define MYBOT_NAVIGATION_H

#include <move.hpp>
#include <entities.hpp>
#include <map.hpp>
#include <algorithm>

namespace bot {
    namespace navigation {
        hlt::nullable<hlt::Planet> planet_between(const hlt::Map &map, const hlt::Vector &a, const hlt::Vector &b);

        hlt::Move
        move_towards(const hlt::Map &map, const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles);

        hlt::Move dock_planet(const hlt::Map &map, const hlt::Ship &ship, const hlt::Planet &planet);
    }
}


#endif //MYBOT_NAVIGATION_H
