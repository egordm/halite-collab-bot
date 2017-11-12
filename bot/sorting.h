//
// Created by egordm on 12-11-2017.
//

#ifndef MYBOT_SORTING_H
#define MYBOT_SORTING_H


#include <vector.hpp>
#include "assignment.h"

namespace bot {
    namespace sorting {
        struct SortByDistance {
            hlt::Vector pos;

            explicit SortByDistance(const hlt::Vector &pos) : pos(pos) {}

            template<class T>
            inline bool operator()(const T &e1, const T &e2) {
                return (pos.dist_sq(e1.pos) < pos.dist_sq(e2.pos));
            }
        };

        struct SameType {
            Assignment::Type type;

            explicit SameType(Assignment::Type type) : type(type) {}

            inline bool operator()(const Assignment *e1) {
                return e1->get_type() == type;
            }
        };

        struct IsDockStatus {
            hlt::ShipDockingStatus status;

            explicit IsDockStatus(hlt::ShipDockingStatus status) : status(status) {}

            inline bool operator()(const hlt::Ship &ship) {
                return ship.docking_status == status;
            }
        };

    }
};


#endif //MYBOT_SORTING_H
