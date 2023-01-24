#ifndef MUSICOS_CELEBRATE_H
#define MUSICOS_CELEBRATE_H

#include "musicos/command.h"

class celebrate : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("celebrate", "Party woo!", event->command.application_id);
  }

  inline void command_definition() override { reply("🎉🎉🎉🎉🎊🎊🎊🎊🎈🎈🎈🧁🧁🥂"); }
};

#endif // MUSICOS_CELEBRATE_H