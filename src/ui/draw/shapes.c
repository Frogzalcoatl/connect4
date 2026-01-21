#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/utils.h"
#include "Connect4/ui/draw/rectangle.h"
#include "Connect4/ui/draw/triangle.h"
#include "Connect4/ui/draw/ellipse.h"
#include <assert.h>

void C4_UI_DrawShape(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer, float UIScale) {
    assert(shape && styleState && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    switch (shape->type) {
        case C4_UI_ShapeType_Rectangle: {
            C4_UI_DrawRectangle(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawRectangleBorders(rect, shape, styleState, mirror, renderer, UIScale);
        }; break;
        case C4_UI_ShapeType_Triangle: {
            C4_UI_DrawTriangle(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawTriangleBorders(rect, shape, styleState, mirror, renderer, UIScale);
        }; break;
        case C4_UI_ShapeType_Ellipse: {
            C4_UI_DrawEllipse(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawEllipseBorders(rect, shape, styleState, mirror, renderer, UIScale);
        }; break;
        default: break;
    }
}