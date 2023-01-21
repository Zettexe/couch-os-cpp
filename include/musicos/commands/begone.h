#ifndef MUSICOS_BEGONE_H
#define MUSICOS_BEGONE_H

#include "musicos/command.h"

class begone : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("begone", "Thot", event->command.application_id);
  }

  inline void command_definition() override { reply("Thot"); }
};

#endif // MUSICOS_BEGONE_H