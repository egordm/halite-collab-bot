//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_UTILS_H
#define MYBOT_UTILS_H


#include "../hlt/types.hpp"
#include <vector>
#include "../hlt/vector.hpp"

namespace bot {
    template<class T>
    hlt::nullable <T> closest_object(std::vector <T> objects, hlt::Vector pos) {
        double dist = 10000000;
        hlt::nullable <T> ret;
        for (const auto &obj : objects) {
            if(obj.health <= 0) continue;
            double d = pos.dist_sq(obj.pos);
            if (d < dist) {
                dist = d;
                ret.first = obj;
                ret.second = true;
            }
        }
        return ret;
    }

    template<class T>
    void erase(std::vector<T> &v, T item) {
        for (int i = static_cast<int>(v.size() - 1); i >= 0; --i) {
            if(v[i].entity_id != item.entity_id) continue;
            v.erase(v.begin() + i);
            return;
        }
    }
}


#endif //MYBOT_UTILS_H
