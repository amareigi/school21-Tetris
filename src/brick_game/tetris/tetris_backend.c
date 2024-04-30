#include "tetris_backend.h"

static void fill_zero_func(int **field, int rows, int cols);
static void alloc_field_func(int ***field, int rows, int cols);
static unsigned long long current_time_millis();
static void reset_static_field(int move_field[][COLS_MAP]);
static void fill_next_func(GameInfo_t *game_inf, int next[][4]);
static bool check_collision_bottom(int move_field[][COLS_MAP]);
static bool check_collision_left(GameInfoExtended_t *game_inf_ex);
static bool check_collision_right(GameInfoExtended_t *game_inf_ex);
static bool check_collision_tetromino(int move_field[][COLS_MAP],
                                      int touched_field[][COLS_MAP]);
static void delete_filled_line(int touched_field[][COLS_MAP], int line);
static int check_filled_line(int touched_field[][COLS_MAP]);
static void cp_tetr_to_tetr(Tetromino *dst, Tetromino *src);
static void turn_stick_func(Tetromino *tetromino);
static void turn_other_func(Tetromino *tetromino);
static bool check_tetr_collision_after_action(GameInfoExtended_t *game_inf_ex);
static void check_y_after_action(GameInfoExtended_t *game_inf_ex);
static bool check_x_after_action(GameInfoExtended_t *game_inf_ex);
static void set_lvl_func(GameInfoExtended_t *game_inf_ex);
static void save_max_score(GameInfoExtended_t *game_inf_ex);
static void load_max_score(GameInfoExtended_t *game_inf_ex);
static unsigned long long time_diff_func(unsigned long long timer);

void prepare_game_field(GameInfoExtended_t *game_inf_ex) {
  if (game_inf_ex->game_inf.field != NULL) {
    for (int i = 0; i < ROWS_MAP; i++)
      for (int j = 0; j < COLS_MAP; j++)
        if (game_inf_ex->move_field[i][j] == 1 ||
            game_inf_ex->touched_field[i][j] == 1)
          game_inf_ex->game_inf.field[i][j] = 1;
        else
          game_inf_ex->game_inf.field[i][j] = 0;
  }
}

void restart_func(GameInfoExtended_t *game_inf_ex) {
  reset_static_field(game_inf_ex->move_field);
  reset_static_field(game_inf_ex->touched_field);
  game_inf_ex->game_inf.score = 0;
  game_inf_ex->game_inf.pause = 0;
  game_inf_ex->game_inf.level = 1;
}

void init_func(GameInfoExtended_t *game_inf_ex) {
  if (!game_inf_ex->mem_alloc) {
    alloc_field_func(&(game_inf_ex->game_inf.field), ROWS_MAP, COLS_MAP);
    alloc_field_func(&(game_inf_ex->game_inf.next), NEXT_SIZE, NEXT_SIZE);
    game_inf_ex->mem_alloc = true;
  }
  game_inf_ex->game_inf.level = 1;
  load_max_score(game_inf_ex);
}

void generate_next_func(GameInfoExtended_t *game_inf_ex) {
  int index = rand() % 7;
  if (index == 0) {
    int i[4][4] = {{0, 0, 0, 0}, {1, 1, 1, 1}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), i);
    game_inf_ex->tetromino.next_type = 'i';
  }
  if (index == 1) {
    int o[4][4] = {{0, 1, 1, 0}, {0, 1, 1, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), o);
    game_inf_ex->tetromino.next_type = 'o';
  }
  if (index == 2) {
    int t[4][4] = {{0, 1, 0, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), t);
    game_inf_ex->tetromino.next_type = 't';
  }
  if (index == 3) {
    int j[4][4] = {{1, 0, 0, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), j);
    game_inf_ex->tetromino.next_type = 'j';
  }
  if (index == 4) {
    int l[4][4] = {{0, 0, 1, 0}, {1, 1, 1, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), l);
    game_inf_ex->tetromino.next_type = 'l';
  }
  if (index == 5) {
    int s[4][4] = {{0, 1, 1, 0}, {1, 1, 0, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), s);
    game_inf_ex->tetromino.next_type = 's';
  }
  if (index == 6) {
    int z[4][4] = {{1, 1, 0, 0}, {0, 1, 1, 0}, {0}, {0}};
    fill_next_func(&(game_inf_ex->game_inf), z);
    game_inf_ex->tetromino.next_type = 'z';
  }
}

