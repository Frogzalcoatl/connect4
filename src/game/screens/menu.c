#include "Connect4/game/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"

typedef struct {
    C4_Game* game;
    C4_UI_Popup* inDevelopment;
    C4_UI_Popup* exitGame;
    C4_UI_Text* title;
    C4_UI_ButtonGroup* buttonGroup;
} C4_MenuScreenData;

static void C4_MenuScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    data->exitGame->isShowing = false;
    data->inDevelopment->isShowing = false;
}

static void C4_MenuScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        return;
    }
    C4_MenuScreenData* menuData = (C4_MenuScreenData*)screen->data;
    C4_Game* game = menuData->game;
    SDL_Renderer* renderer = game->renderer;

    switch (layout) {
        case C4_UI_LayoutType_Wide: {
            C4_Screen_UpdatePopup(
                menuData->inDevelopment, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 100.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
                    .renderer = renderer,
                }
            );

            C4_Screen_UpdatePopup(
                menuData->exitGame, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 100.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 800.f, 400.f},
                    .renderer = renderer,
                }
            );

            C4_UI_ButtonGroup_ChangeDestination(
                menuData->buttonGroup,
                (SDL_FRect){0.f, game->presentationHeight / 3.25f, 700.f, 500.f}
            );
            C4_UI_ButtonGroup_ChangePtSize(menuData->buttonGroup, 32.f, renderer);
        }; break;
        case C4_UI_LayoutType_Tall: {
            C4_Screen_UpdatePopup(
                menuData->inDevelopment, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 150.f,
                    .direction = C4_UI_ButtonGroup_Direction_Horizontal,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 900.f, 500.f},
                    .renderer = renderer,
                }
            );

            C4_Screen_UpdatePopup(
                menuData->exitGame, 
                &(C4_Screen_UpdatePopup_Config){
                    .buttonGroupHeight = 320.f,
                    .direction = C4_UI_ButtonGroup_Direction_Vertical,
                    .buttonGroupPtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .messagePtSize = C4_UI_THEME_DEFAULT.defaultPtSize,
                    .destination = (SDL_FRect){0.f, 0.f, 900.f, 600.f},
                    .renderer = renderer,
                }
            );

            C4_UI_ButtonGroup_ChangeDestination(
                menuData->buttonGroup,
                (SDL_FRect){0.f, game->presentationHeight / 3.25f, 800.f, 660.f}
            );
            C4_UI_ButtonGroup_ChangePtSize(menuData->buttonGroup, C4_UI_THEME_DEFAULT.defaultPtSize, renderer);
        }; break;
        default: break;
    }

    // Functions that are the same between both layouts
    C4_UI_Popup_CenterInWindow(
        menuData->inDevelopment, renderer,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );

    C4_UI_Popup_CenterInWindow(
        menuData->exitGame, renderer,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );

    C4_UI_ButtonGroup_CenterInWindow(
        menuData->buttonGroup, C4_Axis_X, 
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );

    C4_UI_CenterInWindow(
        &menuData->title->destination, C4_Axis_X,
        game->presentationWidth, game->presentationHeight,
        game->UIScale
    );
}

// Forward declaration just for the purpose of organization
static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game);

C4_UI_Screen* C4_MenuScreen_Create(C4_Game* game) {
    if (!game) {
        return NULL;
    }
    C4_UI_Screen* screen = C4_Screen_Create(game->renderer);
    if (!screen) {
        return NULL;
    }

    screen->data = calloc(1, sizeof(C4_MenuScreenData));
    if (!screen->data) {
        screen->Destroy(screen);
        return NULL;
    }

    screen->HandleWindowResize = C4_MenuScreen_HandleWindowResize;
    screen->OnEnter = C4_MenuScreen_OnEnter;

    if (!C4_MenuScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

static void ButtonGroup_SinglePlayer_OnRelease(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = true;
}
static void ButtonGroup_MultiPlayer_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Game);
}
static void ButtonGroup_Settings_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_ScreenChange(C4_ScreenType_Settings);
}
static void ButtonGroup_Quit_OnRelease(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = true;
}
static void ExitPopup_Ok_OnRelease(void* context) {
    (void)context;
    C4_PushEvent_CloseWindow();
}
static void ExitPopup_Cancel_OnRelease(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = false;
}
static void InDevelopmentPopup_Ok_OnRelease(void* context) {
    C4_UI_Popup* popup = (C4_UI_Popup*)context;
    popup->isShowing = false;
}

