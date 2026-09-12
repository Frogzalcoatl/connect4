#include "Connect4/ui/window.h"

void C4_Window_GetScaledSize(SDL_Window* window, float uiScale, int* w, int* h) {
    SDL_GetWindowSize(window, w, h);

    if (uiScale < 0.001f) {
        uiScale = 0.001f;
    }

    if (w)
        *w = (int)SDL_roundf((float)*w / uiScale);
    if (h)
        *h = (int)SDL_roundf((float)*h / uiScale);
}