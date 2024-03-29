//
// Created by egordm on 9-11-2017.
//

#include <algorithm>
#include <navigation/collision_checking.hpp>
#include "observer.h"
#include "sorting.h"
#include "../hlt/hlt_in.hpp"
#include "constants.h"
#include "utils.h"
#include "math.hpp"

using namespace std::placeholders;

namespace bot {
	Observer::Observer(hlt::PlayerId id, hlt::Map *map) : map(map), my_id(id) {}

	void Observer::observe() {

		hlt::in::update_map(map);

		ships.clear();
		my_ships.clear();
		enemy_ships.clear();

		for (const auto &player : map->players) {
			std::copy(player.second.begin(), player.second.end(), std::back_inserter(ships));

			if (player.first == my_id) std::copy(player.second.begin(), player.second.end(), std::back_inserter(my_ships));
			else std::copy(player.second.begin(), player.second.end(), std::back_inserter(enemy_ships));
		}

		planets.clear();
		for (const auto &kv : map->planets) planets.push_back(kv.second);

		step++;
	}

	const hlt::Map *Observer::getMap() {
		return map;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_my_ships() {
		return my_ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_enemies() {
		return enemy_ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_ships() {
		return ships;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_ships(const hlt::Vector &p, double radius, unsigned short owner_mask) {
		std::vector<std::shared_ptr<hlt::Ship>> ret = ships;
		utils::erase_if_not(ret, std::bind(sorting::filter_by_owner_mask, my_id, owner_mask, _1));
		utils::erase_if_not(ret, std::bind(sorting::filter_by_distance, p, radius, _1));

		return ret;
	}

	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_ships(hlt::ShipDockingStatus status) {
		auto undocked_ships = get_my_ships();
		if (undocked_ships.empty()) return std::vector<std::shared_ptr<hlt::Ship>>();
		utils::erase_if_not(undocked_ships, std::bind(sorting::filter_by_status, status, _1));
		return undocked_ships;
	}

	std::shared_ptr<hlt::Ship> Observer::get_ship(hlt::EntityId ship_id) {
		if (map->ships.find(ship_id) == map->ships.end()) return nullptr;
		return map->ships.at(ship_id);
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_planets() {
		return planets;
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_planets(unsigned short owner_mask) {
		std::vector<std::shared_ptr<hlt::Planet>> ret;
		std::copy_if(planets.begin(), planets.end(), std::back_inserter(ret), sorting::FilterOwnerMask(my_id, owner_mask));
		return ret;
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_planets(const hlt::Vector &p, double radius, unsigned short owner_mask) {
		std::vector<std::shared_ptr<hlt::Planet>> ret = planets;
		utils::erase_if_not(ret, std::bind(sorting::filter_by_owner_mask, my_id, owner_mask, _1));
		utils::erase_if_not(ret, std::bind(sorting::filter_by_distance, p, radius, _1));
		return ret;
	}

	std::shared_ptr<hlt::Planet> Observer::get_planet(hlt::EntityId planet_id) {
		if (map->planets.find(planet_id) == map->planets.end()) return nullptr;
		return map->planets.at(planet_id);
	}

	/**
	 * Definition of attacked: enemy ships is undocked and within defend range
	 * @return
	 */
	std::vector<std::shared_ptr<hlt::Ship>> Observer::get_attackers(hlt::Planet *planet) {
		sorting::FilterByStatus status_filter(hlt::ShipDockingStatus::Undocked);

		auto enemies = get_ships(planet->pos, planet->radius + constants::DEFEND_RADIUS, hlt::enemy_mask);
		std::vector<std::shared_ptr<hlt::Ship>> ret;

		std::copy_if(enemies.begin(), enemies.end(), std::back_inserter(ret),
		             std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));

		// Check if moving in direction of
		enemies = get_ships(planet->pos, planet->radius + constants::DEFEND_SEARCH_RADIUS_DIRECTIONAL, hlt::enemy_mask);
		utils::erase_if_not(enemies, std::bind(sorting::filter_by_status, hlt::ShipDockingStatus::Undocked, _1));

		for(const auto &enemy : enemies) {
			if(std::find(ret.begin(), ret.end(), enemy) != ret.end()) continue;
			auto target_point = math::resize_line(enemy->pos, enemy->pos + enemy->vel, constants::DEFEND_SEARCH_RADIUS_DIRECTIONAL);
			std::vector <hlt::EntityIdentifier> ignore;

			std::vector<std::pair<hlt::Entity *, hlt::Vector>> obstacles;
			navigation::check_collisions(planets, enemy->pos, target_point, ignore, obstacles);
			if(obstacles.size() != 1) continue;
			if(obstacles.begin()->first->is_same(planet)) {
				ret.push_back(enemy);
			}
		}

		return ret;
	}

	std::vector<std::shared_ptr<hlt::Planet>> Observer::get_attacked_planets() {
		std::vector<std::shared_ptr<hlt::Planet>> ret;

		const auto &planets = get_planets(hlt::friendly_mask);
		std::copy_if(planets.begin(), planets.end(), std::back_inserter(ret), [this](const std::shared_ptr<hlt::Planet> &el) {
			return !get_attackers(el.get()).empty();
		});

		return ret;
	}

	Observer::~Observer() {
		delete map;
	}

	double Observer::get_production_share(hlt::PlayerId player_id) {
		double shares[map->players.size()]; // TODO: finish
		double sum = 0;
		for (const auto &planet  : get_planets()) {
			if (!planet->owned) continue;
			shares[planet->owner_id] += planet->docked_ships.size();
			sum += planet->docked_ships.size();
		}
		return shares[player_id] / sum;
	}

	double Observer::get_ship_share(hlt::PlayerId player_id) {
		double sum = 0;
		for(const auto &player : map->players) {
			sum += player.second.size();
		}
		return map->players[player_id].size() / sum;
	}
}