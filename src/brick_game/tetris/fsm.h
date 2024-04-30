#ifndef FSM_H
#define FSM_H

#include "tetris_backend.h"

GameInfoExtended_t *get_current_game_inf_ex();
void finite_state_machine_func(GameInfoExtended_t *game_inf_ex);

#endif
