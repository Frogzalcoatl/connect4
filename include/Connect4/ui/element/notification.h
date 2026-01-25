#pragma once
#include "Connect4/ui/node.h"
#include "Connect4/ui/canvas.h"

C4_UI_Node* C4_UI_Notification_AddContainerToCanvas(C4_UI_Canvas* canvas, C4_UI_Align notificationAlign, float uiScale);

typedef struct C4_UI_Notification_Send_Config {
    C4_UI_Node* containerNode;
    float width;
    float height;
    const char* string;
    float fadeInSeconds;
} C4_UI_Notification_Send_Config;
void C4_UI_Notification_Send(C4_UI_Node* node, float width, float height, const char* string, float fadeInSeconds, float fadeOutSeconds);