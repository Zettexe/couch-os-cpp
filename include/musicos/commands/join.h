#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/command.h"

class join : public command {
public:
  join(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("join", "Joins the users voice channel.", bot_id); }

  inline void command_definition() override {
    //   auto p = player_manager->create_player(event.command.guild_id);
    // if (!p->channel_id)
    //   p->set_channel(event.command.channel_id);

    //   int res = join_voice(event.from, player_manager, event.command.guild_id, event.command.usr.id, event.from->creator->me.id);

    //   switch (res) {
    //   case 0:
    //     reply("Joining...");
    //     return;
    //   case 1:
    //     reply("Join a voice channel first you dummy");
    //     return;
    //   case 2:
    //     reply("I'm already in a voice channel");
    //     return;
    //   case 3:
    //     reply("`[ERROR]` No channel to join");
    //     return;
    //   case 4:
    //     reply("I have no permission to join your voice channel");
    //     return;
    //   }

    dpp::guild *g = dpp::find_guild(event->command.guild_id);
    if (!g->connect_member_voice(event->command.usr.id)) {
      reply("You don't seem to be on a voice channel! :(");
    }

    reply("Pong!");
  }
};

#endif // MUSICOS_JOIN_H