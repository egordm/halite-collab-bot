//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_BOT_H
#define MYBOT_BOT_H

#include <types.hpp>
#include <map.hpp>
#include "observer.h"
#include "navigator.h"

namespace bot {
    class Bot {
    private:
        Observer observer;
        Navigator navigator;
        unsigned int step;
    public:
        Bot(hlt::PlayerId id, const hlt::Map &map);

        std::vector<hlt::Move> do_step(const hlt::Map &map);
    };
}

#endif //MYBOT_BOT_H
