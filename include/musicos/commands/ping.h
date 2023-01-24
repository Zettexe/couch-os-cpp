#ifndef MUSICOS_PING_H
#define MUSICOS_PING_H

#include "musicos/command.h"

class ping : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("ping", "Ping Pong!", event->command.application_id);
  }

  inline void command_definition() override { reply("Pong!"); }
};

#endif // MUSICOS_PING_H