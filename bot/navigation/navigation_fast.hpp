//
// Created by egordm on 15-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_HPP_H
#define HALITE_ORION_NAVIGATION_HPP_H

#include <sstream>
#include <algorithm>
#include "../../hlt/move.hpp"
#include "../../hlt/map.hpp"
#include "../utils.h"
#include "../sorting.h"

using namespace std::placeholders;

namespace bot {
	namespace navigation {
		namespace fast {
			//TODO: idea. Pick one tangent left or right and keep picking till a clear trajectory is set

			constexpr double BYPASS_MARGIN = 0.1;

			static std::pair<bool, hlt::Vector>
			correct_route(Observer &observer, const hlt::Vector &a, const hlt::Vector &b, bool correct_left,
			              unsigned int max_corrections, std::vector<hlt::EntityIdentifier> &ignore_list) {
				if (max_corrections == 0) return std::make_pair(false, b);

				std::vector<std::shared_ptr<hlt::Entity>> obst;
				for (const auto &kv : observer.getMap()->planets) {
					const auto dist_trajec = kv.second->pos.dist_line(a, b);
					if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) obst.push_back(kv.second);
				}
				for (const auto &kv : observer.getMap()->ships) {
					if(kv.second->docking_status == hlt::ShipDockingStatus::Undocked /*&& kv.second->owner_id == observer.my_id*/) continue;
					const auto dist_trajec = kv.second->pos.dist_line(a, b);
					if (dist_trajec < hlt::constants::SHIP_RADIUS + kv.second->radius) obst.push_back(kv.second);
				}
				for(const auto &ignore : ignore_list) utils::erase_if(obst, std::bind(sorting::filter_by_identifier, ignore, _1));
				if (obst.empty()) return std::make_pair(true, b);

				auto closest_obstacle = *std::min_element(obst.begin(), obst.end(), std::bind(sorting::sort_by_distance, a, _1, _2));
				auto tangents = a.tangents(closest_obstacle->pos, closest_obstacle->radius + hlt::constants::SHIP_RADIUS + BYPASS_MARGIN);

				if (correct_left) return correct_route(observer, a, tangents.first, correct_left, max_corrections - 1, ignore_list);
				return correct_route(observer, a, tangents.second, correct_left, max_corrections - 1, ignore_list);
			};

			static hlt::Move navigate_towards(Observer &observer, hlt::Ship *ship, const hlt::Vector &target,
			                                  std::vector<hlt::EntityIdentifier> &ignore_list) {
				ignore_list.push_back(ship->identify());
				auto correction_left = correct_route(observer, ship->pos, target, true, 20, ignore_list);
				auto correction_right = correct_route(observer, ship->pos, target, false, 20, ignore_list);

				hlt::Vector corrected_target = target;
				if (correction_left.first && correction_right.first) {
					if (ship->pos.dist(correction_left.second) + target.dist(correction_left.second) <
					    ship->pos.dist(correction_right.second) + target.dist(correction_right.second)) {
						corrected_target = correction_left.second;
					} else {
						corrected_target = correction_right.second;
					}
				} else {
					if (correction_left.first) corrected_target = correction_left.second;
					if (correction_right.first) corrected_target = correction_right.second;
				}

				const double angle = std::round(hlt::rad_to_deg(ship->pos.angle_between(corrected_target)));
				const unsigned int speed = static_cast<const unsigned int>(std::min(7, std::max(0,
				                                                                                static_cast<const int &>(std::round(ship->pos.dist(target))))));

				return hlt::Move::thrust(ship->entity_id, speed, static_cast<const int>(angle));
			}
		}
	}
}

#endif //HALITE_ORION_NAVIGATION_HPP_H
