#include <cmath>

#include "location.hpp"

namespace hlt {
    std::ostream& operator<<(std::ostream& out, const Vector& location) {
        out << '(' << location.pos_x << ", " << location.pos_y << ')';
        return out;
    }

    double Vector::get_distance_to(const Vector &target) const {
        const double dx = pos_x - target.pos_x;
        const double dy = pos_y - target.pos_y;
        return std::sqrt(dx * dx + dy * dy);
    }

    int Vector::orient_towards_in_deg(const Vector &target) const {
        return util::angle_rad_to_deg_clipped(orient_towards_in_rad(target));
    }

    double Vector::orient_towards_in_rad(const Vector &target) const {
        const double dx = target.pos_x - pos_x;
        const double dy = target.pos_y - pos_y;

        return std::atan2(dy, dx) + 2 * M_PI;
    }

    Vector Vector::get_closest_point(const Vector &target, const double target_radius) const {
        const double radius = target_radius + constants::MIN_DISTANCE_FOR_CLOSEST_POINT;
        const double angle_rad = target.orient_towards_in_rad(*this);

        const double x = target.pos_x + radius * std::cos(angle_rad);
        const double y = target.pos_y + radius * std::sin(angle_rad);

        return {x, y};
    }
}
