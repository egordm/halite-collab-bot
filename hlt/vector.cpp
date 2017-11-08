#include <cmath>
#include <tuple>

#include "vector.hpp"

namespace hlt {
    Vector::Vector(double x, double y) : x(x), y(y) {}

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

    std::tuple<Vector, Vector> Vector::tangents(const Vector &center, double radius) const {
        auto diff = center - *this;
        auto dst = dist(center);
        auto a = asin(radius / dst);
        auto b = atan2(diff.y, diff.x);
        auto ta = b - a;
        auto tb = b + a;

        return std::make_tuple(center + Vector(radius * sin(ta), radius * -cos(ta)),
                               center + Vector(radius * -sin(tb), radius * cos(tb)));
    }

    /***
     * @return angle in <b>RAD</b>
     */
    double Vector::angle() const {
        return atan2(y, x);
    }

    /***
     * @param v
     * @return angle in <b>RAD</b>
     */
    double Vector::angle_between(Vector v) const {
        return (v - *this).angle();
    }

    double Vector::length() const {
        return sqrt(x * x + y * y);
    }

    Vector Vector::normalize() const {
        auto l = length();
        return {x / l, y / l};
    }

    double Vector::dist_line(const Vector &a, const Vector &b) const {
        auto end = b - a;
        auto x = *this - a;
        auto area = x.cross_prod(end);
        return area / end.length();
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

    double rad_to_deg(double rad) {
        return rad * 180 / M_PI;
    }

    double deg_to_rad(double deg) {
        return deg / 180 * M_PI;
    }

}
