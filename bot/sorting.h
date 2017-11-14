//
// Created by egordm on 12-11-2017.
//

#ifndef MYBOT_SORTING_H
#define MYBOT_SORTING_H


#include "../hlt/vector.hpp"
#include "../hlt/types.hpp"
#include "../hlt/entities.hpp"
#include <memory>

namespace bot {
	namespace sorting {


		static bool sort_by_distance(const hlt::Vector &pos, const std::shared_ptr<hlt::Entity> &e1, const std::shared_ptr<hlt::Entity> &e2) {
			return (pos.dist_sq(e1->pos)) < pos.dist_sq(e2->pos) && e1->is_alive();
		}

		static bool sort_by_corrected_distance(const hlt::Vector &pos, const std::shared_ptr<hlt::Entity> &e1, const std::shared_ptr<hlt::Entity> &e2) {
			return (pos.dist_sq(pos.closest_point(e1->pos, e1->radius))) < pos.dist_sq(pos.closest_point(e2->pos, e2->radius)) && e1->is_alive();
		}


		struct SortByDistance {
			hlt::Vector pos;

			explicit SortByDistance(const hlt::Vector &pos) : pos(pos) {}

			template<class T>
			inline bool operator()(const T &e1, const T &e2) {
				return (pos.dist_sq(e1->pos)) < pos.dist_sq(e2->pos) && e1->is_alive();

				/* return (pos.dist_sq(pos.closest_point(e1->pos, e1->radius)) < pos.dist_sq(pos.closest_point(e2->pos, e2->radius))) &&
						 e1->is_alive();*/
			}
		};


		struct FilterOwnerMask {
			hlt::PlayerId owner_id;
			short owner_mask;

			explicit FilterOwnerMask(hlt::PlayerId owner_id, short owner_mask)
					: owner_id(owner_id), owner_mask(owner_mask) {};

			template<class T>
			inline bool operator()(const T &el) {
				return (owner_mask & el->owner_mask(owner_id)) != 0 && el->is_alive();
			}
		};

		struct FilterByDistance {
			hlt::Vector pos;
			double distance;

			FilterByDistance(const hlt::Vector &pos, double distance) : pos(pos), distance(distance) {}

			template<class T>
			inline bool operator()(const T &el) {
				return pos.dist_sq(el->pos) < distance && el->is_alive();
			}
		};

		struct FilterByStatus {
			hlt::ShipDockingStatus status;

			explicit FilterByStatus(hlt::ShipDockingStatus status) : status(status) {}

			template<class T>
			inline bool operator()(const T &el) {
				return el->docking_status == status && el->is_alive();
			}
		};

		static bool filter_by_distance(const hlt::Vector &pos, double distance, const std::shared_ptr<hlt::Entity> &el) {
			return pos.dist(el->pos) < distance && el->is_alive();
		}

		static bool filter_by_owner_mask(const hlt::PlayerId owner_id, const unsigned short owner_mask, const std::shared_ptr<hlt::Entity> &el) {
			return (owner_mask & el->owner_mask(owner_id)) != 0 && el->is_alive();
		}

		static bool filter_by_status(const hlt::ShipDockingStatus &status, const std::shared_ptr<hlt::Ship> &el) {
			return el->docking_status == status;
		}

		static bool filter_by_size(const std::shared_ptr<hlt::Entity> &e1, const std::shared_ptr<hlt::Entity> &e2) {
			return e1->radius > e2->radius;
		}
	}
};


#endif //MYBOT_SORTING_H
