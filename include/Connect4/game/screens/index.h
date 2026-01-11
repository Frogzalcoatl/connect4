#include <SDL3/SDL.h>
#include "Connect4/ui/screen.h"
#pragma once

typedef enum {
    C4_ScreenType_Menu,
    C4_ScreenType_Game,
    C4_ScreenType_Settings,
    C4_ScreenType_ScreenCount
} C4_ScreenType;