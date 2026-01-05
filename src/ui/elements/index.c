#include "Connect4/ui/elements/index.h"
#include "SDL3/SDL.h"

void C4_UI_ClampToWindow(SDL_FRect* rect, unsigned int windowWidth, unsigned int windowHeight, float UIScale) {
    if (!rect) {
        return;
    }

    float visualWidth = rect->w * UIScale;
    float visualHeight = rect->h * UIScale;

    float scaledRightEdge = (rect->x * UIScale) + visualWidth;
    if (scaledRightEdge > (float)windowWidth) {
        rect->x = ((float)windowWidth / UIScale) - rect->w;
    }
    if (rect->x < 0.f) {
        rect->x = 0.f;
    }

    float scaledBottomEdge = (rect->y * UIScale) + visualHeight;
    if (scaledBottomEdge > (float)windowHeight) {
        rect->y = ((float)windowHeight / UIScale) - rect->h;
    }
    if (rect->y < 0.f) {
        rect->y = 0.f;
    }
}

void C4_UI_CenterInWindow(SDL_FRect* rect, C4_Axis axis, unsigned int windowWidth, unsigned int windowHeight, float UIScale) {
    if (!rect) {
        SDL_Log("SDL FRect is NULL");
        return;
    }
    if (axis == C4_Axis_X || axis == C4_Axis_XY) {
        rect->x = (windowWidth / 2.f) - (rect->w * UIScale / 2.f);
    }
    if (axis == C4_Axis_Y || axis == C4_Axis_XY) {
        rect->y = (windowHeight / 2.f) - (rect->h * UIScale / 2.f);
    }
    C4_UI_ClampToWindow(rect, windowWidth, windowHeight, UIScale);
}

void C4_UI_GetScaledRect (
    const SDL_FRect* baseRect, SDL_FRect* returnedRect, float scale, float parentX, float parentY
) {
    returnedRect->x = parentX + (baseRect->x * scale);
    returnedRect->y = parentY + (baseRect->y * scale);
    returnedRect->w = baseRect->w * scale;
    returnedRect->h = baseRect->h * scale;
}

C4_UI_LayoutType C4_UI_GetCurrentLayout(unsigned int windowWidth, unsigned int windowHeight) {
    if (windowWidth >= windowHeight) {
        return C4_UI_LayoutType_Wide;
    } else {
        return C4_UI_LayoutType_Tall;
    }
}