//
// Created by egordm on 24-11-2017.
//

#ifndef HALITE_ORION_RVO_ALGORITHM_HPP
#define HALITE_ORION_RVO_ALGORITHM_HPP

#include <vector.hpp>
#include <vector>
#include <cmath>
#include <limits>
#include <math.hpp>

namespace bot { namespace navigation { namespace rvo {
	constexpr double UNCERTAINTY_ANGLE = 0.02;

	class VelocityObstacle {
		/**
		 * S1 and S2 are relative
		 * 	 apex
		 *    /\
		 *   /  \
		 *  /____\
		 * s1	 s2
		 */
	public:
		hlt::Vector apex, first_edge, second_edge;

		VelocityObstacle(const hlt::Vector &apex, const hlt::Vector &side1, const hlt::Vector &side2)
				: apex(apex), first_edge(side1), second_edge(side2) {}

		VelocityObstacle(const hlt::Vector &vel, const hlt::Vector &pos, const hlt::Vector &center, const double radius) {
			const auto diff = center - pos;
			auto dst = pos.dist(center);
			if (radius > dst) dst = radius; // Pick the closest point on the circle if inside

			auto a = asin(radius / dst);
			auto b = atan2(diff.y, diff.x);
			auto ta = b - a; // Theta ort right
			auto tb = b + a; // Theta ort left

			apex = vel;
			first_edge = hlt::Vector(cos(ta), sin(ta));
			second_edge = hlt::Vector(cos(tb), sin(tb));
		}

		bool inside(const hlt::Vector &v) const {
			const auto ptv = v - apex;
			const auto pa = atan2(ptv.y, ptv.x);
			auto pf = atan2(second_edge.y, second_edge.x);
			auto ps = atan2(first_edge.y, first_edge.x);

			if (ps > pf) {
				if (pa < 0) ps -= M_PI * 2;
				else pf += M_PI * 2;
			}

			return ps + UNCERTAINTY_ANGLE < pa && pa < pf - UNCERTAINTY_ANGLE;
		}

		const hlt::Vector &operator[](const int &index) const {
			return index == 0 ? first_edge : second_edge;
		}
	};

	static bool in_vos(const hlt::Vector &velocity, const std::vector<VelocityObstacle> &obstacles) {
		for (const auto &obstacle : obstacles) if (obstacle.inside(velocity)) return true;
		return false;
	}

	static std::vector<VelocityObstacle> compute_velocity(const hlt::Vector &pos, const double &rad,
	                                                      const std::vector<std::tuple<hlt::Vector, hlt::Vector, double>> &neighbours) {
		std::vector<VelocityObstacle> velocity_obstacles;
		velocity_obstacles.reserve(neighbours.size());

		for (const auto &neighbour : neighbours) {
			const auto &pb = std::get<0>(neighbour);
			const auto &vb = std::get<1>(neighbour);
			const auto rb = std::get<2>(neighbour);

			velocity_obstacles.emplace_back(vb, pos, pb, rad + rb);
		}

		return velocity_obstacles;
	}

	static std::vector<hlt::Vector> vel_intersections(const std::vector<VelocityObstacle> &obstacles) {
		std::vector<hlt::Vector> candidate_velocities;
		for (auto i = 0; i < obstacles.size(); ++i) {
			const auto vo1 = obstacles[i];
			for (auto j = i + 1; j < obstacles.size(); ++j) {
				const auto vo2 = obstacles[i];

				hlt::Vector pv;
				for (auto k = 0; k < 2; ++k) {
					for (auto l = 0; l < 2; ++l) {
						pv = math::trajectory_intersection(vo1.apex, vo1.apex + vo1[k], vo2.apex, vo2.apex + vo2[l]);
						if (!pv.isnan() && !in_vos(pv, obstacles)) candidate_velocities.push_back(pv);
					}
				}
			}
		}
		return candidate_velocities;
	}

}}}
#endif //HALITE_ORION_RVO_ALGORITHM_HPP

