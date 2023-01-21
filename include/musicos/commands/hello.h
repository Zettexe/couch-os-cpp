#ifndef MUSICOS_HELLO_H
#define MUSICOS_HELLO_H

#include "musicos/command.h"

class hello : public command {
public:
  dpp::slashcommand register_command() override {
    return dpp::slashcommand("hello", "Says \"Hello!\"", event->command.application_id);
  }

  inline void command_definition() override { reply("Hello!"); }
};

#endif // MUSICOS_HELLO_H