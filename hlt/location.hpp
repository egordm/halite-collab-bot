#pragma once

#include <ostream>

#include "constants.hpp"
#include "util.hpp"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace hlt {
    struct Vector {
        double pos_x, pos_y;

        double get_distance_to(const Vector &target) const;

        int orient_towards_in_deg(const Vector &target) const;

        double orient_towards_in_rad(const Vector &target) const;

        Vector get_closest_point(const Vector &target, const double target_radius) const;

        friend std::ostream &operator<<(std::ostream &out, const Vector &location);
    };

    static bool operator==(const Vector &l1, const Vector &l2) {
        return l1.pos_x == l2.pos_x && l1.pos_y == l2.pos_y;
    }
}
