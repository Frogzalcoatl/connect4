#include "Connect4/ui/draw/rectangle.h"
#include "Connect4/ui/draw/utils.h"

void C4_UI_DrawRectangle(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!shape || !styleState || !renderer) {
        SDL_Log("Unable to draw rectangle. One or more required pointers are NULL");
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

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);
    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 4; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, effectiveRotation);
    }

    // The order in which itll draw the indices. Basically this makes two triangles to form a rectangle
    int indices[] = {0, 1, 2, 0, 2, 3};

    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);
}

void C4_UI_DrawRectangleBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!shape || !styleState || !renderer) {
        SDL_Log("Unable to draw rectangle borders. One or more required pointers are NULL");
        return;
    }

    if (shape->borderWidth == 0) {
        return;
    }

    int scaledBorderWidth = (int)(shape->borderWidth * 1.f);
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

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);
    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 8; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, effectiveRotation);
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