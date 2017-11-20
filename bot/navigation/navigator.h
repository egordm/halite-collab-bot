//
// Created by egordm on 13-11-2017.
//

#ifndef HALITE_ORION_NAVIGATOR_H
#define HALITE_ORION_NAVIGATOR_H


#include "../../hlt/move.hpp"
#include "../observer.h"
#include "collision_avoidance.h"
#include <cmath>

namespace bot {
	namespace navigation {
		struct MovePromise {
			const hlt::EntityId ship_id;
			const hlt::MoveType type;
			hlt::EntityId dock_to;
			hlt::Vector velocity;

			MovePromise(const hlt::EntityId ship_id, const hlt::MoveType type, const hlt::EntityId dock_to, const hlt::Vector &velocity)
					: ship_id(ship_id), type(type), dock_to(dock_to), velocity(velocity) {}

			hlt::Move produce() const {
				switch (type) {
					case hlt::MoveType::Noop: return hlt::Move::noop();
					case hlt::MoveType::Thrust: {
						const double angle = std::round(hlt::rad_to_deg(velocity.angle()));
						const auto speed = std::min(7, static_cast<const int>(std::round(velocity.length())));
						return hlt::Move::thrust(ship_id, speed, static_cast<const int>(angle));
					}
					case hlt::MoveType::Dock: return hlt::Move::dock(ship_id, dock_to);
					case hlt::MoveType::Undock: return hlt::Move::undock(ship_id);
				}
			}

			static MovePromise noop() {
				return {0, hlt::MoveType::Noop, 0, hlt::Vector()};
			}

			static MovePromise dock(const hlt::EntityId ship_id, const hlt::EntityId dock_to) {
				return {ship_id, hlt::MoveType::Dock, dock_to, hlt::Vector()};
			}

			static MovePromise undock(const hlt::EntityId ship_id) {
				return {ship_id, hlt::MoveType::Undock, 0, hlt::Vector()};
			}

			static MovePromise thrust(const hlt::EntityId ship_id, const hlt::Vector &velocity) {
				return {ship_id, hlt::MoveType::Thrust, 0, velocity};
			}
		};

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

			void dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
				move_promises.push_back(promise_dock_planet(ship, planet));
			}

			void attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
				move_promises.push_back(promise_attack_ship(ship, target));
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
			navigation::NavigationCoordinator coordinator;

			MovePromise promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) override;

			MovePromise promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) override;

		public:
			explicit FastNavigator(Observer &observer) : Navigator(observer), coordinator(navigation::NavigationCoordinator(observer)) {}

			std::vector<hlt::Move> produce_moves() override;
		};
	}
}


#endif //HALITE_ORION_NAVIGATOR_H