bool check_game_over(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  for (int j = 0; j < COLS_MAP; j++)
    if (game_inf_ex->touched_field[0][j] == 1) res = true;
  return res;
}

void next_to_tetromino(GameInfoExtended_t *game_inf_ex) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      game_inf_ex->tetromino.brick[i][j] = game_inf_ex->game_inf.next[i][j];
  game_inf_ex->tetromino.x = 0;  // высота поялвения брика
  game_inf_ex->tetromino.y = 4;  // ширина появления брика
  game_inf_ex->tetromino.current_type = game_inf_ex->tetromino.next_type;
  game_inf_ex->tetromino.next_type = 0;
}

void brick_to_move_field(GameInfoExtended_t *game_inf_ex) {
  int x = game_inf_ex->tetromino.x;
  int y = game_inf_ex->tetromino.y;
  reset_static_field(game_inf_ex->move_field);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      if (game_inf_ex->tetromino.brick[i][j] == 1)
        game_inf_ex->move_field[i + x][j + y] = 1;
}

void action_func(GameInfoExtended_t *game_inf_ex) {
  Tetromino tmp_tetr = {0};
  cp_tetr_to_tetr(&tmp_tetr, &(game_inf_ex->tetromino));
  if (game_inf_ex->tetromino.current_type == 'o') {
    ;
  } else if (game_inf_ex->tetromino.current_type == 'i') {
    turn_stick_func(&(game_inf_ex->tetromino));
  } else {
    turn_other_func(&(game_inf_ex->tetromino));
  }
  if (check_x_after_action(game_inf_ex)) {
    cp_tetr_to_tetr(&(game_inf_ex->tetromino), &tmp_tetr);
  }
  check_y_after_action(game_inf_ex);
  brick_to_move_field(game_inf_ex);
  if (check_tetr_collision_after_action(game_inf_ex)) {
    cp_tetr_to_tetr(&(game_inf_ex->tetromino), &tmp_tetr);
    brick_to_move_field(game_inf_ex);
  }
}

void move_brick_right(GameInfoExtended_t *game_inf_ex) {
  if (!check_collision_right(game_inf_ex)) {
    game_inf_ex->tetromino.y++;
    brick_to_move_field(game_inf_ex);
  }
}

void move_brick_left(GameInfoExtended_t *game_inf_ex) {
  if (!check_collision_left(game_inf_ex)) {
    game_inf_ex->tetromino.y--;
    brick_to_move_field(game_inf_ex);
  }
}

bool check_move_to_shift_func(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  if (!game_inf_ex->timer_activated) {
    game_inf_ex->timer = current_time_millis();
    game_inf_ex->timer_activated = true;
  }
  long long unsigned int lvl_speed = 1000 / game_inf_ex->game_inf.level;
  if (time_diff_func(game_inf_ex->timer) >= lvl_speed &&
      game_inf_ex->state != EXIT_STATE && game_inf_ex->game_inf.pause != 1) {
    game_inf_ex->timer_activated = false;
    game_inf_ex->timer = 0;
    res = true;
  }
  return res;
}

bool check_shifting_to_attaching(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  if (check_collision_tetromino(game_inf_ex->move_field,
                                game_inf_ex->touched_field) ||
      check_collision_bottom(game_inf_ex->move_field)) {
    res = true;
  }
  return res;
}

void move_brick(GameInfoExtended_t *game_inf_ex) {
  game_inf_ex->tetromino.x++;
  brick_to_move_field(game_inf_ex);
}

