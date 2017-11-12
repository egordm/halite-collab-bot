//
// Created by egordm on 9-11-2017.
//

#include <algorithm>
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
    unsigned int defending = 0;
    for(auto it = assignments.begin(); it != assignments.end();) {
        if(!it->second->is_valid(observer)) {
            delete it->second;
            it = assignments.erase(it);
            continue;
        }
        if(it->second->get_type() == Assignment::Type::DefendPlanet) defending++;
        it++;
    }

    auto defenders_left = Observer::filter(observer.my_ships(), hlt::ShipDockingStatus::Undocked).size()
                          * constants::DEFEND_RATIO - defending;

    // Assign defenders
/*    const auto &attacked_planets = observer.get_attacked_planets();
    for(auto it = attacked_planets.begin(); it != attacked_planets.end() && defenders_left > 0; ++it) {
        const auto &ships_near = Observer::filter(
                observer.ships_around(it->pos, it->radius + constants::DEFEND_RADIUS, hlt::friendly_mask),
                hlt::ShipDockingStatus::Undocked);
        const auto attackers =  Observer::filter(
                observer.ships_around(it->pos, it->radius + constants::DEFEND_RADIUS, hlt::enemy_mask),
                hlt::ShipDockingStatus::Undocked);
        for(int i = 0; i < ships_near.size() && i < attackers.size() && defenders_left > 0; ++i) {
            add_assignment(new DefendPlanetAssignment(ships_near[i].entity_id, it->entity_id));
            defenders_left--;
        }
    }*/

    for (const auto &ship : Observer::filter(observer.my_ships(), hlt::ShipDockingStatus::Undocked)) {
        if(assignments.find(ship.entity_id) != assignments.end()) continue;
        add_assignment(assign(ship));
    }

    std::vector<hlt::Move> ret;
    for (const auto &kv : assignments) {
#if DEBUG
       hlt::Log::log(kv.second->str());
#endif
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
    auto res = assignments.find(assignment->ship_id);
    if(res != assignments.end()) {
        delete res->second;
        assignments.erase(res);
    }
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
