//
// Created by egordm on 9-11-2017.
//

#include <iostream>
#include <commanders/centred_commander.h>
#include "bot.h"


namespace bot {
	Bot::Bot(hlt::PlayerId id, hlt::Map *map)
			: observer(Observer(id, map)),
			  commander(new commanding::CentredCommander(observer, new navigation::FastNavigator(observer))) {

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