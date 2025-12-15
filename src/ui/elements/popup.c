#include "Connect4/ui/elements/popup.h"
#include "Connect4/constants.h"
#include <stdlib.h>

#define C4_UI_POPUP_MARGIN 10

static void C4_UI_Popup_PositionElementsInBackground(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Tried to position NULL popup elements");
        return;
    }
    SDL_FRect* rect = &popup->background.rectangle;
    popup->message.destination.x = rect->x + popup->borderWidth + C4_UI_POPUP_MARGIN;
    popup->message.destination.y = rect->y + popup->borderWidth + C4_UI_POPUP_MARGIN;
    popup->message.wrapWidth = rect->w - popup->borderWidth * 2 - C4_UI_POPUP_MARGIN;
    C4_UI_Text_Refresh(&popup->message, popup->renderer);
    popup->borders.rectangle = popup->background.rectangle;
    SDL_FRect buttonStackRect;
    buttonStackRect.x = rect->x + popup->borderWidth + C4_UI_POPUP_MARGIN;
    buttonStackRect.w = rect->w - popup->borderWidth - (C4_UI_POPUP_MARGIN * popup->buttonStack.count + 1);
    buttonStackRect.h = popup->buttonStack.bounds.h;
    buttonStackRect.y = rect->y + rect->h - buttonStackRect.h - popup->borderWidth - C4_UI_POPUP_MARGIN;
    C4_UI_ButtonStack_TransformResize(&popup->buttonStack, buttonStackRect);
}

bool C4_UI_Popup_InitProperties(
    C4_UI_Popup* popup, SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, C4_UI_ButtonStack_Direction buttonDirection,
    size_t buttonCount, float buttonStackHeight, const char* messageText, float buttonPtSize, float messagePtSize
) {
    if (!renderer) {
        SDL_Log("Unable to init popup element properties. Renderer is NULL");
        return false;
    }
    if (!messageText) {
        SDL_Log("Unable to init popup element properties. Message text is NULL");
        return false;
    }
    if (!popup) {
        SDL_Log("Unable to init popup elements. Popup pointer is NULL");
        return false;
    }
    popup->renderer = renderer;
    if (!C4_UI_ButtonStack_InitProperties(&popup->buttonStack, renderer, (SDL_FRect){0.f, 0.f, 0.f, buttonStackHeight}, buttonCount, buttonDirection, C4_UI_POPUP_MARGIN, buttonPtSize)) {
        return false;
    }
    if (!C4_UI_Borders_InitProperties(&popup->borders, rectangle, (SDL_Color){255, 255, 255, 255}, borderWidth)) {
        return false;
    }
    if (!C4_UI_Rectangle_InitProperties(&popup->background, rectangle, (SDL_Color){0, 0, 0, 255})) {
        return false;
    }
    if (!C4_UI_Text_InitProperties(&popup->message, popup->renderer, messageText, C4_FontType_Regular, messagePtSize, 0, 0, 0)) {
        return false;
    }
    popup->borderWidth = borderWidth;
    C4_UI_Popup_PositionElementsInBackground(popup);
    popup->isShowing = false;
    return true;
}

C4_UI_Popup* C4_UI_Popup_Create(
    SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, C4_UI_ButtonStack_Direction buttonDirection,
    size_t buttonCount, float buttonStackHeight, const char* messageText, float buttonPtSize, float messagePtSize
) {
    C4_UI_Popup* popup = calloc(1, sizeof(C4_UI_Popup));
    if (!popup) {
        SDL_Log("Unable to allocate memory for popup");
        return NULL;
    }
    if (!C4_UI_Popup_InitProperties(popup, renderer, rectangle, borderWidth, buttonDirection, buttonCount, buttonStackHeight, messageText, buttonPtSize, messagePtSize)) {
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    return popup;
}

void C4_UI_Popup_FreeResources(C4_UI_Popup* popup) {
    if (!popup) {
        return;
    }
    C4_UI_ButtonStack_FreeResources(&popup->buttonStack);
    C4_UI_Text_FreeResources(&popup->message);
}

void C4_UI_Popup_Destroy(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Tried to destroy NULL popup element");
        return;
    }
    C4_UI_Popup_FreeResources(popup);
    free(popup);
}

static void C4_UI_Popup_DrawScreenBackground(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
    SDL_RenderFillRect(renderer, &(SDL_FRect){0.f, 0.f, C4_BASE_WINDOW_WIDTH, C4_BASE_WINDOW_HEIGHT});
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void C4_UI_Popup_Draw(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Popup element is NULL");
        return;
    }
    if (!popup->isShowing) {
        return;
    }
    C4_UI_Popup_DrawScreenBackground(popup->renderer);
    C4_UI_Rectangle_Draw(&popup->background, popup->renderer);
    C4_UI_Borders_Draw(&popup->borders, popup->renderer);
    C4_UI_Text_Draw(&popup->message, popup->renderer);
    C4_UI_ButtonStack_Draw(&popup->buttonStack, popup->renderer);
}

void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Popup element is NULL");
        return;
    }
    C4_UI_CenterInWindow(&popup->background.rectangle, C4_Axis_XY);
    C4_UI_Popup_PositionElementsInBackground(popup);
}

long long C4_UI_Popup_HandleMouseEvents(C4_UI_Popup* popup, SDL_Event* event) {
    if (!popup || !event) {
        SDL_Log("Popup element and/or event is NULL");
        return -1;
    }
    if (!popup->isShowing) {
        return -1;
    }
    return C4_UI_ButtonStack_HandleMouseEvents(&popup->buttonStack, event, popup->renderer);
}

void C4_UI_Popup_SetButtonText(C4_UI_Popup* popup, SDL_Renderer* renderer, size_t buttonIndex, const char* text) {
    if (!popup || buttonIndex >= popup->buttonStack.count) {
        return;
    }
    C4_UI_Button_ChangeStr(&popup->buttonStack.buttons[buttonIndex], text, renderer);
}