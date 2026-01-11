#include "Connect4/game/screens/menu.h"
#include "Connect4/game/events.h"
#include "Connect4/constants.h"
#include "Connect4/ui/element/button.h"

typedef struct {
    C4_Game* game;
} C4_MenuScreenData;

static void C4_MenuScreen_OnEnter(C4_UI_Screen* screen) {
    if (!screen || !screen->data) {
        return;
    }
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
}

static void C4_MenuScreen_HandleWindowResize(C4_UI_Screen* screen, C4_UI_LayoutType layout) {
    if (!screen || !screen->data) {
        return;
    }
    C4_MenuScreenData* menuData = (C4_MenuScreenData*)screen->data;

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
    screen->OnEnter = C4_MenuScreen_OnEnter;

    if (!C4_MenuScreen_Init(screen, game)) {
        screen->Destroy(screen);
        return NULL;
    }

    return screen;
}

static void ButtonClick(void* context) {
    (void)context;
    C4_PlaySound(C4_SoundEffect_ButtonClick);
}

static void ButtonHover(void* context) {
    (void)context;
    C4_PlaySound(C4_SoundEffect_ButtonHover);
}

static bool C4_MenuScreen_Init(C4_UI_Screen* screen, C4_Game* game) {
    if (!screen || !game || !screen->data || !game->renderer) {
        return false;
    }
    C4_UI_Canvas* canvas = &screen->canvas;
    C4_MenuScreenData* data = (C4_MenuScreenData*)screen->data;
    SDL_Renderer* renderer = game->renderer;

    data->game = game;

    C4_UI_Node* singlePlayerButton = C4_UI_Button_Create(
        &(C4_UI_Button_Config){
            .style = &C4_UI_THEME_DEFAULT.style,
            .rect = (SDL_FRect){100.f, 200.f, 800.f, 100.f},
            .shapeType = C4_UI_Shape_Rectangle,
            .borderWidth = C4_UI_THEME_DEFAULT.borderWidth,
            .text = "Singleplayer",
            .font = game->monocraftBold,
            .textEngine = game->textEngine
        }, game->UIScale
    );

    C4_UI_Canvas_AddNode(canvas, singlePlayerButton);
    singlePlayerButton->input.OnHover = ButtonHover;
    singlePlayerButton->input.OnRelease = ButtonClick;

    screen->HandleWindowResize(screen, game->currentLayout);
    
    return true;
}