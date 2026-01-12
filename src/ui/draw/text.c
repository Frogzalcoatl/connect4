#include "Connect4/ui/draw/text.h"

void C4_UI_DrawText(SDL_FRect rect, C4_UI_Data_Text* text, C4_UI_StyleState* styleState) {
    if (!text || !text->textObject) {
        SDL_Log("Unable to draw text. One or more required pointers are NULL");
        return;
    }
    SDL_Color color = styleState->text;
    TTF_SetTextColor(text->textObject, color.r, color.g, color.b, color.a);
    TTF_DrawRendererText(text->textObject, rect.x, rect.y);
}