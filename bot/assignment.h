//
// Created by egordm on 10-11-2017.
//

#ifndef MYBOT_ASSIGNMENT_H
#define MYBOT_ASSIGNMENT_H


#include <types.hpp>

namespace bot {
    struct Assignment {
        enum Type {
            ColonizePlanet = 0,
            DefendPlanet = 1,
            AttackPlanet = 2,
            DefendShip = 3,
            AttackShip = 4,
            NoOp = 5,
        };

        hlt::EntityId ship_id;
        hlt::EntityId target_id;
        Type type;

        Assignment(hlt::EntityId ship, hlt::EntityId target, Type type);


    };

}


#endif //MYBOT_ASSIGNMENT_H
