#include "Connect4/ui/draw/shapes.h"
#include "Connect4/ui/draw/utils.h"
#include <math.h>

static void C4_UI_DrawRectangle(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, SDL_Renderer* renderer) {
    if (!shape || !styleState || !renderer) {
        return;
    }

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->background, &fColor);

    SDL_Vertex vertices[4];
    C4_InitVertices(vertices, 4, fColor);

    SDL_FPoint rawPoints[4];
    rawPoints[0] = (SDL_FPoint){rect.x, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x + rect.w, rect.y};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};
    rawPoints[3] = (SDL_FPoint){rect.x, rect.y + rect.h};

    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 4; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, (float)shape->rotationDegrees);
    }

    // The order in which itll draw the indices. Basically this makes two triangles to form a rectangle
    int indices[] = {0, 1, 2, 0, 2, 3};

    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

static void C4_UI_DrawRectangleBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, SDL_Renderer* renderer, float UIScale) {
    if (!shape || !styleState || !renderer || shape->borderWidth == 0) {
        return;
    }

    int scaledBorderWidth = (int)(shape->borderWidth * UIScale);
    if (scaledBorderWidth < 0) {
        return;
    }

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->border, &fColor);

    SDL_Vertex vertices[8];
    C4_InitVertices(vertices, 8, fColor);

    SDL_FPoint rawPoints[8];
    // Outer points
    rawPoints[0] = (SDL_FPoint){rect.x, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x + rect.w, rect.y};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};
    rawPoints[3] = (SDL_FPoint){rect.x, rect.y + rect.h};
    // Inner Points
    float borderWidth = (float)scaledBorderWidth;
    rawPoints[4] = (SDL_FPoint){rect.x + borderWidth, rect.y + borderWidth};
    rawPoints[5] = (SDL_FPoint){rect.x + rect.w - borderWidth, rect.y + borderWidth};
    rawPoints[6] = (SDL_FPoint){rect.x + rect.w - borderWidth, rect.y + rect.h - borderWidth};
    rawPoints[7] = (SDL_FPoint){rect.x + borderWidth, rect.y + rect.h - borderWidth};

    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 8; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, (float)shape->rotationDegrees);
    }

    // The order in which itll draw the indices. Basically this makes a bunch of triangles to form the borders
    int indices[] = {
        // Top Side
        0, 1, 4,
        1, 5, 4,

        // Right Side
        1, 2, 5,
        2, 6, 5,

        // Bottom Side
        2, 3, 6,
        3, 7, 6,

        // Left Side
        3, 0, 7,
        0, 4, 7
    };

    SDL_RenderGeometry(renderer, NULL, vertices, 8, indices, 24);
}

static void C4_UI_DrawTriangle(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, SDL_Renderer* renderer) {
    if (!shape || !styleState || !renderer) {
        return;
    }

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->background, &fColor);

    SDL_Vertex vertices[3];
    C4_InitVertices(vertices, 3, fColor);

    SDL_FPoint rawPoints[3];
    rawPoints[0] = (SDL_FPoint){rect.x + rect.w / 2.f, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x, rect.y + rect.h};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};

    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 3; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, (float)shape->rotationDegrees);
    }

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

static void C4_UI_DrawTriangleBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, SDL_Renderer* renderer, float UIScale) {
    if (!shape || !styleState || !renderer || shape->borderWidth == 0) {
        return;
    }

    int scaledBorderWidth = (int)(shape->borderWidth * UIScale);
    if (scaledBorderWidth < 0) {
        return;
    }

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->border, &fColor);

    SDL_Vertex vertices[6];
    C4_InitVertices(vertices, 6, fColor);

    SDL_FPoint rawPoints[6];
    float bw = (float)scaledBorderWidth;
    // Outer Points
    rawPoints[0] = (SDL_FPoint){rect.x + rect.w / 2.f, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x, rect.y + rect.h};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};
    // Inner Points
    rawPoints[3] = (SDL_FPoint){rawPoints[0].x, rawPoints[0].y + bw};
    rawPoints[4] = (SDL_FPoint){rawPoints[1].x + bw, rawPoints[1].y - bw};
    rawPoints[5] = (SDL_FPoint){rawPoints[2].x - bw, rawPoints[2].y - bw};

    SDL_FPoint center = C4_GetRectCenter(rect);
    
    for (int i = 0; i < 6; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, (float)shape->rotationDegrees);
    }

    int indices[] = {
        // Left Side
        0, 1, 4,
        0, 4, 3,

        // Bottom Side
        1, 2, 5,
        1, 5, 4,

        // Right Side
        2, 0, 3,
        2, 3, 5
    };

    SDL_RenderGeometry(renderer, NULL, vertices, 6, indices, 18);
}

void C4_UI_DrawShape(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, SDL_Renderer* renderer, float UIScale) {
    if (!shape || !styleState || !renderer) {
        return;
    }
    switch (shape->type) {
        case C4_UI_Shape_Rectangle: {
            C4_UI_DrawRectangle(rect, shape, styleState, renderer);
            C4_UI_DrawRectangleBorders(rect, shape, styleState, renderer, UIScale);
        }; break;
        case C4_UI_Shape_Triangle: {
            C4_UI_DrawTriangle(rect, shape, styleState, renderer);
            C4_UI_DrawTriangleBorders(rect, shape, styleState, renderer, UIScale);
        }; break;
        case C4_UI_Shape_Circle: {

        }; break;
    }
}