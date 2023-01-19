#ifndef MUSICOS_CELEBRATE_H
#define MUSICOS_CELEBRATE_H

#include "musicos/command.h"

class celebrate : public command {
public:
  celebrate(dpp::snowflake bot_id) { command_interface = dpp::slashcommand("celebrate", "Party woo!", bot_id); }

  inline void command_definition() override { reply("🎉🎉🎉🎉🎊🎊🎊🎊🎈🎈🎈🧁🧁🥂"); }
};

#endif // MUSICOS_CELEBRATE_H