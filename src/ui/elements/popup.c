#include "Connect4/ui/elements/popup.h"
#include <stdlib.h>

#define C4_UI_POPUP_MARGIN 20

static void C4_UI_Popup_PositionElementsInBackground(C4_UI_Popup* popup, SDL_Renderer* renderer) {
    if (!popup) {
        SDL_Log("Tried to position NULL popup elements");
        return;
    }
    SDL_FRect* rect = &popup->background.destination;
    popup->message.destination.x = rect->x + (float)popup->borders.width + C4_UI_POPUP_MARGIN;
    popup->message.destination.y = rect->y + (float)popup->borders.width + C4_UI_POPUP_MARGIN;
    popup->message.wrapWidth = (int)rect->w - (int)popup->borders.width * 2 - C4_UI_POPUP_MARGIN;
    C4_UI_Text_ReloadTexture(&popup->message, renderer);
    popup->borders.destination = popup->background.destination;
    SDL_FRect buttonGroupRect;
    buttonGroupRect.x = rect->x + (float)popup->borders.width + C4_UI_POPUP_MARGIN;
    buttonGroupRect.w = rect->w - (float)popup->borders.width - (((float)popup->borders.width + C4_UI_POPUP_MARGIN) * 2.f);
    buttonGroupRect.h = popup->buttonGroup.bounds.h;
    buttonGroupRect.y = rect->y + rect->h - buttonGroupRect.h - (float)popup->borders.width - C4_UI_POPUP_MARGIN;
    C4_UI_ButtonGroup_TransformResize(&popup->buttonGroup, buttonGroupRect);
}

bool C4_UI_Popup_InitProperties(
    C4_UI_Popup* popup, SDL_Renderer* renderer, const C4_UI_Popup_Config* config
) {
    if (!renderer) {
        SDL_Log("Unable to init popup element properties. Renderer is NULL");
        return false;
    }
    if (!config) {
        return false;
    }
    if (!config->message) {
        SDL_Log("Unable to init popup element properties. Message text is NULL");
        return false;
    }
    if (!popup) {
        SDL_Log("Unable to init popup elements. Popup pointer is NULL");
        return false;
    }
    if (!config->theme) {
        SDL_Log("Unable to init popup element properties. Theme is NULL");
        return false;
    }
    if (
        !C4_UI_ButtonGroup_InitProperties(
            &popup->buttonGroup, renderer,
            &(C4_UI_ButtonGroup_Config){
                .destination = (SDL_FRect){0.f, 0.f, 0.f, config->buttonGroupHeight},
                .count = config->buttonCount,
                .buttonDirection = config->buttonDirection,
                .margin = C4_UI_POPUP_MARGIN,
                .font = config->buttonFont,
                .theme = config->theme
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Borders_InitProperties(
            &popup->borders,
            &(C4_UI_Borders_Config){
                .destination = config->destination,
                .color = (SDL_Color){80, 80, 80, 255},
                .width = config->theme->borderWidth
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Rectangle_InitProperties(
            &popup->background,
            &(C4_UI_Rectangle_Config){
                .destination = config->destination,
                .color = (SDL_Color){0, 0, 0, 255}
            }
        )
    ) {
        return false;
    }
    if (
        !C4_UI_Text_InitProperties(
            &popup->message, renderer,
            &(C4_UI_Text_Config){
               .str = config->message,
               .font = config->messageFont,
               .color = config->theme->textColor,
               .ptSize = config->theme->defaultPtSize,
               .destinationX = 0.f,
               .destinationY = 0.f,
               .wrapWidth = 0 
            }
        )
    ) {
        return false;
    }
    C4_UI_Popup_PositionElementsInBackground(popup, renderer);
    popup->isShowing = false;
    return true;
}

C4_UI_Popup* C4_UI_Popup_Create(
    SDL_Renderer* renderer, const C4_UI_Popup_Config* config
) {
    C4_UI_Popup* popup = calloc(1, sizeof(C4_UI_Popup));
    if (!popup) {
        SDL_Log("Unable to allocate memory for popup");
        return NULL;
    }
    if (!C4_UI_Popup_InitProperties(popup, renderer, config)) {
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    return popup;
}

void C4_UI_Popup_FreeResources(C4_UI_Popup* popup) {
    if (!popup) {
        return;
    }
    C4_UI_ButtonGroup_FreeResources(&popup->buttonGroup);
    C4_UI_Text_FreeResources(&popup->message);
}

void C4_UI_Popup_Destroy(void* data) {
    if (!data) {
        SDL_Log("Tried to destroy NULL popup element");
        return;
    }
    C4_UI_Popup* popup = (C4_UI_Popup*)data;
    C4_UI_Popup_FreeResources(popup);
    free(popup);
}

static void C4_UI_Popup_DrawScreenBackground(SDL_Renderer* renderer) {
    SDL_Rect window;
    SDL_GetRenderViewport(renderer, &window);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, &(SDL_FRect){0.f, 0.f, (float)window.w, (float)window.h});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void C4_UI_Popup_Draw(void* data, SDL_Renderer* renderer, float scale, float parentX, float parentY) {
    if (!data) {
        SDL_Log("Popup element is NULL");
        return;
    }
    C4_UI_Popup* popup = (C4_UI_Popup*)data;
    if (!renderer) {
        return;
    }
    if (!popup->isShowing) {
        return;
    }
    C4_UI_Popup_DrawScreenBackground(renderer);
    C4_UI_Rectangle_Draw(&popup->background, renderer, scale, parentX, parentY);
    C4_UI_Borders_Draw(&popup->borders, renderer, scale, parentX, parentY);
    C4_UI_Text_Draw(&popup->message, renderer, scale, parentX, parentY);
    C4_UI_ButtonGroup_Draw(&popup->buttonGroup, renderer, scale, parentX, parentY);
}

void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup, SDL_Renderer* renderer, unsigned int windowWidth, unsigned int windowHeight) {
    if (!popup) {
        SDL_Log("Popup element is NULL");
        return;
    }
    C4_UI_CenterInWindow(&popup->background.destination, C4_Axis_XY, windowWidth, windowHeight);
    C4_UI_Popup_PositionElementsInBackground(popup, renderer);
}

void C4_UI_Popup_Update(void* data, float deltaTime) {
    if (!data) {
        return;
    }
    C4_UI_Popup* popup = (C4_UI_Popup*)data;
    C4_UI_ButtonGroup_Update(&popup->buttonGroup, deltaTime);
}

void C4_UI_Popup_HandleMouseEvents(void* data, SDL_Event* event, float scale, float parentX, float parentY) {
    if (!data || !event) {
        SDL_Log("Popup element and/or event is NULL");
        return;
    }
    C4_UI_Popup* popup = (C4_UI_Popup*)data;
    if (!popup->isShowing) {
        return;
    }
    C4_UI_ButtonGroup_HandleMouseEvents(&popup->buttonGroup, event, scale, parentX, parentY);
}

void C4_UI_Popup_SetButtonText(C4_UI_Popup* popup, SDL_Renderer* renderer, size_t buttonIndex, const char* text) {
    if (!popup || buttonIndex >= popup->buttonGroup.count) {
        return;
    }
    C4_UI_Button_UpdateStr(&popup->buttonGroup.buttons[buttonIndex], text, renderer);
}

void C4_UI_Popup_ResetButtons(void* data) {
    if (!data) {
        return;
    }
    C4_UI_Popup* popup = (C4_UI_Popup*)data;
    C4_UI_ButtonGroup_Reset(&popup->buttonGroup);
}