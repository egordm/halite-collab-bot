//
// Created by egordm on 28-11-2017.
//

#ifndef HALITE_ORION_DISCOVERY_BEHAVIOUR_HPP
#define HALITE_ORION_DISCOVERY_BEHAVIOUR_HPP

#include <commanders/centred_commander.h>
#include "base_bahaviours.hpp"

namespace bot { namespace commanding { namespace behaviours {
	class DiscoveryBehaviour : public CentredBehaviour {
	public:
		explicit DiscoveryBehaviour(CentredCommander *commander) : CentredBehaviour(commander) {}

		void assign() const override {
			auto empty_planets = commander->get_observer().get_planets(hlt::empty_mask);
			if (empty_planets.empty()) return;
			std::sort(empty_planets.begin(), empty_planets.end(), std::bind(sorting::sort_by_distance, get_home_base(), _1, _2));

			auto undocked_ships = commander->get_observer().get_ships(hlt::ShipDockingStatus::Undocked);
			if (undocked_ships.empty()) return;

			// Do defend
			auto to_discover = static_cast<unsigned int>(std::ceil(undocked_ships.size() * constants::DISCOVERY_RATIO));
			for (const auto &planet : empty_planets) {
				const auto max_colonizers = planet->spots_left() / 2;

				auto nearby_ships = commander->get_observer().get_ships(planet->pos, planet->radius + constants::DISCOVERY_SEARCH_RADIUS,
				                                       hlt::friendly_mask);
				if (nearby_ships.empty()) continue;

				utils::erase_if_not(nearby_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
				std::sort(nearby_ships.begin(), nearby_ships.end(), std::bind(sorting::sort_by_distance, planet->pos, _1, _2));

				for (unsigned int i = 0; i < nearby_ships.size() && i < max_colonizers && to_discover > 0; ++i) {
					commander->assign(std::make_shared<ColonizeAssignment>(nearby_ships[i], planet));
					to_discover--;
				}
				if (to_discover <= 0) break;
			}
		}
	};
}}}

#endif //HALITE_ORION_DISCOVERY_BEHAVIOUR_HPP
