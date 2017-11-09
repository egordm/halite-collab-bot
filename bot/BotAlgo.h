//
// Created by Cody on 09/11/2017.
//
#ifndef MYBOT_BOTALGO_H
#define MYBOT_BOTALGO_H

#include "hlt/hlt.hpp"
#include "bot/navigation.h"
#include "types.hpp"
#include "hlt/navigation.hpp"

namespace bot{
    class BotAlgo{
    private:
        int round;
        hlt::PlayerId id;
        std::vector<hlt::Planet> emptyPlanets, ourPlanets, enemyPlanets;
        std::unordered_map<unsigned int, hlt::Vector> prevShipPos, shipSpeeds;
        void SortPlanets(const hlt::Map& map){
            emptyPlanets.clear();
            ourPlanets.clear();
            enemyPlanets.clear();
            for (const hlt::Planet &planet : map.planets) {
                if(planet.owned == false)
                    emptyPlanets.push_back(planet);
                else if(planet.owner_id == id)
                    ourPlanets.push_back(planet);
                else
                    enemyPlanets.push_back(planet);
            }
        }
        void RecordShipSpeed(const hlt::Map& map){
            for(const auto pair : map.ships){
                for(const auto ship : pair.second){
                    shipSpeeds[ship.entity_id] = prevShipPos[ship.entity_id] - ship.pos;
                }
            }
            for(const auto pair : map.ships){
                for(const auto ship : pair.second){
                    prevShipPos[ship.entity_id] = ship.pos;
                }
            }
        }
        hlt::Planet ClosestPlanet(std::vector<hlt::Planet>& source, hlt::Vector pos){
            double min = 100000000;
            hlt::Planet minp;
            for(const auto planet : source){
                hlt::Vector diff = planet.pos - pos;
                double len = diff.length();
                if(len < min){
                    min = len;
                    minp = planet;
                }
            }
            return minp;
        }
    public:
        BotAlgo(hlt::PlayerId id){
            this->id = id;
            round = 0;
        }
        void Moves(hlt::Map& map, std::vector<hlt::Move>& moves){
            SortPlanets(map);
            RecordShipSpeed(map);
            for(const auto ship : map.ships.at(id)){
                //hlt::Planet target = ClosestPlanet(emptyPlanets, ship.pos);
                //hlt::Vector gotoPos = target.pos;
                hlt::Move move = bot::navigation::move_towards(map, ship, hlt::Vector(0, 0), false);
                moves.push_back(move);
            }
            round++;
        }
    };
}

#endif //MYBOT_BOTALGO_H