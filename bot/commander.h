//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_COMMANDER_H
#define MYBOT_COMMANDER_H

#include "navigator.h"
#include "observer.h"

namespace bot {
    class Commander {
    private:
        Observer observer;
        Navigator navigator;
    public:
        Commander(const Observer &observer, const Navigator &navigator);

        hlt::Move command(const hlt::Ship &ship);
    };
}


#endif //MYBOT_COMMANDER_H
