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

        hlt::Move produce_move(const bot::Assignment &assignment);
    };
}


#endif //MYBOT_COMMANDER_H
