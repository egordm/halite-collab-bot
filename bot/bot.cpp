//
// Created by egordm on 9-11-2017.
//

#include <iostream>
#include "../hlt/log.hpp"
#include "bot.h"


namespace bot {
    Bot::Bot(hlt::PlayerId id, hlt::Map &map) : observer(Observer(id, map)), navigator(Navigator(observer)),
                                                      commander(Commander(observer, navigator)) {

    }

    std::vector<hlt::Move> Bot::do_step(const hlt::Map &map) {
        observer.observe(map);
        try {
            return commander.command();
        } catch (const std::exception &exc) {
            hlt::Log::log(exc.what());
            std::cerr << exc.what();
            throw exc;
        }
    }


}