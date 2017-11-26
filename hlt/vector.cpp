#include <cmath>
#include <tuple>
#include <limits>

#include "vector.hpp"

namespace hlt {
	Vector::Vector(double x, double y) : x(x), y(y) {}

	/**
	 * Create vector from angle in RAD
	 * @param angle
	 * @param magnitude
	 * @return
	 */
	Vector Vector::from_angle(double angle, double magnitude) {
		Vector ret;
		angle -= 2 * M_PI;
		ret.x = cos(angle);
		ret.y = sin(angle);
		ret = ret * magnitude;
		return ret;
	}

	double Vector::dist_sq(const Vector &target) const {
		const Vector d = *this - target;
		return d.x * d.x + d.y * d.y;
	}

	double Vector::dist(const Vector &target) const {
		return std::sqrt(dist_sq(target));
	}

	Vector Vector::closest_point(const Vector &center, double radius) const {
		auto angle = center.angle_between(*this);
		return {center.x + radius * cos(angle), center.y + radius * sin(angle)};
	}

	std::pair<Vector, Vector> Vector::tangents(const Vector &center, const double radius) const {
		const auto diff = center - *this;
		auto dst = dist(center);
		if (radius > dst) dst = radius; // Pick the closest point on the circle if inside

		auto a = asin(radius / dst);
		auto b = atan2(diff.y, diff.x);
		auto ta = b - a; // Theta ort right
		auto tb = b + a; // Theta ort left

		return std::make_pair(center + Vector(radius * sin(ta), radius * -cos(ta)),
		                      center + Vector(radius * -sin(tb), radius * cos(tb)));
	}

	/***
	 * @return angle in <b>RAD</b>
	 */
	double Vector::angle() const {
		return atan2(y, x) + 2 * M_PI;
	}

	/***
	 * @param v
	 * @return angle in <b>RAD</b>
	 */
	double Vector::angle_between(Vector v) const {
		return (v - *this).angle();
	}

	double Vector::angle_between(Vector v1, Vector v2) const {
		auto diffa = v1 - *this;
		auto diffb = v1 - v2;

		auto dot = diffa.dot_prod(diffb);
		auto cross = diffa.cross_prod(diffb);
		return atan2(cross, dot) + 2 * M_PI;
	}

	double Vector::length() const {
		return sqrt(x * x + y * y);
	}

	Vector Vector::normalize() const {
		auto l = length();
		if (l == 0) return {};
		return {x / l, y / l};
	}

	double Vector::dist_line(const Vector &a, const Vector &b) const {
		return dist(closest_point_line(a, b));
	}

	Vector Vector::closest_point_line(const Vector &a, const Vector &b) const {
		auto ds = a.dist_sq(b);
		if (ds < 0.1) return a;

		auto delta = b - a;
		auto t = std::fmax(0, std::fmin(1, (*this - a).dot_prod(delta) / ds));
		auto projection = delta * t + a;

		return projection;
	}

	Vector Vector::operator-(const Vector &v) const {
		return {x - v.x, y - v.y};
	}

	Vector Vector::operator+(const Vector &v) const {
		return {x + v.x, y + v.y};
	}

	Vector Vector::operator/(double u) const {
		return {x / u, y / u};
	}

	Vector Vector::operator*(double u) const {
		return {x * u, y * u};
	}

	std::ostream &operator<<(std::ostream &out, const Vector &location) {
		out << '(' << location.x << ", " << location.y << ')';
		return out;
	}

	double Vector::dot_prod(const Vector &v) const {
		return x * v.x + y * v.y;
	}

	double Vector::cross_prod(const Vector &v) const {
		return x * v.y - y * v.x;
	}

	bool Vector::in_radius(const Vector &v, double radius) const {
		return dist(v) < radius;
	}

	Vector Vector::avoid_point(const Vector &v, double radius) const {
		if (in_radius(v, radius)) return closest_point(v, radius);
		const auto tans = tangents(v, radius);
		return dist(tans.first) < dist(tans.second) ? tans.first : tans.second;
	}

	double rad_to_deg(double rad) {
		return rad * 180 / M_PI;
	}

	double deg_to_rad(double deg) {
		return deg / 180 * M_PI;
	}

	Vector Vector::NAN_VEC = Vector(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());

	bool Vector::isnan() const {
		return std::isnan(x) || std::isnan(y);
	}

	bool Vector::is_zero() const {
		return x == 0 || y == 0;
	}
}
