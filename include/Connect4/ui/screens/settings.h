#pragma once
#include "Connect4/ui/elements/buttonGroup.h"
#include "Connect4/ui/elements/popup.h"
#include "Connect4/ui/elements/numberInput.h"
#include "Connect4/game/board.h"

typedef struct {
    // [Ref] Shared resources owned by C4_Game
    SDL_Renderer* renderer;
    SDL_Window* window;
    C4_Board* board;

    // [Owns] properties specific to this screen
    C4_UI_Text title;
    C4_UI_ButtonGroup buttonGroup;
    C4_UI_Popup confirmationPopup;
    C4_UI_NumberInput boardWidthInput;
    C4_UI_Text boardWidthText;
    C4_UI_NumberInput boardHeightInput;
    C4_UI_Text boardHeightText;
    C4_UI_NumberInput amountToWinInput;
    C4_UI_Text amountToWinText;
} C4_Screen_Settings;

C4_Screen_Settings* C4_Screen_Settings_Create(SDL_Renderer* renderer, SDL_Window* window, C4_Board* board);
void C4_Screen_Settings_Destroy(void* screenData);
void C4_Screen_Settings_Draw(void* screenData);
void C4_Screen_Settings_HandleKeyboardInput(void* screenData, SDL_Scancode scancode);
void C4_Screen_Settings_HandleMouseEvents(void* screenData, SDL_Event* event);