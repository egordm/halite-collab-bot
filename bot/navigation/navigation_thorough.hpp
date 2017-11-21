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
		public:
			NavigationCorrector(Observer &observer, std::vector<hlt::EntityIdentifier> &ignore_list, double target_speed) : FastPath(
					observer, ignore_list, target_speed) {}

		};
	}
}

#endif //HALITE_ORION_NAVIGATION_THOROUGH_H
