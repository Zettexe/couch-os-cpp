#include "musicos/command.h"

void command::log_reply(const std::string &message) {
  std::string formatted_message = std::regex_replace(message, std::regex("\n"), "\\n");

  spdlog::info("REPLY [{}]: Command: {}, Message: {}", event->command.get_guild().name, command_interface.name, formatted_message);
}

void command::log_edit(const std::string &message) {
  std::string formatted_message = std::regex_replace(message, std::regex("\n"), "\\n");

  spdlog::info("EDIT REPLY: Guild: {}, Command: {}, Message: {}", event->command.get_guild().name, command_interface.name, formatted_message);
}

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

  spdlog::info("USED COMMAND [{}] ({}): {}", event->command.get_guild().name, event->command.usr.format_username(), cmd.str());

  this->command_preprocess();
  this->command_definition();
}

void command::reply(dpp::command_completion_event_t callback) { event->reply(callback); }

void command::reply(dpp::interaction_response_type type, const dpp::message &message, const std::string &log_message,
                    dpp::command_completion_event_t callback) {
  log_reply(message.content + log_message);
  event->reply(type, message, callback);
}

void command::reply(dpp::interaction_response_type type, const std::string &message, const std::string &log_message, dpp::command_completion_event_t callback) {
  std::string limited_message = limit_message_size(message);
  log_reply(limited_message + log_message);
  event->reply(type, limited_message, callback);
}

void command::reply(const dpp::message &message, const std::string &log_message, dpp::command_completion_event_t callback) {
  log_reply(message.content + log_message);
  event->reply(message, callback);
}

void command::reply(const std::string &message, const std::string &log_message, dpp::command_completion_event_t callback) {
  std::string limited_message = limit_message_size(message);
  log_reply(limited_message + log_message);
  event->reply(limited_message, callback);
}

void command::edit_response(const dpp::message &message, dpp::command_completion_event_t callback) {
  log_edit(message.content);
  event->edit_response(message, callback);
}

void command::edit_response(const std::string &message, dpp::command_completion_event_t callback) {
  std::string limited_message = limit_message_size(message);
  log_edit(limited_message);
  event->edit_response(limited_message, callback);
}