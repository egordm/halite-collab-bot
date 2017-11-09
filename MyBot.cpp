#include "hlt/hlt.hpp"
#include "bot/BotAlgo.h"

int main() {
    const hlt::Metadata metadata = hlt::initialize("ocdy1001");
    const hlt::PlayerId player_id = metadata.player_id;

    const hlt::Map& initial_map = metadata.initial_map;

    // We now have 1 full minute to analyse the initial map.
    std::ostringstream initial_map_intelligence;
    initial_map_intelligence
            << "width: " << initial_map.map_width
            << "; height: " << initial_map.map_height
            << "; players: " << initial_map.ship_map.size()
            << "; my ships: " << initial_map.ship_map.at(player_id).size()
            << "; planets: " << initial_map.planets.size();
    hlt::Log::log(initial_map_intelligence.str());

    std::vector<hlt::Move> moves;
    bot::BotAlgo bot(player_id);
    while(true) {
        //init
        moves.clear();
        hlt::Map map = hlt::in::get_map();
        //moves
        bot.Moves(map, moves);
        //check for errors
        if (!hlt::out::send_moves(moves)) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }
    }
}
