#include "tetris_lib.h"

#include "fsm.h"

void userInput(UserAction_t user_input, bool hold) {
  (void)hold;
  GameInfoExtended_t *game_inf_ex = get_current_game_inf_ex();
  if (!game_inf_ex->new_input) {
    game_inf_ex->input = user_input;
    game_inf_ex->new_input = true;
  }
}

GameInfo_t updateCurrentState() {
  GameInfoExtended_t *game_inf_ex = get_current_game_inf_ex();
  finite_state_machine_func(game_inf_ex);

  return game_inf_ex->game_inf;
}
