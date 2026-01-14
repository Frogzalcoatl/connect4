#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/utils.h"
#include "Connect4/ui/draw/rectangle.h"
#include "Connect4/ui/draw/triangle.h"
#include "Connect4/ui/draw/ellipse.h"

void C4_UI_DrawShape(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!shape || !styleState || !renderer) {
        SDL_Log("Unable to draw shape. One or more required pointers are NULL");
        return;
    }
    switch (shape->type) {
        case C4_UI_Shape_Rectangle: {
            C4_UI_DrawRectangle(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawRectangleBorders(rect, shape, styleState, mirror, renderer);
        }; break;
        case C4_UI_Shape_Triangle: {
            C4_UI_DrawTriangle(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawTriangleBorders(rect, shape, styleState, mirror, renderer);
        }; break;
        case C4_UI_Shape_Ellipse: {
            C4_UI_DrawEllipse(rect, shape, styleState, mirror, renderer);
            C4_UI_DrawEllipseBorders(rect, shape, styleState, mirror, renderer);
        }; break;
    }
}