//
// Created by egordm on 13-11-2017.
//10

#include <algorithm>
#include <cmath>
#include "assignment.h"
#include "sorting.h"
#include "constants.h"

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
		if (weakest.empty()) return nullptr;
		return *std::min_element(weakest.begin(), weakest.end(), distance_sort);
	}

	unsigned int AttackPlanetAssignment::max_count(Observer &observer) const {
		return 9001;
		// TODO: revisit
		auto ndocked = get_target_planet()->docked_ships.size();
		auto nproduction = ndocked * hlt::constants::PER_SHIP_PRODUCTION * constants::ATTACK_PLANET_ASSIGNMENT_MULTIPLIER;
		auto nenemies = observer.get_ships(get_target_planet()->pos, get_target_planet()->radius + constants::ATTACK_PLANET_CHECK_RADIUS,
		                                   hlt::enemy_mask).size();

		return static_cast<unsigned int>(std::ceil(ndocked + nproduction + nenemies));
	}

	const std::shared_ptr<hlt::Ship> DefendPlanetAssignment::get_target(Observer &observer) const {
		return AttackPlanetAssignment::get_target(observer);// TODO
	}
}