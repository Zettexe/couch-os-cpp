#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"
#include "musicos/player_manager.h"
#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <dpp/appcommand.h>
#include <dpp/colors.h>
#include <dpp/message.h>
#include <exception>
#include <fmt/core.h>
#include <nlohmann/json_fwd.hpp>
#include <spdlog/spdlog.h>
#include <string>

class join : public music_command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("join", "Joins the users voice channel.",
                             event->command.application_id);
  }

  inline void command_definition() override {
    event->thinking(true);
    dpp::guild guild = event->command.get_guild();

    if (!player_manager->connect_voice(event, guild, event->command.usr.id)) {
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
    event->thinking(true);

    if (player_manager->player_is_connected(event->command.guild_id)) {
      player_manager->disconnect_voice(event, event->command.guild_id);
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
#include <dpp/dpp.h>

class play : public music_command {
public:
  play(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("play", "Hand me the aux", bot_id)
                          .add_option(dpp::command_option(dpp::co_string, "query",
                                                          "What do you want me to play?", true));
  }

  // TODO: fix playlists not working
  inline void command_definition() override {
    event->thinking(true);

    dpp::guild guild = event->command.get_guild();

    if (!player_manager->connect_voice(event, guild, event->command.usr.id)) {
      reply("Join a vc ya dingus.");
      return;
    }

    std::vector<dpp::command_data_option> options =
      event->command.get_command_interaction().options;
    std::string input = std::get<std::string>(options[0].value);

    nlohmann::json result = player_manager->fetch_search_result(input);

    if (result.size() == 0) {
      spdlog::warn("Input is not a valid url, trying search...");
      result = player_manager->fetch_search_result("ytsearch5:" + input);
      if (result.size() == 0) {
        spdlog::error("Invalid Input");
        return;
      }

      dpp::component c = dpp::component()
                           .set_type(dpp::cot_selectmenu)
                           .set_placeholder("Select song...")
                           .set_id("play_select");
      for (size_t i = 0; i < result.size(); i += 1) {
        int seconds = result[i]["duration"].get<int>();
        int minutes = seconds / 60;
        int hours = minutes / 60;
        seconds %= 60;
        minutes %= 60;

        std::string duration = fmt::format("{}:{}", minutes, seconds);
        if (hours > 0) {
          duration = fmt::format("{}:{}", hours, duration);
        }

        std::string values =
          fmt::format("{}:{}", result[i]["id"].get<std::string>(), event->command.channel_id);

        c.add_select_option(dpp::select_option(
          result[i]["title"].get<std::string>(), values,
          fmt::format("{} - {}", result[i]["uploader"].get<std::string>(), duration)));
      }
      edit_response(dpp::message().add_component(dpp::component().add_component(c)));
      return;
    }

    player_manager->download(result[0]["id"]);
    player_manager->player_add_queue(result[0], event->command.guild_id);

    event->delete_original_response();
    dpp::message m = dpp::message(event->command.channel_id, "")
                       .add_embed(player_manager->generate_embed(result[0]));
    create_message(m);
  }
};

#endif // MUSICOS_PLAY_H
