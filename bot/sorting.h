//
// Created by egordm on 12-11-2017.
//

#ifndef MYBOT_SORTING_H
#define MYBOT_SORTING_H


#include <vector.hpp>
#include <types.hpp>
#include <entities.hpp>
#include <w32api/commctrl.h>

namespace bot {
    namespace sorting {
        struct SortByDistance {
            hlt::Vector pos;

            explicit SortByDistance(const hlt::Vector &pos) : pos(pos) {}

            template<class T>
            inline bool operator()(const T &e1, const T &e2) {
                return (pos.dist_sq(e1->pos) < pos.dist_sq(e2->pos));
            }
        };

        struct FilterOwnerMask {
            hlt::PlayerId owner_id;
            short owner_mask;

            explicit FilterOwnerMask(hlt::PlayerId owner_id, short owner_mask)
                    : owner_id(owner_id), owner_mask(owner_mask) {};

            template<class T>
            inline bool operator()(const T &el) {
                return (owner_mask & el->owner_mask(owner_id)) != 0;
            }
        };

	    struct FilterByDistance {
		    hlt::Vector pos;
		    double distance;

		    FilterByDistance(const hlt::Vector &pos, double distance) : pos(pos), distance(distance) {}

		    template<class T>
		    inline bool operator()(const T &el) {
			    return pos.dist_sq(el->pos) < distance;
		    }
	    };

	    struct FilterByStatus {
		    hlt::ShipDockingStatus status;

		    explicit FilterByStatus(hlt::ShipDockingStatus status) : status(status) {}

		    template<class T>
		    inline bool operator()(const T &el) {
			    return el->docking_status == status;
		    }
	    };
    }
};


#endif //MYBOT_SORTING_H
