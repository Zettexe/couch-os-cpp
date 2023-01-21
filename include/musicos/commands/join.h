#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"

class join : public music_command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("join", "Joins the users voice channel.",
                             event->command.application_id);
  }

  inline void command_definition() override {
    event->thinking();
    join_voice();

    if (!player->connected) {
      reply("Join a vc ya dingus.");
      return;
    }
    event->delete_original_response();
  }
};

#endif // MUSICOS_JOIN_H

#ifndef MUSICOS_LEAVE_H
#define MUSICOS_LEAVE_H

#include "musicos/music_command.h"

class leave : public music_command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("leave", "Leaves voice channel if connected.",
                             event->command.application_id);
  }

  inline void command_definition() override {
    event->thinking();

    if (player->connected) {
      event->from->disconnect_voice(event->command.guild_id);
      player->connected = false;
      event->delete_original_response();
      return;
    }

    reply("Not connected to a voice channel.");
  }
};

#endif // MUSICOS_LEAVE_H

#ifndef MUSICOS_PLAY_H
#define MUSICOS_PLAY_H

#include "musicos/music_command.h"

class play : public music_command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("play", "Play some music.", event->command.application_id);
  }

  inline void command_definition() override {
    event->thinking();

    if (player->connected) {
      event->from->disconnect_voice(event->command.guild_id);
      player->connected = false;
      event->delete_original_response();
      return;
    }

    reply("Not connected to a voice channel.");
  }
};

#endif // MUSICOS_PLAY_H