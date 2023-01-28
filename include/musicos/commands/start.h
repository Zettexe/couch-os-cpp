#ifndef MUSICOS_START_H
#define MUSICOS_START_H

#include "musicos/command.h"

class start : public command {
public:
  start(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("start", "\"Starts\" CouchOS", bot_id);
  }

  inline void command_definition() override { reply("Welcome to CouchOS, please imput command_!"); }
};

#endif // MUSICOS_START_H