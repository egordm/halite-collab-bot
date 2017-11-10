//
// Created by egordm on 9-11-2017.
//

#include "bot.h"


namespace bot {
    Bot::Bot(hlt::PlayerId id, hlt::Map &map) : observer(Observer(id, map)), navigator(Navigator(observer)),
                                                      commander(Commander(observer, navigator)) {

    }

    std::vector<hlt::Move> Bot::do_step(const hlt::Map &map) {
        observer.observe(map);

        std::vector<hlt::Move> ret;
        for (const auto &ship : observer.my_ships()) {
            ret.push_back(commander.command(ship));
        }

        return ret;
    }


}