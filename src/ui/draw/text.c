#include "Connect4/ui/draw/text.h"
#include <assert.h>

void C4_UI_DrawText(SDL_FRect rect, C4_UI_Data_Text* text, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer, float uiScale) {
    assert(text && text->textObject && styleState && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    // Prevent division by zero
    if (uiScale <= 0.001f) {
        uiScale = 1.0f;
    }

    SDL_Color color = styleState->text;
    TTF_SetTextColor(text->textObject, color.r, color.g, color.b, color.a);

    float oldScaleX, oldScaleY;
    SDL_GetRenderScale(renderer, &oldScaleX, &oldScaleY);

    SDL_SetRenderScale(renderer, 1.0f, 1.0f);

    int texW = 0, texH = 0;
    TTF_GetTextSize(text->textObject, &texW, &texH);

    float scaleX = (texW > 0) ? rect.w / (float)texW : 1.0f;
    float scaleY = (texH > 0) ? rect.h / (float)texH : 1.0f;

    float flipX = (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) ? -1.0f : 1.0f;
    float flipY = (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) ? -1.0f : 1.0f;
    
    float drawX = rect.x / scaleX;
    float drawY = rect.y / scaleY;
    float drawW = rect.w / scaleX;
    float drawH = rect.h / scaleY;

    if (flipX < 0) {
        drawX = -drawX - drawW;
    }
    
    if (flipY < 0) {
        drawY = -drawY - drawH;
    }

    SDL_SetRenderScale(renderer, scaleX * flipX, scaleY * flipY);

    TTF_DrawRendererText(text->textObject, SDL_roundf(drawX), SDL_roundf(drawY));

    SDL_SetRenderScale(renderer, oldScaleX, oldScaleY);
}