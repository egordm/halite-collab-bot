//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_HPP_H
#define HALITE_ORION_NAVIGATION_HPP_H

#include <sstream>
#include <algorithm>
#include <math.hpp>
#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include "../utils.h"
#include "../sorting.h"
#include "collision_checking.hpp"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		namespace fast {
			constexpr double BYPASS_MARGIN = 0.1;

			class Path {
			protected:
				Observer &observer;
				std::vector<hlt::EntityIdentifier> &ignore_list;

				hlt::nullable<hlt::Vector>
				correct_route(const hlt::Vector &a, const hlt::Vector &b, bool correct_left, unsigned int max_corrections) {
					if (max_corrections == 0) return {b, false};

					std::vector<std::pair<hlt::Entity *, hlt::Vector>> obst;
					check_collisions(observer.get_planets(), a, b, ignore_list, obst);
					check_collisions(observer.get_ships(), a, b, ignore_list, obst);
					if (obst.empty()) return {b, true};

					auto closest_obstacle = *std::min_element(obst.begin(), obst.end(),
					                                          std::bind(sorting::sort_obst_by_distance, a, _1, _2));

					auto tangents = a.tangents(closest_obstacle.second,
					                           closest_obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

					if(a.in_radius(closest_obstacle.second, closest_obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN)) {
						return {a.closest_point(closest_obstacle.second, closest_obstacle.first->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN), true};
					}

					return correct_route(a, correct_left ? tangents.first : tangents.second, correct_left, max_corrections - 1);
				}

			public:
				Path(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore_list) : observer(observer), ignore_list(ignore_list) {}

				MovePromise navigate(const hlt::Ship *ship, const hlt::Vector &target) {
					ignore_list.push_back(ship->identify());
					auto correction_left = correct_route(ship->pos, target, true, 20);
					auto correction_right = correct_route(ship->pos, target, false, 20);

					hlt::Vector corrected_target = target;
					if (correction_left.second && correction_right.second) {
						if (ship->pos.dist(correction_left.first) + target.dist(correction_left.first) <
						    ship->pos.dist(correction_right.first) + target.dist(correction_right.first)) {
							corrected_target = correction_left.first;
						} else {
							corrected_target = correction_right.first;
						}
					} else {
						if (correction_left.second) corrected_target = correction_left.first;
						if (correction_right.second) corrected_target = correction_right.first;
					}

					return MovePromise::thrust(ship->entity_id,
					                           math::resize_line(ship->pos, corrected_target, hlt::constants::MAX_SPEED) - ship->pos);
				}
			};
		}
	}
}

#endif //HALITE_ORION_NAVIGATION_HPP_H
