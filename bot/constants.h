//
// Created by egordm on 10-11-2017.
//

#ifndef MYBOT_CONSTANTS_H
#define MYBOT_CONSTANTS_H

#define NAVIGATION_BACKUP_LEGACY_MOVE true
#define ALLOW_KAMIKAZE false

namespace bot {
    namespace constants {

		constexpr unsigned int MIN_DOCK_SPOTS_FOR_COLONIZATION = 1;
		constexpr double SHIP_AVOID_PROXIMITY = 10;

        constexpr double AVOID_PROXIMITY_OFFSET = 0.2;
		constexpr double DO_DEFEND = true;
	    constexpr double DEFEND_SEARCH_RADIUS_DIRECTIONAL = hlt::constants::MAX_SPEED * 6;
		constexpr double DEFEND_RADIUS = hlt::constants::DOCK_RADIUS + hlt::constants::WEAPON_RADIUS + hlt::constants::SHIP_RADIUS*2; // TODO: wont work it there are enemy planets closeby
		constexpr double DEFEND_SEARCH_RADIUS = DEFEND_RADIUS * 3; // Optimal large radius
		constexpr double DEFEND_RATIO = 0.6; // Optimal 0.4 TODO: change overtime
	    constexpr double DEFENDERS_PER_ATTACKER = 0.8; // Optimal 0.6

	    constexpr double DO_DISCOVERY = false;
		constexpr double DISCOVERY_RATIO = 0;

	    constexpr double DISCOVERY_SEARCH_RADIUS = 100;
		constexpr unsigned int ATTACK_PLANET_ASSIGNMENT_MULTIPLIER = 3;

        constexpr unsigned int ATTACK_PLANET_CHECK_RADIUS = 4;
    }
}


#endif //MYBOT_CONSTANTS_H
