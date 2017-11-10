#include <bot.h>
#include "hlt/hlt.hpp"
#include "bot/BotAlgo.h"

int main() {
    const hlt::Metadata metadata = hlt::initialize("Magnetron");
    const hlt::PlayerId player_id = metadata.player_id;

    hlt::Map initial_map = metadata.initial_map;

    bot::Bot bot(player_id, initial_map);

    while(true) {
        hlt::Map map = hlt::in::get_map();
        if (!hlt::out::send_moves(bot.do_step(map))) {
            hlt::Log::log("send_moves failed; exiting");
            break;
        }
    }
}
