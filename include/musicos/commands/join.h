#ifndef MUSICOS_JOIN_H
#define MUSICOS_JOIN_H

#include "musicos/music_command.h"

class join : public music_command {
public:
  join(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("join", "Joins the users voice channel.", bot_id);
  }

  inline void command_definition() override {

    // if (!) {
    //   reply("Join a vc dingus.");
    //   return;
    // }

    reply("Success!");
  }
};

#endif // MUSICOS_JOIN_H