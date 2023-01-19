#ifndef MUSICOS_HELLO_H
#define MUSICOS_HELLO_H

#include "musicos/command.h"

class hello : public command {
public:
  hello(dpp::snowflake bot_id) {
    command_interface = dpp::slashcommand("hello", "Says \"Hello!\"", bot_id);
  }

  inline void command_definition() override { reply("Hello!"); }
};

#endif // MUSICOS_HELLO_H