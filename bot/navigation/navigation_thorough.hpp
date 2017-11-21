//
// Created by egordm on 21-11-2017.
//

#ifndef HALITE_ORION_NAVIGATION_THOROUGH_H
#define HALITE_ORION_NAVIGATION_THOROUGH_H

#include <observer.h>
#include <math.hpp>
#include <sorting.h>
#include <utils.h>
#include "navigator.h"
#include "navigation_fast.hpp"

namespace bot {
	namespace navigation {

		class NavigationCorrector : public FastPath {

		protected:
			void
			get_obstacles(const hlt::Vector &a, const hlt::Vector &b, std::vector<std::pair<hlt::Entity *, hlt::Vector>> &obst) override {
				FastPath::get_obstacles(a, b, obst);
				// Ship trajectory correct
				auto vel = math::resize_line(a, b, target_speed) - a;
				const auto &others = observer.get_ships(a + vel, SHIP_COLLISION_CHECK_RADIUS, hlt::all_mask);

				for (const auto &other : others) {
					if(std::find(ignore_list.begin(), ignore_list.end(), other->identify()) != ignore_list.end()) continue;
					bool collides;
					auto colt = time_of_closest_approach(a, other->pos, vel, other->vel, hlt::constants::SHIP_RADIUS, other->radius, collides);
					if(!collides) continue;

					auto colp = point_of_collision(a, other->pos, vel, other->vel, hlt::constants::SHIP_RADIUS, other->radius, colt);
					obst.emplace_back(other.get(), colp);

					std::stringstream ss;
					ss << "Colliding with ship: " <<other->entity_id<< " Pos: " << other->pos <<  " Colp: " << colp;
					hlt::Log::log(ss.str());
				}
			}

		public:
			NavigationCorrector(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore_list, double target_speed) : FastPath(
					observer, ignore_list, target_speed) {}

		};
	}
}

#endif //HALITE_ORION_NAVIGATION_THOROUGH_H
