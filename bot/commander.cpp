//
// Created by egordm on 13-11-2017.
//

#include <algorithm>
#include "../hlt/log.hpp"
#include "commander.h"
#include "utils.h"


bot::Commander::~Commander() {
	delete navigator;
}

std::vector<hlt::Move> bot::Commander::command() {
	//TODO: split into clean, assign, produce moves
	assignments.clear();
	utils::erase_if(assignments, [this](std::pair<hlt::EntityId, std::shared_ptr<Assignment>> ass) {
		return !ass.second->is_valid(observer);
	});

	for (const auto &ship : observer.get_my_ships()) {
		if (ship->docking_status != hlt::ShipDockingStatus::Undocked) continue;
		assign(ship);
	}

	std::vector<hlt::Move> ret;
	for (const auto &kv : assignments) {
#if DEBUG_BOT
		hlt::Log::log(kv.second->str());
#endif
		ret.push_back(kv.second->move(observer, navigator));
	}

	return ret;
}

bool bot::Commander::add_assignment(const std::shared_ptr<bot::Assignment> &assignment) {
	if (assignment != nullptr && !assignment->is_valid(observer)) return false; // Als if exceeds assignment count
	unsigned int assigned_count = 0;
	for(const auto &ass : assignments) {
		if( assignment->is_same(ass.second)) assigned_count++;
	}
	if(assigned_count >= assignment->max_count(observer)) return false;

	assignments.insert(std::make_pair(assignment->get_ship()->entity_id, assignment));
	return true;
}

void bot::JuniorCommander::assign(const std::shared_ptr<hlt::Ship> &ship) {
	auto empty_planets = observer.get_planets(hlt::empty_mask + hlt::friendly_mask); // TODO: check adjacent
	while (!empty_planets.empty()) {
		auto planet = utils::closest_object(empty_planets, ship->pos);
		if (!planet.second) continue;
		utils::erase(empty_planets, planet.first);

		if(add_assignment(std::make_shared<ColonizeAssignment>(ship, planet.first))) return;
	}

	auto enemy_planets = observer.get_planets(hlt::empty_mask);
	while (!enemy_planets.empty()) {
		auto planet = utils::closest_object(enemy_planets, ship->pos);
		if (!planet.second) continue;
		utils::erase(enemy_planets, planet.first);

		if(add_assignment(std::make_shared<AttackPlanetAssignment>(ship, planet.first))) return;
	}

	auto enemy = utils::closest_object(observer.get_enemies(), ship->pos);
	if (enemy.second) add_assignment(std::make_shared<AttackShipAssignment>(ship, enemy.first));
}

void bot::StrongerCommander::assign(const std::shared_ptr<hlt::Ship> &ship) {
	auto planets = observer.get_planets(hlt::all_mask);
	while (!planets.empty()) {
		auto planet = utils::closest_object(planets, ship->pos);
		if (!planet.second) continue;
		utils::erase(planets, planet.first);

		if((hlt::empty_mask + hlt::friendly_mask) & planet.first->owner_mask(observer.my_id)) {
			if(add_assignment(std::make_shared<ColonizeAssignment>(ship, planet.first))) return;
		} else {
			if(add_assignment(std::make_shared<AttackPlanetAssignment>(ship, planet.first))) return;
		}
	}

	auto enemy = utils::closest_object(observer.get_enemies(), ship->pos);
	if (enemy.second) add_assignment(std::make_shared<AttackShipAssignment>(ship, enemy.first));
}
