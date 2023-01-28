#ifndef MUSICOS_BEGONE_H
#define MUSICOS_BEGONE_H

#include "musicos/command.h"

class begone : public command {
public:
  begone(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("begone", "Thot", bot_id); }

  inline void command_definition() override { reply("Thot"); }
};

#endif // MUSICOS_BEGONE_H