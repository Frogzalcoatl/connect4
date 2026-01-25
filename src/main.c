#include "Connect4/game/connect4.h"
#include "Connect4/game/consoleOutput.h"
#include "Connect4/instanceManager/index.h"
#include "SDL3/SDL_main.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    // To shutup compiler warnings
    (void)argc;
    (void)argv;

    if (C4_IsAnotherInstanceRunning()) {
        return 1;
    }

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