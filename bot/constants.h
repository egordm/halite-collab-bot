//
// Created by egordm on 10-11-2017.
//

#ifndef MYBOT_CONSTANTS_H
#define MYBOT_CONSTANTS_H


namespace bot {
    namespace constants {
        constexpr unsigned int MIN_DOCK_SPOTS_FOR_COLONIZATION = 1;
        constexpr double SHIP_AVOID_PROXIMITY = 10;
        constexpr double AVOID_PROXIMITY_OFFSET = 0.2;
        constexpr double DEFEND_RADIUS = 8; // TODO: wont work it there are enemy planets closeby
        constexpr double DEFEND_SEARCH_RADIUS = 32;
        constexpr double DEFEND_RATIO = 0.4;

        constexpr unsigned int ATTACK_PLANET_ASSIGNMENT_MULTIPLIER = 3;
        constexpr unsigned int ATTACK_PLANET_CHECK_RADIUS = 4;
    }
}


#endif //MYBOT_CONSTANTS_H
