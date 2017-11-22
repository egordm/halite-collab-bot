//
// Created by egordm on 13-11-2017.
//

#include "../../hlt/navigation.hpp"
#include "navigator.h"

namespace bot {
    namespace navigation {
        MovePromise LegacyNavigator::promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
            if (ship->can_dock(planet)) {
                return MovePromise::dock(ship->entity_id, planet->entity_id);
            }

            auto ret = hlt::navigation::navigate_ship_to_dock(observer.getMap(), ship, planet,
                                                              hlt::constants::MAX_SPEED);
            return (ret.second) ? MovePromise::thrust(ship->entity_id, ret.first.get_velocity(), planet->pos)
                                : MovePromise::noop();
        }

        MovePromise LegacyNavigator::promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
            auto target_pos = ship->pos.closest_point(target->pos, target->radius + hlt::constants::WEAPON_RADIUS - 2);
            auto ret = hlt::navigation::navigate_ship_towards_target(observer.getMap(), ship, target_pos,
                                                                     hlt::constants::MAX_SPEED,
                                                                     true, 100, 0.02);

            return ret.second ? MovePromise::thrust(ship->entity_id, ret.first.get_velocity(), target_pos)
                              : MovePromise::noop();
        }

        MovePromise FastNavigator::promise_dock_planet(const hlt::Ship *ship, const hlt::Planet *planet) {
            if (ship->can_dock(planet)) {
                return MovePromise::dock(ship->entity_id, planet->entity_id);
            }

            auto target_pos = ship->pos.closest_point(planet->pos,
                                                      ship->radius + planet->radius + hlt::constants::DOCK_RADIUS - 1);
            std::vector<hlt::EntityIdentifier> ignore_list{planet->identify()};

            return navigation::FastPath(observer, ignore_list, 0).navigate(ship, target_pos);
        }

        MovePromise FastNavigator::promise_attack_ship(const hlt::Ship *ship, const hlt::Ship *target) {
            auto target_pos = ship->pos.closest_point(target->pos,
                                                      ship->radius + target->radius + hlt::constants::WEAPON_RADIUS -
                                                      1);

            std::vector<hlt::EntityIdentifier> ignore_list{target->identify()};
            return navigation::FastPath(observer, ignore_list, hlt::constants::MAX_SPEED).navigate(ship, target_pos);
        }

        std::vector<hlt::Move> FastNavigator::produce_moves() {
            for (const auto &ship : observer.get_my_ships()) { //TODO: ship correction
                ship->vel = hlt::Vector();
            }


            for (const auto &move_promise : move_promises)
                observer.get_ship(move_promise.ship_id)->vel = move_promise.velocity;
            corrector.correct_moves(move_promises);

            std::vector<hlt::Move> ret;
            for (const auto &move_promise : move_promises) ret.push_back(move_promise.produce());

            move_promises.clear();
            return ret;
        }
    }
}