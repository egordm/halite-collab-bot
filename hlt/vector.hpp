#pragma once

#include <ostream>

#include "constants.hpp"
#include "util.hpp"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

namespace hlt {
    struct Vector {
        double x, y;

        Vector(double x, double y);

        double dist(const Vector &target) const;

        int orient_towards_in_deg(const Vector &target) const;

        double orient_towards_in_rad(const Vector &target) const;

        Vector get_closest_point(const Vector &target, const double target_radius) const;

        friend std::ostream &operator<<(std::ostream &out, const Vector &location);

        double angle();

        double angle_between(Vector v);

        Vector operator-(const Vector &v);

        Vector operator+(const Vector &v);

        Vector operator/(double u);

        Vector operator*(double u);

        double length();

        Vector normalize();
    };

    static bool operator==(const Vector &l1, const Vector &l2) {
        return l1.x == l2.x && l1.y == l2.y;
    }
}
