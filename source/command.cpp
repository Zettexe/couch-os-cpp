#include "musicos/command.h"
#include <dpp/cache.h>
#include <dpp/dispatcher.h>
#include <dpp/message.h>
#include <spdlog/spdlog.h>

dpp::cluster *command::bot;

void command::log_generic(const std::string &message, const std::string &source) {
  std::string formatted_message = std::regex_replace(message, std::regex("\n"), "\\n");

  std::string guild_name;
  if (event) {
    guild_name = event->command.get_guild().name;
  } else {
    guild_name = dpp::find_guild(message_event->msg.guild_id)->name;
  }

  spdlog::info("{} [{}]: Command: {}, Message: {}", source, guild_name, register_command().name,
               formatted_message);
}

void command::log_reply(const std::string &message) { log_generic(message, "REPLY"); }

void command::log_edit(const std::string &message) { log_generic(message, "EDIT REPLY"); }

std::string command::to_string(const dpp::command_value &value) {
  std::string result;

  std::visit(
    [&result](auto &&arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, std::monostate>) {
        result = "Invalid";
      } else if constexpr (std::is_same_v<T, bool>) {
        result = arg ? "true" : "false";
      } else if constexpr (std::is_same_v<T, std::string>) {
        result = arg;
      } else {
        result = std::to_string(arg);
      }
    },
    value);

  return result;
}

void command::execute(dpp::slashcommand_t &e) {
  event = &e;
  dpp::command_interaction command_interaction = event->command.get_command_interaction();

  std::stringstream cmd;

  cmd << command_interaction.name;

  for (size_t i = 0; i < command_interaction.options.size(); ++i) {
    const dpp::command_data_option &option = command_interaction.options[i];
    cmd << " " << option.name;

    if (!(option.type == dpp::co_sub_command || option.type == dpp::co_sub_command_group)) {
      cmd << ":" << to_string(option.value);
      continue;
    }

    for (const dpp::command_data_option &option2 : option.options) {
      if (option2.type == dpp::co_sub_command) {
        cmd << " " << option2.name;
      } else {
        cmd << ":" << to_string(option2.value);
      }
    }
  }

  spdlog::info("USED COMMAND [{}] ({}): {}", event->command.get_guild().name,
               event->command.usr.format_username(), cmd.str());

  this->command_preprocess();
  this->command_definition();
  this->command_postprocess();

  event = nullptr;
}

void command::execute(dpp::message_create_t &e, std::string command_name) {
  message_event = &e;

  spdlog::info("USED COMMAND [{}] ({}): {}", dpp::find_guild(message_event->msg.guild_id)->name,
               message_event->msg.author.format_username(), command_name);

  this->command_preprocess();
  this->command_definition();
  this->command_postprocess();

  message_event = nullptr;
}

void command::reply(dpp::interaction_response_type type, const dpp::message &message,
                    const std::string &log_message) {
  log_reply(message.content + log_message);
  event->reply(type, message);
}

void command::reply(dpp::interaction_response_type type, const std::string &message,
                    const std::string &log_message) {
  std::string limited_message = limit_message_size(message);
  log_reply(limited_message + log_message);
  event->reply(type, limited_message);
}

void command::reply(dpp::message &message, const std::string &log_message) {
  log_reply(message.content + log_message);
  if (message_event) {
    message_event->reply(message);
    return;
  }
  event->reply(message);
}

void command::reply(const std::string &message, const std::string &log_message) {
  std::string limited_message = limit_message_size(message);
  log_reply(limited_message + log_message);
  if (message_event) {
    message_event->reply(message);
    return;
  }
  event->reply(limited_message);
}

void command::edit_response(const dpp::message &message) {
  log_edit(message.content);
  event->edit_response(message);
}

void command::edit_response(const std::string &message) {
  std::string limited_message = limit_message_size(message);
  log_edit(limited_message);
  event->edit_response(limited_message);
}

void command::create_message(const dpp::message &message) {
  log_generic(message.content, "MESSAGE CREATED");
  bot->message_create(message);
}

void command::create_message(const std::string &message, const dpp::snowflake &channel_id) {
  log_generic(message, "MESSAGE CREATED");
  bot->message_create(dpp::message(channel_id, message));
}

void command::log(const std::string &message) { spdlog::info(message); }
template <typename... Args> void command::log(fmt::format_string<Args...> fmt, Args &&...args) {
  spdlog::info(fmt, std::forward<Args>(args)...);
}
