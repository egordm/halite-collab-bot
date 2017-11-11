#pragma once

#include <algorithm>

#include "vector.hpp"
#include "entities.hpp"

namespace hlt {
    namespace collision {
        static double square(const double num) {
            return num * num;
        }

        /**
         * Test whether a given line segment intersects a circular area.
         *
         * @param start  The start of the segment.
         * @param end    The end of the segment.
         * @param circle The circle to test against.
         * @param fudge  An additional safety zone to leave when looking for collisions. Probably set it to ship radius.
         * @return true if the segment intersects, false otherwise
         */
        static bool segment_circle_intersect(
                const Vector& start,
                const Vector& end,
                const Entity& circle,
                const double fudge)
        {
            // Parameterize the segment as start + t * (end - start),
            // and substitute into the equation of a circle
            // Solve for t
            const double circle_radius = circle.radius;
            const double start_x = start.x;
            const double start_y = start.y;
            const double end_x = end.x;
            const double end_y = end.y;
            const double center_x = circle.pos.x;
            const double center_y = circle.pos.y;
            const double dx = end_x - start_x;
            const double dy = end_y - start_y;

            const double a = square(dx) + square(dy);

            const double b =
                    -2 * (square(start_x) - (start_x * end_x)
                    - (start_x * center_x) + (end_x * center_x)
                    + square(start_y) - (start_y * end_y)
                    - (start_y * center_y) + (end_y * center_y));

            if (a == 0.0) {
                // Start and end are the same point
                return start.dist(circle.pos) <= circle_radius + fudge;
            }

            // Time along segment when closest to the circle (vertex of the quadratic)
            const double t = std::min(-b / (2 * a), 1.0);
            if (t < 0) {
                return false;
            }

            const double closest_x = start_x + dx * t;
            const double closest_y = start_y + dy * t;
            const double closest_distance = Vector{closest_x, closest_y}.dist(circle.pos);

            return closest_distance <= circle_radius + fudge;
        }
    }
}
