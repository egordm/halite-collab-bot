#include <cmath>

#include "vector.hpp"

namespace hlt {
    Vector::Vector(double x, double y) : x(x), y(y) {}

    double Vector::dist(const Vector &target) const{
        const double dx = x - target.x;
        const double dy = y - target.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    Vector Vector::closest_point(const Vector &target, double radius) const{
        auto angle = target.angle_between(*this);
        return {target.x + radius * cos(angle), target.y + radius * sin(angle)};
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
    double Vector::angle_between(Vector v) const{
        return (v - *this).angle();
    }

    double Vector::length()const {
        return sqrt(x * x + y * y);
    }

    Vector Vector::normalize() const{
        auto l = length();
        return {x / l, y / l};
    }

    Vector Vector::operator-(const Vector &v) const{
        return {x - v.x, y - v.y};
    }

    Vector Vector::operator+(const Vector &v)const {
        return {x + v.x, y + v.y};
    }

    Vector Vector::operator/(double u) const{
        return {x / u, y / u};
    }

    Vector Vector::operator*(double u)const {
        return {x * u, y * u};
    }

    std::ostream &operator<<(std::ostream &out, const Vector &location) {
        out << '(' << location.x << ", " << location.y << ')';
        return out;
    }

    double rad_to_deg(double rad) {
        return rad * 180 / M_PI;
    }

    double deg_to_rad(double deg) {
        return deg / 180 * M_PI;
    }
}
