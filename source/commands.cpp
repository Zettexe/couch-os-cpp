#include "musicos/command.h"

#include "musicos/commands/begone.h"
#include "musicos/commands/celebrate.h"
#include "musicos/commands/crusade_time.h"
#include "musicos/commands/fuck.h"
#include "musicos/commands/god_tier_quote.h"
#include "musicos/commands/hello.h"
#include "musicos/commands/join.h"
#include "musicos/commands/pasta.h"
#include "musicos/commands/ping.h"
#include "musicos/commands/quote.h"
#include "musicos/commands/recycle.h"
#include "musicos/commands/restart.h"
#include "musicos/commands/roll.h"
#include "musicos/commands/shalom.h"
#include "musicos/commands/start.h"
#include "musicos/commands/zen_quote.h"

#include <dpp/dpp.h>
#include <string>

// Define the list of commands as a global variable
std::list<command *> commands;
std::vector<dpp::slashcommand> commands_vector;

// Add an instance of the derived class to the list of commands in a function
void initialize_commands() {
  commands.push_back(new begone);
  commands.push_back(new celebrate);
  commands.push_back(new crusade_time);
  commands.push_back(new fuck);
  commands.push_back(new god_tier_quote);
  commands.push_back(new hello);
  commands.push_back(new pasta);
  commands.push_back(new ping);
  commands.push_back(new quote);
  commands.push_back(new recycle);
  commands.push_back(new restart);
  commands.push_back(new roll);
  commands.push_back(new shalom);
  commands.push_back(new start);
  commands.push_back(new zen_quote);
  commands.push_back(new join);
  commands.push_back(new leave);

  for (command *command : commands) {
    commands_vector.push_back(command->register_command());
  }
}