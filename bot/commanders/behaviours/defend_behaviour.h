//
// Created by egordm on 27-11-2017.
//

#ifndef HALITE_ORION_DEFEND_BEHAVIOUR_H
#define HALITE_ORION_DEFEND_BEHAVIOUR_H

#include "base_bahaviours.hpp"

namespace bot { namespace commanding { namespace behaviours {
	class DefendBehaviour : public CentredBehaviour {
	protected:
		/**
		 * @return Amout of ships that should help defend
		 */
		int get_defend_count() const {
			auto undocked_ships = commander->get_observer().get_my_ships();
			if (undocked_ships.empty()) return 0;
			utils::erase_if_not(undocked_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
			return static_cast<unsigned int>(std::ceil(undocked_ships.size() * constants::DEFEND_RATIO));
		}

	public:
		explicit DefendBehaviour(CentredCommander *commander) : CentredBehaviour(commander) {}

		void assign() const override {
			auto attacked_planets = commander->get_observer().get_attacked_planets();
			if (attacked_planets.empty()) return;
			std::sort(attacked_planets.begin(), attacked_planets.end(), std::bind(sorting::sort_by_distance, get_home_base(), _1, _2));

			// Do defend
			int to_defend = get_defend_count();
			if (to_defend == 0) return;

			for (const auto &planet : attacked_planets) {
				const auto attackers = commander->get_observer().get_attackers(planet.get());
				const int max_defenders = std::max(1, static_cast<const int &>(attackers.size() * constants::DEFENDERS_PER_ATTACKER));

				auto nearby_ships = commander->get_observer().get_ships(planet->pos, planet->radius + constants::DEFEND_SEARCH_RADIUS,
				                                                        hlt::friendly_mask);
				if (nearby_ships.empty()) continue;

				// Remove not undocked and sort by distance
				utils::erase_if_not(nearby_ships, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));
				std::sort(nearby_ships.begin(), nearby_ships.end(), std::bind(sorting::sort_by_distance, planet->pos, _1, _2));

				for (unsigned int i = 0; i < nearby_ships.size() && i < max_defenders && to_defend > 0; ++i) {
					commander->assign(std::make_shared<DefendPlanetAssignment>(nearby_ships[i], planet));
					to_defend--;
				}
				if (to_defend <= 0) break;
			}
		}
	};
}}}

#endif //HALITE_ORION_DEFEND_BEHAVIOUR_H
