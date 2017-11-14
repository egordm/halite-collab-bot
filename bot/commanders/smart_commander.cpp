//
// Created by egordm on 14-11-2017.
//

#include <algorithm>
#include <sorting.h>
#include "smart_commander.h"

namespace bot {
	namespace commanding {

		void SmarterCommander::assign() {
			Commander::assign();
		}

		void SmarterCommander::clean() {
			Commander::clean();
			if(home_base != nullptr && (home_base->owner_mask(observer.my_id) != hlt::friendly_mask || !home_base->is_alive())) {
				home_base = nullptr;
			}
			// Pick the base planet
			// TODO: mb use average center
			if(home_base == nullptr) {
				const auto &my_planets = observer.get_planets(hlt::friendly_mask);
				if(!my_planets.empty()) {
					home_base = *std::max_element(my_planets.begin(), my_planets.end(), sorting::filter_by_size);
				}
			}
		}

		void SmarterCommander::assign(const std::shared_ptr<hlt::Ship> &ship) {
			StrongerCommander::assign(ship);
		}
	};
}