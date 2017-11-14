//
// Created by egordm on 14-11-2017.
//

#include <algorithm>
#include <sorting.h>
#include <constants.h>
#include <utils.h>
#include <cmath>
#include "smart_commander.h"

using namespace std::placeholders;

namespace bot {
	namespace commanding {

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

		void SmarterCommander::assign() {
			auto attacked_planets = observer.get_planets();
			std::sort(attacked_planets.begin(), attacked_planets.end(), std::bind(sorting::sort_by_distance, home_base->pos, _1, _2));

			auto undocked_ships = observer.get_my_ships();
			utils::erase_if_not(undocked_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));

			// Do defend
			unsigned int defend_left = static_cast<unsigned int>(std::ceil(undocked_ships.size() * constants::DEFEND_RATIO));
			for(const auto &planet : attacked_planets) {
				const auto n_attackers = observer.get_attackers(planet.get()).size();
				const auto &nearby_ships = observer.get_ships(planet->pos, planet->radius + constants::DEFEND_SEARCH_RADIUS, hlt::friendly_mask);
				utils::erase_if_not(nearby_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
				std::sort(nearby_ships.begin(), nearby_ships.end(), std::bind(sorting::sort_by_distance, planet->pos, _1, _2));

				for(unsigned int i = 0; i < nearby_ships.size() && i < n_attackers + 1 && defend_left > 0; ++i) {
					// TODO: assign
					defend_left++;
				}
				if(defend_left <= 0) break;
			}

			Commander::assign();
		}

		void SmarterCommander::assign(const std::shared_ptr<hlt::Ship> &ship) {
			StrongerCommander::assign(ship);
		}
	};
}