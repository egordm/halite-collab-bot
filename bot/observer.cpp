//
// Created by egordm on 9-11-2017.
//

#include "observer.h"

namespace bot {
    Observer::Observer(hlt::PlayerId id, const hlt::Map &map) : id(id), map(map) {}

    void Observer::observe(const hlt::Map &new_map) {
        /*for (int i = 0; i < new_map.ships.size(); ++i) {
            for (int j = 0; j < new_map.ships[i].size(); ++j) {
                const auto &ship = new_map.ships[i][j];
                velocities.insert(std::make_pair(ship.entity_id, ship.pos - this->map.ships[i][j].pos));
            }
        }*/

        this->map = new_map;
        step++;
    }

    const hlt::Map &Observer::getMap() const {
        return map;
    }
}