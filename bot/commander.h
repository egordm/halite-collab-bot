//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_COMMANDER_H
#define MYBOT_COMMANDER_H

#include "navigator.h"
#include "observer.h"
#include "assignment.h"

namespace bot {
    class Commander {
    private:
        Observer &observer;
        Navigator &navigator;

        std::vector<bot::Assignment> assignments;
    public:
        Commander(Observer &observer, Navigator &navigator);

        hlt::Move command(const hlt::Ship &ship);

        std::vector<hlt::Move> command();

        hlt::Move produce_move(const bot::Assignment &assignment);

        hlt::nullable<hlt::Ship> attack_planet(const hlt::Ship &ship, const hlt::Planet &planet);
    };
}


#endif //MYBOT_COMMANDER_H