static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    C4_UI_Canvas* canvas = &screen->canvas;
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    SDL_Renderer* renderer = game->renderer;

    data->game = game;

    data->title = C4_UI_Canvas_Add_Text(
        canvas, &(C4_UI_Text_Config){
            .str = "Connect4",
            .font = game->fontBold,
            .color = C4_UI_THEME_DEFAULT.textColor,
            .ptSize = 200.f,
            .destinationX = 0.f,
            .destinationY = 0.f,
            .wrapWidth = 0
        }
    );
    if (!data->title) {
        return false;
    }

    data->inDevelopment = C4_UI_Canvas_Add_Popup(
        canvas, &(C4_UI_Popup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 1,
            .buttonGroupHeight = 100.f,
            .message = "This is still in development!",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->inDevelopment) {
        return false;
    }
    C4_UI_Button* InDevelopmentPopup_Ok = &data->inDevelopment->buttonGroup.buttons[0];
    C4_UI_Text_UpdateStr(&InDevelopmentPopup_Ok->text, "Ok", renderer);
    InDevelopmentPopup_Ok->OnReleaseCallback = InDevelopmentPopup_Ok_OnRelease;
    InDevelopmentPopup_Ok->OnReleaseContext = data->inDevelopment;

    data->exitGame = C4_UI_Canvas_Add_Popup(
        canvas, &(C4_UI_Popup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Horizontal,
            .buttonCount = 2,
            .buttonGroupHeight = 100.f,
            .message = "Do you want to exit Connect4?",
            .messageFont = game->fontRegular,
            .buttonFont = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->exitGame) {
        return false;
    }

    C4_UI_Button* ExitPopup_Ok = &data->exitGame->buttonGroup.buttons[0];
    ExitPopup_Ok->OnReleaseCallback = ExitPopup_Ok_OnRelease;
    C4_UI_Text_UpdateStr(&ExitPopup_Ok->text, "Ok", renderer);
    
    C4_UI_Button* ExitPopup_Cancel = &data->exitGame->buttonGroup.buttons[1];
    ExitPopup_Cancel->OnReleaseCallback = ExitPopup_Cancel_OnRelease;
    ExitPopup_Cancel->OnReleaseContext = data->exitGame;
    C4_UI_Text_UpdateStr(&ExitPopup_Cancel->text, "Cancel", renderer);

    data->buttonGroup = C4_UI_Canvas_Add_ButtonGroup(
        canvas, &(C4_UI_ButtonGroup_Config){
            .destination = C4_EMPTY_SDL_FRECT,
            .count = 4,
            .buttonDirection = C4_UI_ButtonGroup_Direction_Vertical,
            .margin = 15,
            .font = game->fontBold,
            .theme = &C4_UI_THEME_DEFAULT
        }
    );
    if (!data->buttonGroup) {
        return false;
    }

    C4_UI_Button* ButtonGroup_SinglePlayer = &data->buttonGroup->buttons[0];
    C4_UI_Text_UpdateStr(&ButtonGroup_SinglePlayer->text, "SinglePlayer", renderer);
    ButtonGroup_SinglePlayer->OnReleaseCallback = ButtonGroup_SinglePlayer_OnRelease;
    ButtonGroup_SinglePlayer->OnReleaseContext = data->inDevelopment;

    C4_UI_Button* ButtonGroup_MultiPlayer = &data->buttonGroup->buttons[1];
    C4_UI_Text_UpdateStr(&ButtonGroup_MultiPlayer->text, "MultiPlayer", renderer);
    ButtonGroup_MultiPlayer->OnReleaseCallback = ButtonGroup_MultiPlayer_OnRelease;

    C4_UI_Button* ButtonGroup_Settings = &data->buttonGroup->buttons[2];
    C4_UI_Text_UpdateStr(&ButtonGroup_Settings->text, "Settings", renderer);
    ButtonGroup_Settings->OnReleaseCallback = ButtonGroup_Settings_OnRelease;

    C4_UI_Button* ButtonGroup_Quit = &data->buttonGroup->buttons[3];
    C4_UI_Text_UpdateStr(&ButtonGroup_Quit->text, "Quit", renderer);
    ButtonGroup_Quit->OnReleaseCallback = ButtonGroup_Quit_OnRelease;
    ButtonGroup_Quit->OnReleaseContext = data->exitGame;

    screen->HandleWindowResize(screen, game->currentLayout);
    
    return true;
}