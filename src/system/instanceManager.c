#include "Connect4/system/instanceManager.h"

#ifdef _WIN32
#include <windows.h>
static HANDLE mutex = NULL;
#endif

bool C4_IsAnotherInstanceRunning(void) {
#ifdef _WIN32
    mutex = CreateMutexA(NULL, TRUE, "Global\\1efdd983-4a0e-42d4-9183-f748b0f799f0");
    if (!mutex)
        return false;

    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(mutex);
        return true;
    }
#elif defined(__unix__) || defined(__APPLE__)
    // Will do later
#endif
    return false;
}