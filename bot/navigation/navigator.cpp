//
// Created by egordm on 13-11-2017.
//

#include "../../hlt/navigation.hpp"
#include "navigator.h"
#include "../constants.h"
#include "navigation_fast.hpp"

namespace bot {
	namespace navigation {
		MovePromise LegacyNavigator::promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
			if (ship->can_dock(planet)) {
				return MovePromise::dock(ship->entity_id, planet->entity_id);
			}

			auto ret = hlt::navigation::navigate_ship_to_dock(observer.getMap(), ship, planet, hlt::constants::MAX_SPEED);
			return (ret.second) ? MovePromise::thrust(ship->entity_id, ret.first.get_velocity(), planet->pos) : MovePromise::noop();
		}

		MovePromise LegacyNavigator::promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
			auto target_pos = ship->pos.closest_point(target->pos, target->radius + hlt::constants::WEAPON_RADIUS - 2);
			auto ret = hlt::navigation::navigate_ship_towards_target(observer.getMap(), ship, target_pos, hlt::constants::MAX_SPEED,
			                                                         true, 100, 0.02);

			return ret.second ? MovePromise::thrust(ship->entity_id, ret.first.get_velocity(), target_pos) : MovePromise::noop();
		}

		MovePromise FastNavigator::promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
			if (ship->can_dock(planet)) {
				return MovePromise::dock(ship->entity_id, planet->entity_id);
			}

			auto target_pos = ship->pos.closest_point(planet->pos, ship->radius + planet->radius+ hlt::constants::DOCK_RADIUS - 1);
			std::vector<hlt::EntityIdentifier> ignore_list {planet->identify()};

			return navigation::fast::Path(observer, ignore_list).navigate(ship, target_pos);
		}

		MovePromise FastNavigator::promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
			auto target_pos = ship->pos.closest_point(target->pos, ship->radius + target->radius + hlt::constants::WEAPON_RADIUS - 1);

			std::vector<hlt::EntityIdentifier> ignore_list {target->identify()};
			return navigation::fast::Path(observer, ignore_list).navigate(ship, target_pos);
		}

		std::vector<hlt::Move> FastNavigator::produce_moves() {
			for (const auto &ship : observer.get_my_ships()) {
				ship->vel = hlt::Vector();
			}

			std::vector<hlt::Move> ret;
			hlt::entity_map<MovePromise> move_map;
			for(const auto &move_promise : move_promises) {
				// TODO: threat noop as displaceable
				if(move_promise.type == hlt::MoveType::Thrust) {
					observer.get_ship(move_promise.ship_id)->vel = move_promise.velocity;
					move_map.emplace(move_promise.ship_id, move_promise);
				} else {
					observer.get_ship(move_promise.ship_id)->docking_status = hlt::ShipDockingStatus::Docked;
					ret.push_back(move_promise.produce());
				}
			}

			for (const auto &kv : move_map) {
				auto promise = kv.second;
				ret.push_back(promise.produce());
			}

			move_promises.clear();
			return ret;
		}
	}
}