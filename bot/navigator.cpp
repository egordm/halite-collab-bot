//
// Created by egordm on 13-11-2017.
//

#include <navigation.hpp>
#include "navigator.h"

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
			auto taget_pos = ship->pos.closest_point(target->pos, target->radius + hlt::constants::WEAPON_RADIUS - 2);
			auto ret = hlt::navigation::navigate_ship_towards_target(observer.getMap(), ship.get(), taget_pos, hlt::constants::MAX_SPEED,
			                                                         true, 100, 0.02);
			return ret.second ? ret.first : hlt::Move::noop();
		}
	}
}