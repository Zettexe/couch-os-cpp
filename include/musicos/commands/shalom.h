#ifndef MUSICOS_SHALOM_H
#define MUSICOS_SHALOM_H

#include "musicos/command.h"
#include <dpp/appcommand.h>
#include <dpp/dpp.h>
#include <string>

class shalom : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("shalom", "Hello but in Jewish", event->command.application_id);
  }

  inline void command_definition() override { reply("Greetings and may peace be upon you!"); }
};

#endif // MUSICOS_SHALOM_H