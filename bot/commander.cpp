//
// Created by egordm on 9-11-2017.
//

#include "../hlt/log.hpp"
#include "commander.h"
#include "utils.h"
#include "constants.h"
#include "defines.h"


bot::Commander::Commander(bot::Observer &observer, bot::Navigator *navigator) : observer(observer),
                                                                                navigator(navigator) {}

bot::Commander::~Commander() {
    for (auto ass : assignments) delete ass.second;
    assignments.clear();
}

std::vector<hlt::Move> bot::Commander::command() {
    for(auto it = assignments.begin(); it != assignments.end();) {
        if(!it->second->is_valid(observer)) {
            delete it->second;
            it = assignments.erase(it);
        }
        else it++;
    }

    for (const auto &ship : observer.my_ships()) {
        if(assignments.find(ship.entity_id) != assignments.end()) continue;
        if (ship.docking_status != hlt::ShipDockingStatus::Undocked) continue;
        add_assignment(assign(ship));
    }

    std::vector<hlt::Move> ret;
    for (const auto &kv : assignments) {
        if(DEBUG) hlt::Log::log(kv.second->str());
        const auto &move = kv.second->get_move(observer, navigator);
        if(move.second) ret.push_back(move.first);
    }

    return ret;
}

bot::Assignment *bot::Commander::assign(const hlt::Ship &ship) {
    auto empty_planets = observer.get_planets(hlt::empty_mask + hlt::friendly_mask); // TODO: do what cody did
    while (!empty_planets.empty()) {
        auto planet = closest_object(empty_planets, ship.pos);
        if (!planet.second) continue;
        bot::erase(empty_planets, planet.first);

        auto ass = new ColonizeAssignment(ship.entity_id, planet.first.entity_id);
        if (can_assign(ass)) return ass;
        delete ass;
    }

    auto enemy_planets = observer.get_planets(hlt::empty_mask); // TODO: do what cody did
    while (!enemy_planets.empty()) {
        auto planet = closest_object(enemy_planets, ship.pos);
        if (!planet.second) continue;
        bot::erase(enemy_planets, planet.first);

        auto ass = new AttackPlanetAssignment(ship.entity_id, planet.first.entity_id);
        if (can_assign(ass)) return ass;
        delete ass;
    }

    auto enemy = closest_object(observer.get_enemies(), ship.pos);
    if (enemy.second) return new AttackShipAssignment(ship.entity_id, enemy.first.entity_id);

    return new Assignment(ship.entity_id);
}

void bot::Commander::add_assignment(bot::Assignment *assignment) {
    assignments.insert(std::make_pair(assignment->ship_id, assignment));
}

bool bot::Commander::can_assign(bot::Assignment *assignment) {
    if (!assignment->is_valid(observer)) return false;
    unsigned int dupe_count = 0;
    for (const auto &kv : assignments) {
        if (kv.first == assignment->ship_id) return false;
        if (assignment == kv.second) dupe_count++;
    }
    return (dupe_count < assignment->max_assignments(observer));
}
