#ifndef MUSICOS_PING_H
#define MUSICOS_PING_H

#include "musicos/command.h"

class ping : public command {
public:
  ping(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("ping", "Ping Pong!", bot_id); }

  inline void command_definition() override {
    reply("Pong!");
  }
};

#endif // MUSICOS_PING_H