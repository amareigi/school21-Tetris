#ifndef C_TETRIS_BACKEND_H_
#define C_TETRIS_BACKEND_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "tetris_lib.h"

#define ROWS_MAP 20
#define COLS_MAP 10
#define NEXT_SIZE 4

typedef enum {
  START = 0,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER,
  PAUSE,
  EXIT_STATE
} State;

typedef struct {
  int brick[4][4];
  int x;
  int y;
  char current_type;
  char next_type;
} Tetromino;

typedef struct {
  GameInfo_t game_inf;
  Tetromino tetromino;
  UserAction_t input;
  int move_field[20][10];
  int touched_field[20][10];
  bool new_input;
  State state;
  unsigned long long timer;
  bool timer_activated;
  bool mem_alloc;
  bool high_score_readed;
} GameInfoExtended_t;

void prepare_game_field(GameInfoExtended_t *game_inf_ex);
void restart_func(GameInfoExtended_t *game_inf_ex);
void init_func(GameInfoExtended_t *game_inf_ex);
void generate_next_func(GameInfoExtended_t *game_inf_ex);
bool check_game_over(GameInfoExtended_t *game_inf_ex);
void next_to_tetromino(GameInfoExtended_t *game_inf_ex);
void brick_to_move_field(GameInfoExtended_t *game_inf_ex);
void action_func(GameInfoExtended_t *game_inf_ex);
void move_brick_right(GameInfoExtended_t *game_inf_ex);
void move_brick_left(GameInfoExtended_t *game_inf_ex);
bool check_move_to_shift_func(GameInfoExtended_t *game_inf_ex);
bool check_shifting_to_attaching(GameInfoExtended_t *game_inf_ex);
void move_brick(GameInfoExtended_t *game_inf_ex);
void move_from_move_to_touch(GameInfoExtended_t *game_inf_ex);
void count_game_score(GameInfoExtended_t *game_inf_ex);
void dealloc_field(int ***field, int rows);

#endif
