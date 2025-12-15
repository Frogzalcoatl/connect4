#include "Connect4/ui/elements/index.h"
#include "Connect4/constants.h"
#include <SDL3/SDL.h>

void C4_UI_CenterInWindow(SDL_FRect* rect, C4_Axis axis) {
    if (!rect) {
        SDL_Log("SDL FRect is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        rect->x = (C4_BASE_WINDOW_WIDTH / 2.f) - (rect->w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        rect->y = (C4_BASE_WINDOW_HEIGHT / 2.f) - (rect->h / 2.f);
    }
}