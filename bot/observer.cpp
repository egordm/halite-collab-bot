//
// Created by egordm on 9-11-2017.
//

#include <algorithm>
#include "observer.h"
#include "sorting.h"
#include "../hlt/hlt_in.hpp"
#include "constants.h"


namespace bot {
	Observer::Observer(hlt::PlayerId id, const hlt::Map &map) : map(map), my_id(id) {}

	void Observer::observe() {
		hlt::in::update_map(map);

		ships.clear();
		my_ships.clear();
		enemy_ships.clear();
		for (const auto &player : map.players) {
			std::copy(player.second.begin(), player.second.end(), std::back_inserter(ships));

			if (player.first == my_id) std::copy(player.second.begin(), player.second.end(), std::back_inserter(my_ships));
			else std::copy(player.second.begin(), player.second.end(), std::back_inserter(enemy_ships));
		}

		planets.clear();
		for (const auto &kv : map.planets) planets.push_back(kv.second);

		step++;
	}

	const hlt::Map &Observer::getMap() const {
		return map;
	}

	std::vector<std::shared_ptr<hlt::Ship>> &Observer::get_my_ships() const {
		return my_ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> &Observer::get_enemies() const {
		return enemy_ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> &Observer::get_ships() const {
		return ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_ships(const hlt::Vector &p, double radius, unsigned short owner_mask) const {
		sorting::FilterOwnerMask owner_filter(my_id, owner_mask);
		sorting::FilterByDistance distance_filter(p, radius);
		std::vector<std::shared_ptr<hlt::Ship>> ret;

		std::copy_if(ships.begin(), ships.end(), std::back_inserter(ret), [](const std::shared_ptr<hlt::Ship> &el) {
			return owner_filter(el) && distance_filter(el);
		});

		return ret;
	}

	std::shared_ptr<hlt::Ship> Observer::get_ship(hlt::EntityId ship_id) const {
		if (map.ships.find(ship_id) == map.ships.end()) return nullptr;
		return map.ships.at(ship_id);
	}

	std::vector<std::shared_ptr<hlt::Planet>> &Observer::get_planets() const {
		return planets;
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_planets(unsigned short owner_mask) const {
		std::vector<std::shared_ptr<hlt::Planet>> ret;
		std::copy_if(planets.begin(), planets.end(), std::back_inserter(ret), sorting::FilterOwnerMask(my_id, owner_mask));
		return ret;
	}

	std::shared_ptr<hlt::Planet> Observer::get_planet(hlt::EntityId planet_id) {
		if (map.planets.find(planet_id) == map.planets.end()) return nullptr;
		return map.planets.at(planet_id);
	}


	/**
	 * Definition of attacked: enemy ships is undocked and within defend range
	 * @return
	 */
	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_attackers(hlt::Planet *planet) const {
		sorting::FilterByDistance distance_filter(planet->pos, planet->radius + constants::DEFEND_RADIUS);
		sorting::FilterByStatus status_filter(hlt::ShipDockingStatus::Undocked);

		std::vector<std::shared_ptr<hlt::Ship>> ret;

		std::copy_if(enemy_ships.begin(), enemy_ships.end(), std::back_inserter(ret), [](const std::shared_ptr<hlt::Ship> &el) {
			return distance_filter(el) && status_filter(el);
		});

		return ret;
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_attacked_planets() const {
		std::vector<std::shared_ptr<hlt::Planet>> ret;

		const auto &planets = get_planets(hlt::friendly_mask);
		std::copy_if(planets.begin(), planets.end(), std::back_inserter(ret), [](const std::shared_ptr<hlt::Planet> &el) {
			return !get_attackers(el.get()).empty();
		});

		return planets;
	}
}