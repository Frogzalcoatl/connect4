#include "Connect4/ui/elements/popup.h"
#include "Connect4/constants.h"
#include <stdlib.h>

#define C4_UI_POPUP_PADDING 10

static void C4_UI_Popup_PositionElementsInBackground(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Tried to position NULL popup elements");
        return;
    }
    SDL_FRect* rect = &popup->background->rectangle;
    C4_UI_Button_TransformResize(popup->button1, rect->x + popup->borderWidth + C4_UI_POPUP_PADDING, rect->y + rect->h - popup->buttonHeight - popup->borderWidth - C4_UI_POPUP_PADDING, rect->w / 2 - C4_UI_POPUP_PADDING * 1.5 - popup->borderWidth, popup->buttonHeight);
    C4_UI_Button_TransformResize(popup->button2, rect->x + C4_UI_POPUP_PADDING / 2 + (rect->w / 2), rect->y + rect->h - popup->buttonHeight - popup->borderWidth - C4_UI_POPUP_PADDING, rect->w / 2 - C4_UI_POPUP_PADDING * 1.5 - popup->borderWidth, popup->buttonHeight);
    popup->message->destination.x = rect->x + popup->borderWidth + C4_UI_POPUP_PADDING;
    popup->message->destination.y = rect->y + popup->borderWidth + C4_UI_POPUP_PADDING;
    popup->message->wrapWidth = rect->w - (popup->borderWidth * 2);
    C4_UI_Text_Refresh(popup->message, popup->renderer);
    popup->borders->rectangle = popup->background->rectangle;
}

C4_UI_Popup* C4_UI_Popup_Create(SDL_Renderer* renderer, SDL_FRect rectangle, unsigned int borderWidth, unsigned int buttonHeight, const char* button1Text, const char* button2Text, const char* messageText, float buttonPtSize, float messagePtSize) {
    if (!renderer) {
        SDL_Log("Unable to create popup element. Renderer is NULL");
        return NULL;
    }
    C4_UI_Popup* popup = calloc(1, sizeof(C4_UI_Popup));
    if (!popup) {
        SDL_Log("Unable to allocate memory for popup");
        return NULL;
    }
    popup->renderer = renderer;
    popup->button1 = C4_UI_Button_Create(renderer, button1Text, C4_FontType_Bold, buttonPtSize, (SDL_FRect){1, 1, 1, 1});
    if (!popup->button1) {
        SDL_Log("Unable to create popup button1");
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    popup->button2 = C4_UI_Button_Create(renderer, button2Text, C4_FontType_Bold, buttonPtSize, (SDL_FRect){1, 1, 1, 1});
    if (!popup->button2) {
        SDL_Log("Unable to create popup button2");
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    popup->borders = C4_UI_Borders_Create(rectangle, (SDL_Color){255, 255, 255, 255}, borderWidth);
    if (!popup->borders) {
        SDL_Log("Unable to create popup borders");
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    popup->background = C4_UI_Rectangle_Create(rectangle, (SDL_Color){0, 0, 0, 255});
    if (!popup->background) {
        SDL_Log("Unable to create popup background");
        C4_UI_Popup_Destroy(popup);
        return NULL;
    }
    popup->message = C4_UI_Text_Create(popup->renderer, messageText, C4_FontType_Regular, messagePtSize, 0, 0, 0);
    if (!popup->message) {
        SDL_Log("Unable to create popup message");
        C4_UI_Popup_Destroy(popup);   
        return NULL;
    }
    popup->borderWidth = borderWidth;
    popup->buttonHeight = buttonHeight;
    C4_UI_Popup_PositionElementsInBackground(popup);
    popup->isShowing = false;
    return popup;
}

void C4_UI_Popup_Destroy(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Tried to destroy NULL popup element");
        return;
    }
    if (popup->message) {
        C4_UI_Text_Destroy(popup->message);
    }
    if (popup->background) {
        C4_UI_Rectangle_Destroy(popup->background);
    }
    if (popup->borders) {
        C4_UI_Borders_Destroy(popup->borders);
    }
    if (popup->button2) {
        C4_UI_Button_Destroy(popup->button2);
    }
    if (popup->button1) {
        C4_UI_Button_Destroy(popup->button1);
    }
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
    C4_UI_Rectangle_Draw(popup->background, popup->renderer);
    C4_UI_Borders_Draw(popup->borders, popup->renderer);
    C4_UI_Text_Draw(popup->message, popup->renderer);
    C4_UI_Button_Draw(popup->button1, popup->renderer);
    C4_UI_Button_Draw(popup->button2, popup->renderer);
}

void C4_UI_Popup_CenterInWindow(C4_UI_Popup* popup) {
    if (!popup) {
        SDL_Log("Popup element is NULL");
        return;
    }
    C4_UI_Rectangle_CenterInWindow(popup->background, C4_Axis_XY);
    C4_UI_Popup_PositionElementsInBackground(popup);
}

unsigned short C4_UI_Popup_HandleMouseEvents(C4_UI_Popup* popup, SDL_Event* event) {
    if (!popup || !event) {
        SDL_Log("Popup element and/or event is NULL");
        return 0;
    }
    if (!popup->isShowing) {
        return 0;
    }
    if (C4_UI_Button_HandleMouseEvents(popup->button1, event, popup->renderer)) {
        return 1;
    }
    if (C4_UI_Button_HandleMouseEvents(popup->button2, event, popup->renderer)) {
        return 2;
    }
    return 0;
}