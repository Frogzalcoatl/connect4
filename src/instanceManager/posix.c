#include "Connect4/instanceManager/posix.h"

#if defined(__unix__) || defined(__APPLE__)
#include <fcntl.h>
#include <sys/file.h>
#include <unistd.h>
#endif

bool C4_Posix_IsAnotherInstanceRunning(void) {
    // TODO: Ill do this once my linux/macos builds are working again (when sdl3 mixer is released on vcpkg)
    return false;
}