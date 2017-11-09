//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_UTILS_H
#define MYBOT_UTILS_H


#include <types.hpp>
#include <vector>
#include <vector.hpp>

namespace bot {
    template<class T>
    hlt::nullable <T> closest_object(std::vector <T> objects, hlt::Vector pos) {
        double dist = 10000000;
        hlt::nullable <T> ret;
        for (const auto &obj : objects) {
            double d = pos.dist_sq(obj.pos);
            if (d < dist) {
                dist = d;
                ret.first = obj;
                ret.second = true;
            }
        }
        return ret;
    }
};


#endif //MYBOT_UTILS_H