void move_from_move_to_touch(GameInfoExtended_t *game_inf_ex) {
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP; j++)
      if (game_inf_ex->move_field[i][j] == 1) {
        game_inf_ex->touched_field[i][j] = 1;
        game_inf_ex->move_field[i][j] = 0;
      }
}

void count_game_score(GameInfoExtended_t *game_inf_ex) {
  int filled_lines = 0;
  if ((filled_lines = check_filled_line(game_inf_ex->touched_field))) {
    if (filled_lines == 1) game_inf_ex->game_inf.score += 100;
    if (filled_lines == 2) game_inf_ex->game_inf.score += 300;
    if (filled_lines == 3) game_inf_ex->game_inf.score += 700;
    if (filled_lines == 4) game_inf_ex->game_inf.score += 1500;
  }
  if (game_inf_ex->game_inf.score > game_inf_ex->game_inf.high_score) {
    game_inf_ex->game_inf.high_score = game_inf_ex->game_inf.score;
    save_max_score(game_inf_ex);
  }
  set_lvl_func(game_inf_ex);
}

void dealloc_field(int ***field, int rows) {
  if (*field != NULL) {
    for (int row = 0; row < rows; row++) {
      if ((*field)[row] != NULL) free((*field)[row]);
    }
    free(*field);
  }
  *field = NULL;
}

static unsigned long long time_diff_func(unsigned long long timer) {
  return current_time_millis() - timer;
}

static unsigned long long current_time_millis() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)(tv.tv_sec) * 1000 +
         (unsigned long long)(tv.tv_usec) / 1000;
}

static void save_max_score(GameInfoExtended_t *game_inf_ex) {
  FILE *file = NULL;
  file = fopen("high_score.txt", "w");
  if (file) {
    fprintf(file, "%d", game_inf_ex->game_inf.high_score);
    fclose(file);
  }
}

static void load_max_score(GameInfoExtended_t *game_inf_ex) {
  FILE *file = NULL;
  int high_score = 0;
  file = fopen("high_score.txt", "r");
  if (file) {
    fscanf(file, "%d", &high_score);
    game_inf_ex->game_inf.high_score = high_score;
    fclose(file);
  }
}

static void fill_zero_func(int **field, int rows, int cols) {
  for (int i = 0; i < rows; i++)
    for (int j = 0; j < cols; j++) field[i][j] = 0;
}

static void alloc_field_func(int ***field, int rows, int cols) {
  *field = (int **)malloc(rows * sizeof(int *));
  if (*field != NULL) {
    for (int row = 0; row < rows; row++)
      (*field)[row] = (int *)malloc(cols * sizeof(int));
    fill_zero_func(*field, rows, cols);
  }
}

static void fill_next_func(GameInfo_t *game_inf, int next[][4]) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) game_inf->next[i][j] = next[i][j];
}

static void reset_static_field(int static_field[][COLS_MAP]) {
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP; j++) static_field[i][j] = 0;
}

static bool check_collision_bottom(int move_field[][COLS_MAP]) {
  for (int j = 0; j < COLS_MAP; j++)
    if (move_field[19][j] == 1) return true;
  return false;
}

static bool check_collision_left(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  if (game_inf_ex->tetromino.y == 0) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][0] == 1) res = true;
  }
  if (game_inf_ex->tetromino.y == -1) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][1] == 1) res = true;
  }
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 1; j < COLS_MAP;
         j++)  // -1 так как проверяем j+1 и превышение будет
      if (game_inf_ex->move_field[i][j] == 1 &&
          game_inf_ex->touched_field[i][j - 1] == 1)
        res = true;
  return res;
}

static bool check_collision_right(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  if (game_inf_ex->tetromino.y == 6) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][3] == 1) res = true;
  }
  if (game_inf_ex->tetromino.y == 7) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][2] == 1) res = true;
  }
  if (game_inf_ex->tetromino.y == 8) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][1] == 1) res = true;
  }
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP - 1;
         j++)  // -1 так как проверяем j+1 и превышение будет
      if (game_inf_ex->move_field[i][j] == 1 &&
          game_inf_ex->touched_field[i][j + 1] == 1)
        res = true;
  return res;
}

