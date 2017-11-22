//
// Created by egordm on 19-11-2017.
//

#ifndef HALITE_ORION_COLLISION_CHECKING_HPP
#define HALITE_ORION_COLLISION_CHECKING_HPP

#include <entities.hpp>
#include <memory>
#include <math.hpp>
#include <cmath>
#include <algorithm>

namespace bot {
    namespace navigation {
        template<typename T>
        static void
        check_collisions(const std::vector<std::shared_ptr<T>> &objects, const hlt::Vector &a, const hlt::Vector &b,
                         const std::vector<hlt::EntityIdentifier> &ignore,
                         std::vector<std::pair<hlt::Entity *, hlt::Vector>> &result) {
            for (const auto &kv : objects) {
                if (std::find(ignore.begin(), ignore.end(), kv->identify()) != ignore.end()) continue;

                const auto dist_trajec = kv->pos.dist_line(a, b);
                if (dist_trajec < hlt::constants::SHIP_RADIUS + kv->radius) result.emplace_back(kv.get(), kv->pos);
            }
        }

        static double time_of_closest_approach(const hlt::Vector &p1, const hlt::Vector &p2, const hlt::Vector &v1,
                                               const hlt::Vector &v2,
                                               const double r1, const double r2, bool &collision) {
            auto pab = p1 - p2;
            auto vab = v1 - v2;
            double a = vab.dot_prod(vab);
            double b = 2 * pab.dot_prod(vab);
            double c = pab.dot_prod(pab) - pow((r1 + r2), 2);

            // Quadratic discriminant
            double discriminant = b * b - 4 * a * c;

            double t;

            if (discriminant < 0) {
                t = -b / (2 * a);
                collision = false;
            } else {
                double t0 = (-b + sqrt(discriminant)) / (2 * a);
                double t1 = (-b - sqrt(discriminant)) / (2 * a);
                t = std::min(t0, t1);

                collision = t >= 0;
            }

            return (t < 0) ? 0 : t;
        }

        static hlt::Vector
        point_of_collision(const hlt::Vector &p1, const hlt::Vector &p2, const hlt::Vector &v1, const hlt::Vector &v2,
                           const double r1, const double r2, const double t) {
            const auto c1 = p1 + v1 * t;
            const auto c2 = p2 + v2 * t;
            return (c1 - c2) * (r2 / (r1 + r2)) + c2;
        }

        static bool do_trajectoies_cross(const hlt::Vector &a1, const hlt::Vector &a2, const hlt::Vector &b1,
                                         const hlt::Vector &b2) {
            const auto ix = math::trajectory_intersection(a1, a2, b1, b2);
            return a1.dist_sq(b1) < a1.dist_sq(ix) && a2.dist_sq(b2) < a2.dist_sq(ix);
        }

        static double calculate_cross_speed(const hlt::Vector &a1, const hlt::Vector &a2, const hlt::Vector &b1,
                                               const hlt::Vector &b2, const double &r1, const double &r2) {
            const auto ix = math::trajectory_intersection(a1, a2, b1, b2);

            bool collide;
            const auto t = time_of_closest_approach(a1, a2, b1 -a1, b2 - a1, r1, r2, collide);
            if(!collide) return 0; // TODO: pass it instead. No need for recalculation
            const auto colp = point_of_collision(a1, a2, b1 -a1, b2 - a1, r1, r2, t);

            if(fabs(a1.angle_between(ix, a2)) <= M_PI / 4) {
                return a1.dist(colp) - (r1 + r2);
            } else {
                return 0; // TODO: bypass by adjusting trajectory
            }

        }
    }
}
#endif //HALITE_ORION_COLLISION_CHECKING_HPP
