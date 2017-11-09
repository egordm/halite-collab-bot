//
// Created by egordm on 9-11-2017.
//

#include "bot.h"


namespace bot {
    Bot::Bot(hlt::PlayerId id, const hlt::Map &map) : observer(Observer(id, map)), navigator(Navigator(observer)){

    }

    std::vector<hlt::Move> Bot::do_step(const hlt::Map &map) {
        observer.observe(map);

        return std::vector<hlt::Move>();
    }


}