//
// Created by egordm on 13-11-2017.
//10

#include <algorithm>
#include "assignment.h"
#include "sorting.h"

namespace bot {
	hlt::Move ColonizeAssignment::move(Observer &observer, navigation::Navigator *navigator) const {
		return navigator->dock_planet(get_ship(), get_target(observer));
	}

	hlt::Move AttackShipAssignment::move(Observer &observer, navigation::Navigator *navigator) const {
		return navigator->attack_ship(get_ship(), get_target(observer));
	}

	static bool comparator(const std::shared_ptr<hlt::Ship> &a, const std::shared_ptr<hlt::Ship> &b) {
		return true;
	}

	const std::shared_ptr<hlt::Ship> AttackPlanetAssignment::get_target(Observer &observer) const {
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

		sorting::SortByDistance distance_sort(get_ship()->pos);
		if(weakest.empty()) return nullptr;
		return *std::min_element(weakest.begin(), weakest.end(), distance_sort);
	}
}