//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_NAVIGATOR_H
#define MYBOT_NAVIGATOR_H

#include <move.hpp>
#include "observer.h"

namespace bot {
    class Navigator {
    private:
        Observer observer;
    public:
        explicit Navigator(const Observer &observer);

        hlt::nullable<hlt::Planet> planet_between(const hlt::Vector &a, const hlt::Vector &b);

        hlt::Move move_towards(const hlt::Ship &ship, const hlt::Vector &pos, bool avoid_obstacles);

        hlt::Move dock_planet(const hlt::Ship &ship, const hlt::Planet &planet);

        hlt::Move attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel);
    };
}

#endif //MYBOT_NAVIGATOR_H
