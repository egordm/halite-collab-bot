//
// Created by egordm on 17-11-2017.
//

#ifndef HALITE_ORION_MATH_H
#define HALITE_ORION_MATH_H

#include <vector.hpp>

namespace bot {
	namespace math {
		struct Line {
			double slope;
			double intercept;

			Line(double slope, double intercept) : slope(slope), intercept(intercept) {}
		};

		static Line find_line(const hlt::Vector &a, const hlt::Vector &b) {
			auto diff = b - a;
			auto slope = diff.y / diff.x;
			return {slope, a.y - slope * a.x};
		};

		static hlt::Vector intersection(const Line &l1, const Line &l2) {
			if (l1.slope == l2.slope) return hlt::Vector::NAN_VEC;
			auto ix = (l2.intercept - l1.intercept) / (l1.slope - l2.slope);
			return {ix, l1.slope * ix + l1.intercept};
		}

		static std::pair<hlt::Vector, hlt::Vector>
		tangent_intersections(const hlt::Vector &a, const hlt::Vector &b, const hlt::Vector &c, const double r) {
			auto tana = a.tangents(c, r);
			auto tanb = b.tangents(c, r);

			auto la1 = bot::math::find_line(a, tana.first);
			auto la2 = bot::math::find_line(a, tana.second);
			auto lb1 = bot::math::find_line(b, tanb.first);
			auto lb2 = bot::math::find_line(b, tanb.second);

			auto inter1 = intersection(la1, lb2);
			if(inter1.isnan() || a.dist_sq(inter1) < c.dist_sq(inter1)) inter1 = hlt::Vector::NAN_VEC;
			auto inter2 = intersection(la2, lb1);
			if(inter2.isnan() || a.dist_sq(inter2) < c.dist_sq(inter2)) inter2 = hlt::Vector::NAN_VEC;

			return {inter1, inter2};
		};

		static hlt::Vector resize_line(const hlt::Vector &a, const hlt::Vector &b, const double length) {
			return a + (b - a).normalize() * length;
		}
	}
}

#endif //HALITE_ORION_MATH_H
