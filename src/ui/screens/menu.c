#include "Connect4/ui/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include "Connect4/ui/element/button.h"
#include "Connect4/ui/utils.h"
#include "Connect4/assets/fonts.h"

typedef struct {
    C4_Game* game;
    C4_UI_Node* buttons;
    C4_UI_Node* controllerInfo;
    C4_UI_Node* title;
} C4_MenuScreenData;

static void C4_MenuScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        SDL_Log("Unable to resize menu ui. One or more required pointers are NULL.");
        return;
    }
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    C4_Game* game = data->game;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            
        }; break;
        case C4_UI_LayoutType_Tall: {

        }; break;
        default: break;
    }

    SDL_FPoint refWindowDim = C4_GetReferenceWindowDimensions(game->windowWidth, game->windowHeight, game->UIScale);

    C4_UI_Buttons_SetChildrenButtonSizes(data->buttons, C4_FMin(refWindowDim.x - 60, 800.f), 100.f);
    C4_UI_CenterInWindow(data->buttons, C4_UI_Axis_XY, (unsigned int)refWindowDim.x, (unsigned int)refWindowDim.y);
    C4_UI_Node_AlignChildren(data->buttons, C4_UI_Axis_X);

    C4_UI_AlignInWindow(data->controllerInfo, C4_UI_Align_BottomLeft, (unsigned int)refWindowDim.x, (unsigned int)refWindowDim.y);

    // float bounceMultiplier = C4_GetBPMBounce(85);
    C4_UI_CenterInWindow(data->title, C4_UI_Axis_X, (unsigned int)refWindowDim.x, (unsigned int)refWindowDim.y);
}

// Forward declaration just for the purpose of organization
static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game);

static void UpdateControllerText(C4_MenuScreenData* data) {
    char displayText[2048] = "Controllers:\n";

    char* controllerArr[C4_MAX_GAMEPADS];
    C4_Gamepad_GetNames(controllerArr, C4_MAX_GAMEPADS);
    char* controllerText = C4_JoinStrings((const char**)controllerArr, C4_MAX_GAMEPADS, "\n");
    
    if (controllerText && *controllerText != '\0') {
        strcat(displayText, controllerText);
    } else {
        strcat(displayText, "None");
    }

    if (controllerText) {
        SDL_free(controllerText);
    }

    strcat(displayText, "\n\nCurrent: ");

    char activeController[128];
    C4_Gamepad_GetActiveName(activeController, sizeof(activeController));
    strcat(displayText, activeController);
    
    C4_UI_Node_SetTextString(data->controllerInfo, displayText);
    SDL_FPoint refWindowDim = C4_GetReferenceWindowDimensions(data->game->windowWidth, data->game->windowHeight, data->game->UIScale);
    C4_UI_AlignInWindow(data->controllerInfo, C4_UI_Align_BottomLeft, (unsigned int)refWindowDim.x, (unsigned int)refWindowDim.y);
}

static void C4_MenuScreen_CloseWindow(void* context) {
    (void)context;
    C4_PushEvent_CloseWindow(false);
}

static void C4_MenuScreen_CloseWindowAndTask(void* context) {
    (void)context;
    C4_PushEvent_CloseWindow(true);
}

static void C4_MenuScreen_HandleEvent(C4_UI_Screen* screen, SDL_Event* event, float UIScale) {
    if (!screen || !event) {
        return;
    }
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;

    C4_UI_Screen_HandleEvent_Default(screen, event, UIScale);

    if (
        event->type == SDL_EVENT_GAMEPAD_ADDED ||
        event->type == SDL_EVENT_GAMEPAD_REMOVED
    ) {
        UpdateControllerText(data);
    }
}

static void C4_MenuScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen) {
        return;
    }
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    UpdateControllerText(data);
}

C4_UI_Screen* C4_MenuScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer, game->textEngine);
    if (!screen) {
        return NULL;
    }

    screen->data = SDL_calloc(1, sizeof(C4_MenuScreenData));
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
    float UIScale = game->UIScale;

    data->title = C4_UI_Node_Create(
        &canvas->arena, &(C4_UI_Node_Config) {
            .type = C4_UI_Type_Text,
            .style = &C4_UI_THEME_DEFAULT.style,
            .text = &(C4_UI_Data_Text_Config) {
                .posX = 0.f,
                .posY = 0.f,
                .UIScale = UIScale,
                .text = "Connect4",
                .font = C4_GetFont(C4_FONT_ASSET_MONOCRAFT, 200.f, TTF_STYLE_BOLD),
                .textEngine = game->textEngine
            }
        }
    );
    C4_UI_Canvas_AddNode(canvas, data->title);

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
            .rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f},
            .UIScale = UIScale,
            .shapeType = C4_UI_Shape_Rectangle,
            .borderWidth = C4_UI_THEME_DEFAULT.borderWidth,
            .text = "",
            .font = C4_GetFont(C4_FONT_ASSET_MONOCRAFT, 48.f, TTF_STYLE_BOLD),
            .textEngine = game->textEngine
        },
        BUTTON_STRINGS, BUTTON_COUNT, buttonConfigs
    );

    data->buttons = C4_UI_Buttons_Create(
        &canvas->arena, &(C4_UI_Buttons_Config){
            .posX = 0.f,
            .posY = 0.f,
            .UIScale = UIScale,
            .direction = C4_UI_Direction_Vertical,
            .spacing = 25.f,
            .padding = 25.f,
            .buttonsArr = buttonConfigs,
            .buttonsArrSize = BUTTON_COUNT
        }
    );
    data->buttons->shape = (C4_UI_Data_Shape){
        .borderWidth = 3,
        .rotationDegrees = 0,
        .type = C4_UI_Shape_Rectangle
    };
    data->buttons->input.OnCancel = C4_MenuScreen_CloseWindow;
    data->buttons->lastChild->input.OnPress = C4_MenuScreen_CloseWindowAndTask;
    C4_UI_Canvas_AddNode(canvas, data->buttons);

    data->controllerInfo = C4_UI_Node_Create(
        &canvas->arena,
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = &C4_UI_THEME_DEFAULT.style,
            .text = &(C4_UI_Data_Text_Config){
                .posX = 0.f,
                .posY = 0.f,
                .text = "",
                .font = C4_GetFont(C4_FONT_ASSET_MONOCRAFT, 32.f, TTF_STYLE_NORMAL),
                .textEngine = game->textEngine
            }
        }
    );
    C4_UI_Canvas_AddNode(canvas, data->controllerInfo);
    
    UpdateControllerText(data);
    
    return true;
}