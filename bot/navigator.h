//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_NAVIGATOR_H
#define HALITE_ORION_NAVIGATOR_H


#include <move.hpp>
#include "observer.h"

namespace bot {
	namespace navigation {
		class Navigator {
		protected:
			Observer &observer;
		public:
			explicit Navigator(Observer &observer) : observer(observer) {}

			virtual ~Navigator() = default;

			virtual hlt::Move
			dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) { return hlt::Move::noop(); }

			virtual hlt::Move attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target,
			                              const hlt::Vector &target_vel) { return hlt::Move::noop(); }
		};

		class LegacyNavigator : public Navigator {
		public:
			explicit LegacyNavigator(Observer &observer) : Navigator(observer) {}

			hlt::Move dock_planet(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Planet> &planet) override;

			hlt::Move attack_ship(const std::shared_ptr<hlt::Ship> &ship, const std::shared_ptr<hlt::Ship> &target,
			                      const hlt::Vector &target_vel) override;

		};
	}
}


#endif //HALITE_ORION_NAVIGATOR_H
