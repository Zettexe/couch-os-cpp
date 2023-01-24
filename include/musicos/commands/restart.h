#ifndef MUSICOS_RESTART_H
#define MUSICOS_RESTART_H

#include "musicos/command.h"

class restart : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("restart", "Restart, Reboot, Reload", event->command.application_id);
  }

  inline void command_definition() override {
    std::string message = "Restarting";
    reply(message);
    for (char c : "...") {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      message += c;
      edit_response(message);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    message = "Loading";
    edit_response(message);
    for (char c : "...") {
      std::this_thread::sleep_for(std::chrono::milliseconds(250));
      message += c;
      edit_response(message);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    edit_response("Welcome to CouchOS, please imput command_!");
  }
};

#endif // MUSICOS_RESTART_H