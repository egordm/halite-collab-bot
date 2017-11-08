#include <cmath>

#include "vector.hpp"

namespace hlt {
    Vector::Vector(double x, double y) : x(x), y(y) {}

    std::ostream &operator<<(std::ostream &out, const Vector &location) {
        out << '(' << location.x << ", " << location.y << ')';
        return out;
    }

    double Vector::dist(const Vector &target) const {
        const double dx = x - target.x;
        const double dy = y - target.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    int Vector::orient_towards_in_deg(const Vector &target) const {
        return util::angle_rad_to_deg_clipped(orient_towards_in_rad(target));
    }

    double Vector::orient_towards_in_rad(const Vector &target) const {
        const double dx = target.x - x;
        const double dy = target.y - y;

        return std::atan2(dy, dx) + 2 * M_PI;
    }

    Vector Vector::get_closest_point(const Vector &target, const double target_radius) const {
        const double radius = target_radius + constants::MIN_DISTANCE_FOR_CLOSEST_POINT;
        const double angle_rad = target.orient_towards_in_rad(*this);

        const double x = target.x + radius * std::cos(angle_rad);
        const double y = target.y + radius * std::sin(angle_rad);

        return {x, y};
    }

    double Vector::angle() {
        return atan2(y, x) * 180 / M_PI; // TODO: create to deg function
    }

    double Vector::angle_between(Vector v) {
        return (v - *this).angle();
    }

    Vector Vector::operator-(const Vector &v) {
        return {x - v.x, y - v.y};
    }

    Vector Vector::operator+(const Vector &v) {
        return {x + v.x, y + v.y};
    }

    Vector Vector::operator/(double u) {
        return {x / u, y / u};
    }

    Vector Vector::operator*(double u) {
        return {x * u, y * u};
    }

    double Vector::length() {
        return sqrt(x * x + y * y);
    }

    Vector Vector::normalize() {
        auto l = length();
        return {x / l, y / l};
    }
}