static bool check_collision_tetromino(int move_field[][COLS_MAP],
                                      int touched_field[][COLS_MAP]) {
  bool res = false;
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP; j++)
      if (move_field[i][j] == 1 && touched_field[i + 1][j] == 1) res = true;
  return res;
}

static void delete_filled_line(int touched_field[][COLS_MAP], int line) {
  for (int j = 0; j < COLS_MAP; j++) touched_field[line][j] = 0;

  for (int i = line; i > 0; i--)
    for (int j = 0; j < COLS_MAP; j++)
      touched_field[i][j] = touched_field[i - 1][j];
}

static int check_filled_line(int touched_field[][COLS_MAP]) {
  int filled_lines = 0;
  int summ = 0;
  for (int i = 19; i > 0; i--) {
    for (int j = 0; j < COLS_MAP; j++) {
      summ += touched_field[i][j];
      if (summ == 10) {
        delete_filled_line(touched_field, i);
        filled_lines += 1;
        i++;  // i прибавляем, т.к. линии опустились, иначе пропустим
      }
    }
    summ = 0;
  }
  return filled_lines;
}

static void set_lvl_func(GameInfoExtended_t *game_inf_ex) {
  int lvl = game_inf_ex->game_inf.level;
  int score = game_inf_ex->game_inf.score;
  if (lvl < 11 && (score > (lvl * 600))) game_inf_ex->game_inf.level++;
}

static void turn_stick_func(Tetromino *tetromino) {
  int turned_tetromino[4][4] = {0};
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) turned_tetromino[j][i] = tetromino->brick[i][j];
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) tetromino->brick[i][j] = turned_tetromino[i][j];
}

static void turn_other_func(Tetromino *tetromino) {
  int turned_tetromino[3][3] = {0};
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) {
      int temp_j = j - 1;
      int temp_i = i - 1;
      int needed_i = 0;
      int needed_j = 0;
      needed_i = temp_j;
      needed_j = -temp_i;
      turned_tetromino[needed_i + 1][needed_j + 1] = tetromino->brick[i][j];
    }
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) tetromino->brick[i][j] = turned_tetromino[i][j];
}

static bool check_tetr_collision_after_action(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  for (int i = 0; i < ROWS_MAP; i++)
    for (int j = 0; j < COLS_MAP; j++) {
      if (game_inf_ex->move_field[i][j] == 1 &&
          game_inf_ex->touched_field[i][j] == 1)
        res = true;
    }
  return res;
}

static void cp_tetr_to_tetr(Tetromino *dst, Tetromino *src) {
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++) dst->brick[i][j] = src->brick[i][j];
  dst->x = src->x;
  dst->y = src->y;
  dst->current_type = src->current_type;
  dst->next_type = src->next_type;
}

static void check_y_after_action(GameInfoExtended_t *game_inf_ex) {
  int y = game_inf_ex->tetromino.y;
  if (y == -1) {  // y - координата левого верхнего угла
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][0] == 1) game_inf_ex->tetromino.y++;
  }
  if (y == 7) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][3] == 1) game_inf_ex->tetromino.y--;
  }
  if (y == 8) {
    for (int i = 0; i < 4; i++)
      if (game_inf_ex->tetromino.brick[i][2] == 1) game_inf_ex->tetromino.y--;
  }
}

static bool check_x_after_action(GameInfoExtended_t *game_inf_ex) {
  bool res = false;
  int x = game_inf_ex->tetromino.x;
  if (x == 17) {
    for (int j = 0; j < 4; j++)
      if (game_inf_ex->tetromino.brick[3][j] == 1) res = true;
  }
  if (x == 18) {
    for (int j = 0; j < 4; j++)
      if (game_inf_ex->tetromino.brick[2][j] == 1) res = true;
  }
  if (x == 19) {
    for (int j = 0; j < 4; j++)
      if (game_inf_ex->tetromino.brick[1][j] == 1) res = true;
  }
  return res;
}
