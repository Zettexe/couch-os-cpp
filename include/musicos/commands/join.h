#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"

class join : public music_command {
public:
  join(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("join", "Joins the users voice channel.", bot_id);
  }

  inline void command_definition() override {
    join_voice();

    if (!player->connected) {
      reply("Join a vc ya dingus.");
    }
  }
};

#endif // MUSICOS_JOIN_H

#ifndef MUSICOS_LEAVE_H
#define MUSICOS_LEAVE_H

#include "musicos/music_command.h"

class leave : public music_command {
public:
  leave(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("leave", "Leaves voice channel if connected.", bot_id);
  }

  inline void command_definition() override {
    if (player->connected) {
      player->voice_connection->disconnect();
      return;
    }

    reply("Not in a voice channel.");
  }
};

#endif // MUSICOS_LEAVE_H