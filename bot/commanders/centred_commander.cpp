//
// Created by egordm on 28-11-2017.
//

#include <commanders/behaviours/defend_behaviour.h>
#include <commanders/behaviours/attack_behaviour.h>
#include "centred_commander.h"


namespace bot { namespace commanding {
	CentredCommander::CentredCommander(bot::Observer &observer, bot::navigation::Navigator *navigator) : Commander(observer, navigator) {
		behaviours.push_back(new behaviours::DefendBehaviour(this));
		behaviours.push_back(new behaviours::AttackBehaviour(this));
	}

	void CentredCommander::clean() {
		Commander::clean();
		if (home_base != nullptr && (home_base->owner_mask(observer.my_id) != hlt::friendly_mask || !home_base->is_alive())) {
			home_base = nullptr;
		}
		// Pick the base planet
		// TODO: mb use average center
		if (home_base == nullptr) {
			const auto &my_planets = observer.get_planets(hlt::friendly_mask);
			if (!my_planets.empty()) {
				home_base = *std::max_element(my_planets.begin(), my_planets.end(), sorting::filter_by_size);
			}
		}
	}

	std::vector<hlt::Move> CentredCommander::produce_moves() {
		// TODO: move this to commander & navigator
		for (const auto &ship : observer.get_my_ships()) { //TODO: ship correction
			ship->vel = hlt::Vector();
		}

		for (const auto &kv : assignments) {
			auto promise = kv.second->produce_move(navigator);
			if (promise.type == hlt::MoveType::Thrust) observer.get_ship(promise.ship_id)->vel = promise.velocity;
		}

		return navigator->produce_moves();
	}
}}