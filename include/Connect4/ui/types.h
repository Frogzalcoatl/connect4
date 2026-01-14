#pragma once
#include "SDL3_ttf/SDL_ttf.h"

typedef enum C4_UI_Align {
    C4_UI_Align_TopLeft,
    C4_UI_Align_Top,
    C4_UI_Align_TopRight,
    C4_UI_Align_CenterLeft,
    C4_UI_Align_Center,
    C4_UI_Align_CenterRight,
    C4_UI_Align_BottomLeft,
    C4_UI_Align_Bottom,
    C4_UI_Align_BottomRight
} C4_UI_Align;

typedef enum {
    C4_UI_Axis_X,
    C4_UI_Axis_Y,
    C4_UI_Axis_XY
} C4_UI_Axis;

typedef enum {
    C4_UI_Type_Container,
    C4_UI_Type_Text,
    C4_UI_Type_Shape
} C4_UI_Type;

typedef enum {
    C4_UI_Shape_Rectangle = 1,
    C4_UI_Shape_Triangle,
    C4_UI_Shape_Ellipse
} C4_UI_ShapeType;

typedef struct {
    TTF_Font* font;
    TTF_Text* textObject;
    char* storage;
} C4_UI_Data_Text;

typedef struct {
    C4_UI_ShapeType type;
    float rotationDegrees;
    unsigned int borderWidth;
} C4_UI_Data_Shape;

typedef enum {
    C4_UI_Direction_Horizontal,
    C4_UI_Direction_Vertical
} C4_UI_Direction;

typedef enum {
    C4_UI_Mirroring_None,
    C4_UI_Mirroring_X,
    C4_UI_Mirroring_Y,
    C4_UI_Mirroring_XY
} C4_UI_Mirroring;