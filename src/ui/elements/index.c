#include "Connect4/ui/elements/index.h"
#include "SDL3/SDL.h"

void C4_UI_CenterInWindow(SDL_FRect* rect, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight) {
    if (!rect) {
        SDL_Log("SDL FRect is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        rect->x = (windowWidth / 2.f) - (rect->w / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        rect->y = (windowHeight / 2.f) - (rect->h / 2.f);
    }
}

void C4_UI_GetScaledRect (
    const SDL_FRect* baseRect, SDL_FRect* returnedRect, float scale, float parentX, float parentY
) {
    returnedRect->x = parentX + (baseRect->x * scale);
    returnedRect->y = parentY + (baseRect->y * scale);
    returnedRect->w = baseRect->w * scale;
    returnedRect->h = baseRect->h * scale;
}