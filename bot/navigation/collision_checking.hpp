//
// Created by egordm on 19-11-2017.
//

#ifndef HALITE_ORION_COLLISION_CHECKING_HPP
#define HALITE_ORION_COLLISION_CHECKING_HPP

#include <entities.hpp>
#include <memory>
#include <algorithm>

namespace bot {
	namespace navigation {
		template<typename T>
		static std::vector <std::pair<hlt::Entity *, hlt::Vector>> check_collisions(std::vector <std::shared_ptr<T>> &objects,
		                                                                            const hlt::Vector &a, const hlt::Vector &b,
		                                                                            std::vector <hlt::EntityIdentifier> &ignore) {
			std::vector <std::pair<hlt::Entity *, hlt::Vector>> ret;
			for (const auto &kv : objects) {
				if (std::find(ignore.begin(), ignore.end(), kv->identify()) != ignore.end()) continue;

				const auto dist_trajec = kv->pos.dist_line(a, b);
				if (dist_trajec < hlt::constants::SHIP_RADIUS + kv->radius) ret.emplace_back(kv.get(), kv->pos);
			}
			return ret;
		}
	}
}
#endif //HALITE_ORION_COLLISION_CHECKING_HPP
