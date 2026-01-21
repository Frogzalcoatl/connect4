#include "Connect4/ui/element/button.h"
#include <assert.h>

C4_UI_Node* C4_UI_Button_Create(C4_MemoryArena* arena, C4_UI_Button_Config* config) {
    assert(arena && config);
    
    C4_UI_Node* background = C4_UI_Node_Create(
        arena, &(C4_UI_Node_Config){
            .type = C4_UI_Type_Shape,
            .style = config->style,
            .shape = &(C4_UI_Data_Shape_Config){
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
        arena, &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = config->style,
            .text = &(C4_UI_Data_Text_Config){
                .posX = 0.f,
                .posY = 0.f,
                .UIScale = config->UIScale,
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

C4_UI_Node* C4_UI_Buttons_Create(C4_MemoryArena* arena, C4_UI_Buttons_Config* config) {
    assert(arena && config && config->buttonsArr && config->buttonsArrSize > 0);

    C4_UI_Node* container = C4_UI_Node_Create(
        arena, &(C4_UI_Node_Config){
            .type = C4_UI_Type_Container,
            .style = config->buttonsArr[0].style
        }
    );
    container->direction = config->direction;
    container->spacing = config->spacing;
    container->padding = config->padding;
    container->rect = (SDL_FRect){config->posX, config->posY, 0.f, 0.f};
    container->childrenAlign = C4_UI_Align_Center;

    for (size_t i = 0; i < config->buttonsArrSize; i++) {
        config->buttonsArr[i].UIScale = config->UIScale; 
        C4_UI_Node* btn = C4_UI_Button_Create(arena, &config->buttonsArr[i]);
        
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

void C4_UI_Buttons_SetChildrenButtonSizes(C4_UI_Node* node, float width, float height) {
    assert(node);

    C4_UI_Node* current = node->firstChild;
    while (current) {
        current->rect.w = width;
        current->rect.h = height;
        C4_UI_Node_AlignChildren(current, C4_UI_Axis_XY);
        current = current->nextSibling;
    }
    C4_UI_Node_ApplyChildSpacing(node);
}

void C4_UI_Buttons_CreateConfigArr(C4_UI_Button_Config* defaultConfig, char* buttonStrings[], size_t buttonCount, C4_UI_Button_Config returnValue[]) {
    assert(defaultConfig && buttonStrings && returnValue);
    
    for (size_t i = 0; i < buttonCount; i++) {
        returnValue[i] = *defaultConfig;
        returnValue[i].text = buttonStrings[i];
    }
}