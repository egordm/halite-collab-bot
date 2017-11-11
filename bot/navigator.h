//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_NAVIGATOR_H
#define MYBOT_NAVIGATOR_H

#include "../hlt/move.hpp"
#include "observer.h"


namespace bot {


    class Navigator {
    protected:
        Observer &observer;
    public:
        explicit Navigator(Observer &observer);

        virtual hlt::Move dock_planet(const hlt::Ship &ship, const hlt::Planet &planet);

        virtual hlt::Move attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel);
    };

    class LegacyNavigator : public Navigator{
    public:
        explicit LegacyNavigator(Observer &observer);

        hlt::Move dock_planet(const hlt::Ship &ship, const hlt::Planet &planet) override;

        hlt::Move attack_ship(const hlt::Ship &ship, const hlt::Ship &target, const hlt::Vector &target_vel) override;
    };
}

#endif //MYBOT_NAVIGATOR_H
