//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_UTILS_H
#define MYBOT_UTILS_H


#include "../hlt/types.hpp"
#include <vector>
#include "../hlt/vector.hpp"
#include "sorting.h"

namespace bot {
    template<class T>
    hlt::nullable <T> closest_object(std::vector <T> objects, hlt::Vector pos) {
        if(objects.size() == 0) return std::make_pair(T(), false);
        return std::make_pair(std::min(objects.begin(), objects.end(), sorting::SortByDistance(pos)), true);
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
