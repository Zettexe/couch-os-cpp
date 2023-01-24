#ifndef MUSICOS_MUSIC_COMMAND_H
#define MUSICOS_MUSIC_COMMAND_H

#include "musicos/command.h"
#include "musicos/player_manager.h"

class music_command : public command {
protected:
  static player_manager_c *player_manager;

public:
  static void set_player_manager(player_manager_c *pm) { player_manager = pm; }
};

#endif // MUSICOS_MUSIC_COMMAND_H