#include "Connect4/ui/draw/text.h"
#include <math.h>

void C4_UI_DrawText(SDL_FRect rect, C4_UI_Data_Text* text, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!text || !text->textObject) {
        SDL_Log("Unable to draw text. One or more required pointers are NULL");
        return;
    }
    SDL_Color color = styleState->text;
    TTF_SetTextColor(text->textObject, color.r, color.g, color.b, color.a);

    if (mirror == C4_UI_Mirroring_None) {
        TTF_DrawRendererText(text->textObject, roundf(rect.x), roundf(rect.y));
        return;
    }

    float oldScaleX, oldScaleY;
    SDL_GetRenderScale(renderer, &oldScaleX, &oldScaleY);

    float flipX = (mirror == C4_UI_Mirroring_X || mirror == C4_UI_Mirroring_XY) ? -1.0f : 1.0f;
    float flipY = (mirror == C4_UI_Mirroring_Y || mirror == C4_UI_Mirroring_XY) ? -1.0f : 1.0f;

    float drawX = rect.x;
    float drawY = rect.y;

    if (flipX < 0) {
        drawX = -drawX - rect.w;
    }
    
    if (flipY < 0) {
        drawY = -drawY - rect.h;
    }

    SDL_SetRenderScale(renderer, oldScaleX * flipX, oldScaleY * flipY);

    TTF_DrawRendererText(text->textObject, roundf(drawX), roundf(drawY));

    SDL_SetRenderScale(renderer, oldScaleX, oldScaleY);
}