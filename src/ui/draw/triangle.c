#include "Connect4/ui/draw/triangle.h"
#include "Connect4/ui/draw/utils.h"
#include <assert.h>

void C4_UI_DrawTriangle(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer) {
    assert(shape && styleState && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->background, &fColor);

    SDL_Vertex vertices[3];
    C4_InitVertices(vertices, 3, fColor);

    SDL_FPoint rawPoints[3];
    rawPoints[0] = (SDL_FPoint){rect.x, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x, rect.y + rect.h};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);
    SDL_FPoint center = C4_GetRectCenter(rect);

    for (int i = 0; i < 3; i++) {
        SDL_FPoint point = C4_UI_MirrorPoint(rawPoints[i], center, mirror);
        vertices[i].position = C4_UI_RotatePoint(point, center, effectiveRotation);
    }

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void C4_UI_DrawTriangleBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer, float uiScale) {
    assert(shape && styleState && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    if (shape->borderWidth == 0) {
        return;
    }

    float bw = ceilf(shape->borderWidth * uiScale);
    if (bw <= 0) {
        bw = 1;
    }

    SDL_FColor fColor;
    C4_ColorToFColor(&styleState->border, &fColor);

    SDL_Vertex vertices[6];
    C4_InitVertices(vertices, 6, fColor);

    SDL_FPoint rawPoints[6];
    // Outer Points
    rawPoints[0] = (SDL_FPoint){rect.x, rect.y};
    rawPoints[1] = (SDL_FPoint){rect.x, rect.y + rect.h};
    rawPoints[2] = (SDL_FPoint){rect.x + rect.w, rect.y + rect.h};
    // Inner Points
    rawPoints[3] = (SDL_FPoint){rawPoints[0].x + bw, rawPoints[0].y + bw};
    rawPoints[4] = (SDL_FPoint){rawPoints[1].x + bw, rawPoints[1].y - bw};
    rawPoints[5] = (SDL_FPoint){rawPoints[2].x - bw, rawPoints[2].y - bw};

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);
    SDL_FPoint center = C4_GetRectCenter(rect);
    
    for (int i = 0; i < 6; i++) {
        SDL_FPoint point = C4_UI_MirrorPoint(rawPoints[i], center, mirror);
        vertices[i].position = C4_UI_RotatePoint(point, center, effectiveRotation);
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