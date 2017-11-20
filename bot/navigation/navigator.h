//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_NAVIGATOR_H
#define HALITE_ORION_NAVIGATOR_H


#include "../../hlt/move.hpp"
#include "../observer.h"
#include <cmath>

namespace bot {
	namespace navigation {
		class Navigator {
		protected:
			Observer &observer;
		public:
			explicit Navigator(Observer &observer) : observer(observer) {}

			virtual hlt::Move
			dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) { return hlt::Move::noop(); }

			virtual hlt::Move
			attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target) { return hlt::Move::noop(); }
		};

		class LegacyNavigator : public Navigator {
		public:
			explicit LegacyNavigator(Observer &observer) : Navigator(observer) {}

			hlt::Move dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) override;

			hlt::Move attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target) override;

		};

		class FastNavigator : public Navigator {
		public:
			explicit FastNavigator(Observer &observer) : Navigator(observer) {}

			hlt::Move dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) override;

			hlt::Move attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target) override;
		};

		static hlt::Move create_thrust(const hlt::EntityId entity_id, hlt::Vector velocity) {
			const double angle = std::round(hlt::rad_to_deg(velocity.angle()));
			const unsigned int speed = static_cast<const unsigned int>(std::min(7, static_cast<const int>(std::round(velocity.length()))));
			return hlt::Move::thrust(entity_id, speed, static_cast<const int>(angle));
		}
	}
}


#endif //HALITE_ORION_NAVIGATOR_H
