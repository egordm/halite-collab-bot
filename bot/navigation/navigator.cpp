//
// Created by egordm on 13-11-2017.
//

#include "../../hlt/navigation.hpp"
#include "navigator.h"
#include "../constants.h"
#include "navigation_fast.hpp"

namespace bot {
	namespace navigation {

		hlt::Move LegacyNavigator::dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) {
			if (ship->can_dock(planet.get())) {
				return hlt::Move::dock(ship->entity_id, planet->entity_id);
			}

			auto ret = hlt::navigation::navigate_ship_to_dock(observer.getMap(), ship.get(), planet.get(), hlt::constants::MAX_SPEED);
			return (ret.second) ? ret.first : hlt::Move::noop();
		}

		hlt::Move LegacyNavigator::attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target) {
			auto target_pos = ship->pos.closest_point(target->pos, target->radius + hlt::constants::WEAPON_RADIUS - 2);

			auto ret = hlt::navigation::navigate_ship_towards_target(observer.getMap(), ship.get(), target_pos, hlt::constants::MAX_SPEED,
			                                                         true, 100, 0.02);
			return ret.second ? ret.first : hlt::Move::noop();
		}

		hlt::Move FastNavigator::dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) {
			if (ship->can_dock(planet.get())) {
				return hlt::Move::dock(ship->entity_id, planet->entity_id);
			}

			auto target_pos = ship->pos.closest_point(planet->pos, planet->radius + 1);
			std::vector<hlt::EntityIdentifier> ignore_list {planet->identify()};
			return navigation::fast::navigate_towards(observer, ship.get(), target_pos, ignore_list);
		}

		hlt::Move FastNavigator::attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target) {
			auto target_pos = ship->pos.closest_point(target->pos, target->radius + hlt::constants::WEAPON_RADIUS/2);

			std::vector<hlt::EntityIdentifier> ignore_list {target->identify()};
			return navigation::fast::navigate_towards(observer, ship.get(), target_pos, ignore_list);
		}
	}
}