#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"
#include "musicos/player_manager.h"
#include <deque>
#include <dpp/appcommand.h>
#include <dpp/message.h>
#include <tuple>

class join : public music_command {
public:
  join(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("join", "Joins the users voice channel.", bot_id);
  }

  inline void command_definition() override {
    dpp::guild guild = event->command.get_guild();

    if (!player_manager->connect_voice(event, guild, event->command.usr.id)) {
      reply("Join a voice channel first.", true);
      return;
    }

    reply("Connected to voice channel.", true);
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
    if (player_manager->player_is_connected(event->command.guild_id)) {
      player_manager->disconnect_voice(event, event->command.guild_id);
      reply("Disconnected from voice channel.", true);
      return;
    }

    reply("Not connected to a voice channel.", true);
  }
};

#endif // MUSICOS_LEAVE_H

#ifndef MUSICOS_STOP_H
#define MUSICOS_STOP_H

#include "musicos/music_command.h"

class stop : public music_command {
public:
  stop(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("stop", "Stops the player and clears queue.", bot_id);
  }

  inline void command_definition() override {
    player_manager->player_clear_queue(event->command.guild_id);
    reply("Stopped Player.", true);
  }
};

#endif // MUSICOS_STOP_H

#ifndef MUSICOS_PLAY_H
#define MUSICOS_PLAY_H

#include "musicos/music_command.h"

class play : public music_command {
public:
  play(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("play", "Hand me the aux", bot_id)
                          .add_option(dpp::command_option(dpp::co_string, "query",
                                                          "What do you want me to play?", false));
  }

  inline void command_definition() override {
    event->thinking(true);

    dpp::guild guild = event->command.get_guild();

    std::vector<dpp::command_data_option> options =
      event->command.get_command_interaction().options;

    if (options.empty() || std::get<std::string>(options[0].value) == "") {
      if (player_manager->player_get_queue(guild.id)->empty()) {
        edit_response("Nothing is paused or you forgot to input a query.");
        return;
      }

      player_manager->player_pause(false, guild.id);
      edit_response("Player unpaused.");
      return;
    }

    if (!player_manager->connect_voice(event, guild, event->command.usr.id)) {
      edit_response("Join a voice channel first.");
      return;
    }

    std::string input = std::get<std::string>(options[0].value);

    nlohmann::json result = player_manager->fetch_search_result(input);

    if (result.empty()) {
      spdlog::warn("Input is not a valid url, trying search...");
      result = player_manager->fetch_search_result("ytsearch5:" + input);
      if (result.empty()) {
        spdlog::error("Invalid Input");
        edit_response("Invalid input.");
        return;
      }

      dpp::component component = dpp::component()
                                   .set_type(dpp::cot_selectmenu)
                                   .set_placeholder("Select song...")
                                   .set_id("play_select");
      for (size_t i = 0; i < result.size(); i += 1) {
        std::string duration =
          player_manager->get_formatted_timestamp(result[i]["duration"].get<int>());

        std::string values =
          fmt::format("{}:{}", result[i]["id"].get<std::string>(), event->command.channel_id);

        component.add_select_option(dpp::select_option(
          result[i]["title"].get<std::string>(), values,
          fmt::format("{} - {}", result[i]["uploader"].get<std::string>(), duration)));
      }

      edit_response(dpp::message().add_component(dpp::component().add_component(component)));
      return;
    }

    for (nlohmann::json video : result) {
      player_manager->add_to_queue(video, event->command.guild_id);
    }

    event->delete_original_response();
    dpp::message m = dpp::message(event->command.channel_id, "")
                       .add_embed(player_manager->generate_embed(result, "Added to queue:"));
    create_message(m);
  }
};

#endif // MUSICOS_PLAY_H

#ifndef MUSICOS_SKIP_H
#define MUSICOS_SKIP_H

#include "musicos/music_command.h"

class skip : public music_command {
public:
  skip(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("skip", "Skip the currently playing song.", bot_id);
  }

  inline void command_definition() override {
    dpp::snowflake guild_id = event->command.guild_id;

    player_manager->player_skip(guild_id);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (player_manager->player_paused(guild_id)) {
      player_manager->player_pause(false, guild_id);
    }
    player_manager->player_pop_queue(guild_id);

    player_queue *queue = player_manager->player_get_first_queue(guild_id);
    reply(fmt::format("Skipped to [{}]()", queue->video_data["title"],
                      queue->video_data["webpage_url"]),
          true);
  }
};

#endif // MUSICOS_SKIP_H

#ifndef MUSICOS_PAUSE_H
#define MUSICOS_PAUSE_H

#include "musicos/music_command.h"

class pause : public music_command {
public:
  pause(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("pause", "Pause the player.", bot_id);
  }

  inline void command_definition() override {
    bool paused = player_manager->player_paused(event->command.guild_id);
    player_manager->player_pause(!paused, event->command.guild_id);

    if (!paused) {
      reply("Player paused.", true);
    } else {
      reply("Player unpaused.", true);
    }
  }
};

#endif // MUSICOS_PAUSE_H

#ifndef MUSICOS_QUEUE_H
#define MUSICOS_QUEUE_H

#include "musicos/music_command.h"

class queue : public music_command {
public:
  queue(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("queue", "Display the current queue.", bot_id);
  }

  inline void command_definition() override {
    std::tuple<bool, dpp::embed> result =
      player_manager->generate_queue_embed(event->command.channel_id);

    if (std::get<0>(result)) {
      reply("Nothing is playing.", true);
      return;
    }

    reply(dpp::message().add_embed(std::get<1>(result)), true);
  }
};

#endif // MUSICOS_QUEUE_H

#ifndef MUSICOS_LOOP_H
#define MUSICOS_LOOP_H

#include "musicos/music_command.h"

class loop : public music_command {
public:
  loop(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("loop", "Looper is a movie I have not watched.", bot_id)
                          .add_option(dpp::command_option(dpp::co_number, "times",
                                                          "How many times we spinning?", false));
  }

  inline void command_definition() override {
    std::vector<dpp::command_data_option> options =
      event->command.get_command_interaction().options;

    int times = 0;

    if (!options.empty()) {
      times = std::get<double>(options[0].value);
    }

    if (times > 0) {
      player_manager->player_set_looping(times, event->command.guild_id);
      reply(fmt::format("We spinning {} times today, eh?", times), true);
      return;
    }

    int looping = player_manager->player_get_looping(event->command.guild_id);
    if (looping > 0) {
      player_manager->player_set_looping(0, event->command.guild_id);
      reply("No loops sadge :(", true);
      return;
    }

    player_manager->player_set_looping('*', event->command.guild_id);
    reply("We loopin bby >:)", true);
  }
};

#endif // MUSICOS_LOOP_H
