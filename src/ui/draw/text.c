#include "Connect4/ui/draw/text.h"
#include <math.h>

void C4_UI_DrawText(SDL_FRect rect, C4_UI_Data_Text* text, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer, float UIScale) {
    if (!text || !text->textObject) {
        SDL_Log("Unable to draw text. One or more required pointers are NULL");
        return;
    }
    // Prevent division by zero
    if (UIScale <= 0.001f) UIScale = 1.0f;

    SDL_Color color = styleState->text;
    TTF_SetTextColor(text->textObject, color.r, color.g, color.b, color.a);

    float oldScaleX, oldScaleY;
    SDL_GetRenderScale(renderer, &oldScaleX, &oldScaleY);

    if (mirror == C4_UI_Mirror_None) {
        SDL_SetRenderScale(renderer, oldScaleX * UIScale, oldScaleY * UIScale);

        TTF_DrawRendererText(text->textObject, roundf(rect.x / UIScale), roundf(rect.y / UIScale));
        
        SDL_SetRenderScale(renderer, oldScaleX, oldScaleY);
        return;
    }

    float flipX = (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) ? -1.0f : 1.0f;
    float flipY = (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) ? -1.0f : 1.0f;
    
    float drawX = rect.x / UIScale;
    float drawY = rect.y / UIScale;
    float drawW = rect.w / UIScale;
    float drawH = rect.h / UIScale;

    if (flipX < 0) {
        drawX = -drawX - drawW;
    }
    
    if (flipY < 0) {
        drawY = -drawY - drawH;
    }

    SDL_SetRenderScale(renderer, oldScaleX * flipX * UIScale, oldScaleY * flipY * UIScale);

    TTF_DrawRendererText(text->textObject, roundf(drawX), roundf(drawY));

    SDL_SetRenderScale(renderer, oldScaleX, oldScaleY);
}