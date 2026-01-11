#include "Connect4/ui/element/button.h"

C4_UI_Node* C4_UI_Button_Create(C4_UI_Button_Config* config, float UIScale) {
    if (!config) {
        return NULL;
    }
    C4_UI_Node* background = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Shape,
            .style = config->style,
            .rect = config->rect,
            .shape = (C4_UI_Data_Shape_Config){
                .type = config->shapeType,
                .borderWidth = config->borderWidth
            }
        }, UIScale
    );
    background->input.isFocusable = true;
    background->input.isActive = true;

    C4_UI_Node* text = C4_UI_Node_Create(
        &(C4_UI_Node_Config){
            .type = C4_UI_Type_Text,
            .style = config->style,
            .rect = (SDL_FRect){0.f, 0.f, 0.f, 0.f},
            .text = (C4_UI_Data_Text_Config){
                .font = config->font,
                .text = config->text,
                .textEngine = config->textEngine
            }
        }, UIScale
    );
    text->inheritState = true;

    C4_UI_Node_AttachChild(background, text);

    C4_UI_Node_AlignChildren(background, C4_UI_Align_Center);
    
    return background; 
}