#include "fsm.h"

static void start_func(GameInfoExtended_t *game_inf_ex);
static void spawn_func(GameInfoExtended_t *game_inf_ex);
static void moving_func(GameInfoExtended_t *game_inf_ex);
static void pause_func(GameInfoExtended_t *game_inf_ex);
static void shifting_func(GameInfoExtended_t *game_inf_ex);
static void attaching_func(GameInfoExtended_t *game_inf_ex);
static void game_over_func(GameInfoExtended_t *game_inf_ex);
static void exit_func(GameInfoExtended_t *game_inf_ex);

GameInfoExtended_t *get_current_game_inf_ex() {
  static GameInfoExtended_t game_inf_ex = {0};
  return &game_inf_ex;
}

void finite_state_machine_func(GameInfoExtended_t *game_inf_ex) {
  switch (game_inf_ex->state) {
    case START:
      start_func(game_inf_ex);
      break;
    case SPAWN:
      spawn_func(game_inf_ex);
      break;
    case MOVING:
      moving_func(game_inf_ex);
      break;
    case PAUSE:
      pause_func(game_inf_ex);
      break;
    case SHIFTING:
      shifting_func(game_inf_ex);
      break;
    case ATTACHING:
      attaching_func(game_inf_ex);
      break;
    case GAMEOVER:
      game_over_func(game_inf_ex);
      break;
    case EXIT_STATE:
      exit_func(game_inf_ex);
      break;
    default:
      break;
  }
  prepare_game_field(game_inf_ex);
}

static void start_func(GameInfoExtended_t *game_inf_ex) {
  if (game_inf_ex->new_input) {
    switch (game_inf_ex->input) {
      case Start:
        if (game_inf_ex->game_inf.pause == 2) {  // после game_over pause = 2
          restart_func(game_inf_ex);
        }
        init_func(game_inf_ex);
        generate_next_func(game_inf_ex);
        game_inf_ex->state = SPAWN;
        break;
      case Terminate:
        game_inf_ex->state = EXIT_STATE;
        break;
      default:
        break;
    }
  }
  game_inf_ex->new_input = false;
}

static void spawn_func(GameInfoExtended_t *game_inf_ex) {
  next_to_tetromino(game_inf_ex);
  generate_next_func(game_inf_ex);
  brick_to_move_field(game_inf_ex);
  game_inf_ex->state = MOVING;
}

static void moving_func(GameInfoExtended_t *game_inf_ex) {
  if (game_inf_ex->new_input) switch (game_inf_ex->input) {
      case Action:
        action_func(game_inf_ex);
      case Up:
        break;
      case Down:
        game_inf_ex->state = SHIFTING;
        break;
      case Right:
        move_brick_right(game_inf_ex);
        break;
      case Left:
        move_brick_left(game_inf_ex);
        break;
      case Terminate:
        game_inf_ex->state = EXIT_STATE;
        break;
      case Pause:
        game_inf_ex->state = PAUSE;
        game_inf_ex->game_inf.pause = 1;
        break;
      default:
        break;
    }
  if (check_move_to_shift_func(game_inf_ex)) {
    game_inf_ex->state = SHIFTING;
  }
  game_inf_ex->new_input = false;
}

static void pause_func(GameInfoExtended_t *game_inf_ex) {
  if (game_inf_ex->new_input) {
    switch (game_inf_ex->input) {
      case Pause:
        game_inf_ex->game_inf.pause = 0;
        game_inf_ex->state = MOVING;
        break;
      case Terminate:
        game_inf_ex->state = EXIT_STATE;
        break;
      default:
        break;
    }
  }
  game_inf_ex->new_input = false;
}

static void shifting_func(GameInfoExtended_t *game_inf_ex) {
  if (check_shifting_to_attaching(game_inf_ex)) {
    game_inf_ex->state = ATTACHING;
  } else {
    move_brick(game_inf_ex);
    game_inf_ex->state = MOVING;
  }
}

static void attaching_func(GameInfoExtended_t *game_inf_ex) {
  move_from_move_to_touch(game_inf_ex);
  count_game_score(game_inf_ex);
  if (check_game_over(game_inf_ex))
    game_inf_ex->state = GAMEOVER;
  else {
    game_inf_ex->state = SPAWN;
  }
}

static void game_over_func(GameInfoExtended_t *game_inf_ex) {
  game_inf_ex->state = START;
  game_inf_ex->game_inf.pause = 2;  // game_over
}

static void exit_func(GameInfoExtended_t *game_inf_ex) {
  dealloc_field(&(game_inf_ex->game_inf.field), ROWS_MAP);
  dealloc_field(&(game_inf_ex->game_inf.next), NEXT_SIZE);
  game_inf_ex->game_inf.pause = 3;  // terminate app
}
