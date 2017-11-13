//
// Created by egordm on 13-11-2017.
//10

#include "assignment.h"
#include "sorting.h"

namespace bot {
	hlt::Move ColonizeAssignment::move(const Observer &observer, navigation::Navigator *navigator) const {
		return navigator->dock_planet(get_ship(), get_target(observer));
	}

	hlt::Move AttackShipAssignment::move(const Observer &observer, navigation::Navigator *navigator) const {
		return navigator->attack_ship(get_ship(), get_target(observer));
	}

	const std::shared_ptr<hlt::Ship> &AttackPlanetAssignment::get_target(const Observer &observer) const {
		// Find the weakest enemy TODO: weigh health, status and distance
		std::vector<std::shared_ptr<hlt::Ship>> weakest;
		for (const auto &ship_id : get_target_planet()->docked_ships) {
			const auto &enemy = observer.get_ship(ship_id);

			if (weakest.empty()) weakest.push_back(enemy);
			else if (weakest[0]->health >= enemy->health) {
				if (weakest[0]->health != enemy->health) weakest.clear();
				weakest.push_back(enemy);
			}
		}

		return *std::min(weakest.begin(), weakest.end(), sorting::SortByDistance(get_ship()->pos));
	}
}