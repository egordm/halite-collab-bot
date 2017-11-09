//
// Created by egordm on 9-11-2017.
//

#ifndef MYBOT_OBSERVER_H
#define MYBOT_OBSERVER_H

#include <map.hpp>

namespace bot {
    const unsigned short empty_mask = 0x01;
    const unsigned short friendly_mask = 0x02;
    const unsigned short enemy_mask = 0x03;

    class Observer {
    private:
        hlt::Map map;
        std::unordered_map<unsigned int, hlt::Vector> velocities;
    public:
        const hlt::PlayerId my_id;
        unsigned int step;

        Observer(hlt::PlayerId id, const hlt::Map &map);

        void observe(const hlt::Map &new_map);

        hlt::nullable<hlt::Planet> closest_planet(hlt::Vector pos);

        hlt::nullable<hlt::Planet> closest_planet(hlt::Vector pos, unsigned short owner_mask) const;

        const hlt::Map &getMap() const;

        const std::vector<hlt::Ship> my_ships();
    };
}

#endif //MYBOT_OBSERVER_H
