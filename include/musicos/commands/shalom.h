#ifndef MUSICOS_SHALOM_H
#define MUSICOS_SHALOM_H

#include "musicos/command.h"
#include <dpp/dpp.h>
#include <string>

class shalom : public command {
public:
  shalom(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("shalom", "Hello but in Jewish", bot_id); }

  inline void command_definition() override { reply("Greetings and may peace be upon you!"); }
};

#endif // MUSICOS_SHALOM_H