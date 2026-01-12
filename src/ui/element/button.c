#include "Connect4/ui/element/button.h"

C4_UI_Node* C4_UI_Button_Create(C4_UI_Button_Config* config) {
    if (!config) {
        SDL_Log("Unable to create button. Config is NULL");
        return NULL;
    }
    C4_UI_Node* background = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Shape,
            .style = config->style,
            .shape = (C4_UI_Data_Shape_Config){
                .rect = config->rect,
                .type = config->shapeType,
                .borderWidth = config->borderWidth
            }
        }
    );
    background->input.isFocusable = true;
    background->input.isActive = true;
    background->childrenAlign = C4_UI_Align_Center;
    background->input.sounds.onHover = C4_SoundEffect_ButtonHover;
    background->input.sounds.onPress = C4_SoundEffect_ButtonClick;

    C4_UI_Node* text = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = config->style,
            .text = (C4_UI_Data_Text_Config){
                .posX = 0.f,
                .posY = 0.f,
                .font = config->font,
                .text = config->text,
                .textEngine = config->textEngine
            }
        }
    );
    text->inheritState = true;

    C4_UI_Node_AttachChild(background, text);

    C4_UI_Node_AlignChildren(background, C4_UI_Axis_XY);
    
    return background; 
}

C4_UI_Node* C4_UI_Buttons_Create(C4_UI_Buttons_Config* config) {
    if (!config || !config->buttonsArr || config->buttonsArrSize == 0) {
        SDL_Log("Unable to create buttons. One or more required values are NULL or 0");
        return NULL;
    }

    C4_UI_Node* container = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Shape,
            .style = config->buttonsArr[0].style
        }
    );
    container->direction = config->direction;
    container->spacing = config->spacing;
    container->padding = config->padding;
    container->rect = (SDL_FRect){config->posX, config->posY, 0.f, 0.f};
    container->childrenAlign = C4_UI_Align_Center;

    for (size_t i = 0; i < config->buttonsArrSize; i++) {
        C4_UI_Node* btn = C4_UI_Button_Create(&config->buttonsArr[i]);
        C4_UI_Node_AttachChild(container, btn);
        if (container->direction == C4_UI_Direction_Horizontal) {
            if (btn->prevSibling) {
                btn->prevSibling->navRight = btn;
                btn->navLeft = btn->prevSibling;
            }
        } else {
            if (btn->prevSibling) {
                btn->prevSibling->navDown = btn;
                btn->navUp = btn->prevSibling;
            }
        }
    }

    C4_UI_Node_ApplyChildSpacing(container);

    return container;
}

void C4_UI_Buttons_CreateConfigArr(C4_UI_Button_Config* defaultConfig, char* buttonStrings[], size_t buttonCount, C4_UI_Button_Config returnValue[]) {
    if (!defaultConfig || !buttonStrings) {
        SDL_Log("Unable to create config arr. One or more required pointers are NULL.");
        return;
    }
    for (size_t i = 0; i < buttonCount; i++) {
        returnValue[i] = *defaultConfig;
        returnValue[i].text = buttonStrings[i];
    }
}