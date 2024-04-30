#include "tetris.h"

int main(){
  enable_ncurses();
  game_loop();
  disable_ncurses();
  return 0;
}

void user_input_to_action(int user_input)
{
  switch(user_input)
  {
    case KEY_LEFT:
      userInput(Left, false);
      break;
    case KEY_RIGHT:
      userInput(Right, false);
      break;
    case KEY_UP:
      userInput(Up, false);
      break;
    case KEY_DOWN:
      userInput(Down, false);
      break;
    case 'a':
      userInput(Action, false);
      break;
    case 's':
      userInput(Start, false);
      break;
    case 'p':
      userInput(Pause, false);
      break;
    case 't':
      userInput(Terminate, false);
      break;
    default:
      break;
  }
}

void game_loop()
{
  GameInfo_t game_inf = {0};
  while(game_inf.pause != 3)
  {
    game_inf = updateCurrentState();
    frontend_general_func(game_inf);
    user_input_to_action(getch());
  }
}
