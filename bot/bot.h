//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_BOT_H
#define MYBOT_BOT_H

#include "../hlt/map.hpp"
#include "observer.h"
#include "navigator.h"
#include "commander.h"

namespace bot {
    class Bot {
    private:
        Observer observer;
        Commander *commander;
        unsigned int step;
    public:
        Bot(hlt::PlayerId id, hlt::Map *map);

        virtual ~Bot();

        std::vector<hlt::Move> do_step();
    };
}

#endif //MYBOT_BOT_H
