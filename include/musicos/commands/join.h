#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"
#include "musicos/player_manager.h"
#include <chrono>

class join : public music_command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("join", "Joins the users voice channel.",
                             event->command.application_id);
  }

  inline void command_definition() override {
    event->thinking();
    dpp::guild guild = event->command.get_guild();
    bool connected = player_manager->connect_voice(event, guild, event->command.usr.id);

    if (!connected) {
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

    if (player_manager->player_is_connected(event->command.guild_id)) {
      player_manager->disconnect_voice(event, event->command.guild_id);
      event->delete_original_response();
      return;
    }

    reply("Not connected to a voice channel.");
  }
};

#endif // MUSICOS_LEAVE_H

// #ifndef MUSICOS_PLAY_H
// #define MUSICOS_PLAY_H

// #include "musicos/music_command.h"
// #include <dpp/dpp.h>

// class play : public music_command {
// public:
//   play(dpp::snowflake bot_id) {
//     command_interface = dpp::slashcommand("play", "Hand me the aux", bot_id);
//   }

//   inline void command_definition() override {
//     event->thinking();
//     dpp::guild = event->command.get_guild(event->command.guild_id);
//     player_manager.join_voice(event, , event->command.usr.id);
//     join_voice();

//     if (!player->connected) {
//       reply("Join a vc ya dingus.");
//       return;
//     }

//     // while (!player->voice_connection->voiceclient->is_ready()) {
//     //   sleep(std::chrono::milliseconds(100));
//     // }

//     // stream("file here");
//   }
// };

// #endif // MUSICOS_PLAY_H
