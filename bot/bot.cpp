//
// Created by egordm on 9-11-2017.
//

#include <iostream>
#include <move.hpp>
#include "../hlt/log.hpp"
#include "bot.h"


namespace bot {
    Bot::Bot(hlt::PlayerId id, hlt::Map *map)
            : observer(Observer(id, map)), commander(new JuniorCommander(observer, new navigation::LegacyNavigator(observer))) {

    }

    std::vector<hlt::Move> Bot::do_step() {
        observer.observe();
        try {
            return commander->command();
        } catch (const std::exception &exc) {
            hlt::Log::log(exc.what());
            std::cerr << exc.what();
            throw exc;
        }
    }

	Bot::~Bot() {
		delete commander;
	}
}