
#include "Connect4/instanceManager/index.h"
#include "Connect4/instanceManager/windows.h"
#include "Connect4/instanceManager/posix.h"

bool C4_IsAnotherInstanceRunning(void) {
    #ifdef _WIN32
        return C4_Windows_IsAnotherInstanceRunning();
    #elif defined(__unix__) || defined(__APPLE__)
        return C4_Posix_IsAnotherInstanceRunning();
    #endif
}