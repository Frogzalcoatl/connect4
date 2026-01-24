#include "Connect4/game/connect4.h"
#include "Connect4/game/consoleOutput.h"
#include "SDL3/SDL_main.h"


int main(int argc, char *argv[]) {
  // To shutup compiler warnings
  (void)argc;
  (void)argv;

  Connect4_Init_Dependencies();

  C4_Game* connect4 = C4_Game_Create(7, 6, 4);
  if (!connect4) {
    Connect4_Quit_Dependencies();
    C4_FatalError(C4_ErrorCode_RipConnect4Struct, " ");
  }

  C4_Game_Run(connect4);
  C4_Game_Destroy(connect4);

  Connect4_Quit_Dependencies();
  return 0;
}