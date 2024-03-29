#include "musicos/command.h"

#include "musicos/commands/begone.h"
#include "musicos/commands/celebrate.h"
#include "musicos/commands/crusade_time.h"
#include "musicos/commands/fuck.h"
#include "musicos/commands/god_tier_quote.h"
#include "musicos/commands/hello.h"
#include "musicos/commands/music.h"
#include "musicos/commands/pasta.h"
#include "musicos/commands/ping.h"
#include "musicos/commands/quote.h"
#include "musicos/commands/recycle.h"
#include "musicos/commands/restart.h"
#include "musicos/commands/roll.h"
#include "musicos/commands/shalom.h"
#include "musicos/commands/start.h"
#include "musicos/commands/zen_quote.h"
#include "musicos/music_command.h"

#include "musicos/player_manager.h"

// Define the list of commands as a global variable
std::list<command *> commands;
std::vector<dpp::slashcommand> commands_vector;

// Add an instance of the derived class to the list of commands in a function
void initialize_commands(dpp::snowflake bot_id, player_manager_c *player_manager) {
  commands.push_back(new begone(bot_id));
  commands.push_back(new celebrate(bot_id));
  commands.push_back(new crusade_time(bot_id));
  commands.push_back(new fuck(bot_id));
  commands.push_back(new god_tier_quote(bot_id));
  commands.push_back(new hello(bot_id));
  commands.push_back(new pasta(bot_id));
  commands.push_back(new ping(bot_id));
  commands.push_back(new quote(bot_id));
  commands.push_back(new recycle(bot_id));
  commands.push_back(new restart(bot_id));
  commands.push_back(new roll(bot_id));
  commands.push_back(new shalom(bot_id));
  commands.push_back(new start(bot_id));
  commands.push_back(new zen_quote(bot_id));
  commands.push_back(new join(bot_id));
  commands.push_back(new leave(bot_id));
  commands.push_back(new play(bot_id));
  commands.push_back(new skip(bot_id));
  commands.push_back(new class pause(bot_id));
  commands.push_back(new queue(bot_id));
  commands.push_back(new stop(bot_id));
  commands.push_back(new loop(bot_id));

  for (command *command : commands) {
    commands_vector.push_back(command->command_interface);
    music_command *t = dynamic_cast<music_command *>(command);
    if (t != nullptr) {
      t->set_player_manager(player_manager);
    }
  }
}

player_manager_c *music_command::player_manager;