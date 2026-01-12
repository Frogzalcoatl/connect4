#include "Connect4/ui/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include "Connect4/ui/element/button.h"
#include "Connect4/ui/utils.h"

typedef struct {
    C4_Game* game;
    C4_UI_Node* controllerList;
    C4_UI_Node* activeControllerText;
} C4_MenuScreenData;

static void C4_MenuScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        SDL_Log("Unable to resize menu ui. One or more required pointers are NULL.");
        return;
    }
    //C4_MenuScreenData* menuData = (C4_MenuScreenData*)screen->data;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            
        }; break;
        case C4_UI_LayoutType_Tall: {
            
        }; break;
        default: break;
    }
}

// Forward declaration just for the purpose of organization
static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game);

static void UpdateControllerText(C4_UI_Node* controllerList, C4_UI_Node* activeControllerText) {
    char* controllerArr[C4_MAX_GAMEPADS];
    C4_Gamepad_GetNames(controllerArr, C4_MAX_GAMEPADS);

    char* controllerText = C4_JoinStrings((const char**)controllerArr, C4_MAX_GAMEPADS, "\n");
    
    char header[2048] = "Controllers:\n";
    if (controllerText && *controllerText != '\0') {
        strcat(header, controllerText);
    } else {
        strcat(header, "None");
    }

    if (controllerText) {
        free(controllerText);
    }
    
    C4_UI_Node_SetTextString(controllerList, header);

    char activeController[128];
    C4_Gamepad_GetActiveName(activeController, sizeof(activeController));
    char displayText[256] = "Active: ";
    strcat(displayText, activeController);

    C4_UI_Node_SetTextString(activeControllerText, displayText);
}

static void C4_MenuScreen_ExitGame(void* context) {
    (void)context;
    C4_PushEvent_CloseWindow();
}

static void C4_MenuScreen_HandleEvent(C4_UI_Screen* screen, SDL_Event* event, float UIScale) {
    if (!screen || !event) {
        return;
    }
    C4_MenuScreenData* menuData = (C4_MenuScreenData*)screen->data;

    C4_UI_Screen_HandleEvent_Default(screen, event, UIScale);

    if (
        event->type == SDL_EVENT_GAMEPAD_ADDED ||
        event->type == SDL_EVENT_GAMEPAD_REMOVED
    ) {
        UpdateControllerText(menuData->controllerList, menuData->activeControllerText);
    }
}

static void C4_MenuScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen) {
        return;
    }
    C4_MenuScreenData* menuData = (C4_MenuScreenData*)screen->data;
    UpdateControllerText(menuData->controllerList, menuData->activeControllerText);
}

C4_UI_Screen* C4_MenuScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        return NULL;
    }

    screen->data = calloc(1, sizeof(C4_MenuScreenData));
    if (!screen->data) {
        screen->Destroy(screen);
        return NULL;
    }

    screen->HandleWindowResize = C4_MenuScreen_HandleWindowResize;
    screen->HandleEvent = C4_MenuScreen_HandleEvent;
    screen->OnEnter = C4_MenuScreen_OnEnter;

    if (!C4_MenuScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    C4_UI_Canvas* canvas = &screen->canvas;
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    //SDL_Renderer* renderer = game->renderer;

    data->game = game;

    const SDL_FRect DEFAULT_BUTTON_SIZE =  (SDL_FRect){0.f, 0.f, 800.f, 100.f};

    #define BUTTON_COUNT 4
    char* BUTTON_STRINGS[BUTTON_COUNT] = {
        "Singleplayer",
        "Multiplayer",
        "Settings",
        "Quit"
    };


    C4_UI_Button_Config buttonConfigs[BUTTON_COUNT];
    C4_UI_Buttons_CreateConfigArr(
        &(C4_UI_Button_Config){
            .style = &C4_UI_THEME_DEFAULT.style,
            .rect = DEFAULT_BUTTON_SIZE,
            .shapeType = C4_UI_Shape_Rectangle,
            .borderWidth = C4_UI_THEME_DEFAULT.borderWidth,
            .text = "",
            .font = game->monocraftBold,
            .textEngine = game->textEngine
        },
        BUTTON_STRINGS, BUTTON_COUNT, buttonConfigs
    );

    C4_UI_Node* buttons = C4_UI_Buttons_Create(
        &(C4_UI_Buttons_Config){
            .posX = 0.f,
            .posY = 0.f,
            .direction = C4_UI_Direction_Vertical,
            .spacing = 25.f,
            .padding = 25.f,
            .buttonsArr = buttonConfigs,
            .buttonsArrSize = BUTTON_COUNT
        }
    );
    buttons->shape = (C4_UI_Data_Shape){
        .borderWidth = 3,
        .rotationDegrees = 0,
        .type = C4_UI_Shape_Rectangle
    };
    buttons->input.OnCancel = C4_MenuScreen_ExitGame;
    C4_UI_CenterInWindow(buttons, C4_UI_Axis_XY, game->presentationWidth, game->presentationHeight, game->UIScale);
    C4_UI_Node_AlignChildren(buttons, C4_UI_Axis_X);
    C4_UI_Canvas_AddNode(canvas, buttons);

    data->controllerList = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = &C4_UI_THEME_DEFAULT.style,
            .text = (C4_UI_Data_Text_Config){
                .posX = 0.f,
                .posY = 0.f,
                .text = "",
                .font = game->monocraftRegular,
                .textEngine = game->textEngine
            }
        }
    );
    C4_UI_Canvas_AddNode(canvas, data->controllerList);

    data->activeControllerText = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = &C4_UI_THEME_DEFAULT.style,
            .text = (C4_UI_Data_Text_Config){
                .posX = 0.f,
                .posY = 1000.f,
                .text = "",
                .font = game->monocraftRegular,
                .textEngine = game->textEngine
            }
        }
    );
    C4_UI_Canvas_AddNode(canvas, data->activeControllerText);
    
    UpdateControllerText(data->controllerList, data->activeControllerText);

    screen->HandleWindowResize(screen, game->currentLayout);
    
    return true;
}