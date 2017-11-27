//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_NAVIGATOR_H
#define HALITE_ORION_NAVIGATOR_H


#include "../../hlt/move.hpp"
#include "../observer.h"
#include "navigation_fast.hpp"
#include <cmath>

namespace bot {
	namespace navigation {
		class Navigator {
		protected:
			Observer &observer;
			std::vector<MovePromise> move_promises;

			virtual MovePromise promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) = 0;

			virtual MovePromise promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) = 0;

		public:
			explicit Navigator(Observer &observer) : observer(observer) {}

			virtual std::vector<hlt::Move> produce_moves() {
				std::vector<hlt::Move> ret;
				for (const auto &move_promise : move_promises) ret.push_back(move_promise.produce());
				move_promises.clear();
				return ret;
			}

			MovePromise dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
				auto promise = promise_dock_planet(ship, planet);
				move_promises.push_back(promise);
				return promise;
			}

			MovePromise attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
				auto promise = promise_attack_ship(ship, target);
				move_promises.push_back(promise);
				return promise;
			}

			Observer &get_observer() const {
				return observer;
			}
		};

		class LegacyNavigator : public Navigator {
		protected:
			MovePromise promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) override;

			MovePromise promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) override;

		public:
			explicit LegacyNavigator(Observer &observer) : Navigator(observer) {}
		};

		class FastNavigator : public Navigator {
		protected:
			MovePromise promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) override;

			MovePromise promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) override;

		public:
			explicit FastNavigator(Observer &observer) : Navigator(observer) {}

			std::vector<hlt::Move> produce_moves() override;
		};
	}
}


#endif //HALITE_ORION_NAVIGATOR_H
