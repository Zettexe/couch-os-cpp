#ifndef MUSICOS_START_H
#define MUSICOS_START_H

#include "musicos/command.h"
#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <string>

class start : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("start", "\"Starts\" CouchOS", event->command.application_id);
  }

  inline void command_definition() override { reply("Welcome to CouchOS, please imput command_!"); }
};

#endif // MUSICOS_START_H