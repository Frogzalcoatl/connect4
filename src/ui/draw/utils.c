#include "Connect4/ui/draw/utils.h"
#include <math.h>

SDL_FPoint C4_UI_RotatePoint(SDL_FPoint point, SDL_FPoint center, float degrees) {
    if (degrees == 0) {
        return point;
    }
    float radians = degrees * ((float)M_PI / 180.f);
    float sineVal = sinf(radians);
    float cosineVal = cosf(radians);
    float localX = point.x - center.x;
    float localY = point.y - center.y;
    float rotatedX = (localX * cosineVal) - (localY * sineVal);
    float rotatedY = (localX * sineVal) + (localY * cosineVal);
    return (SDL_FPoint){rotatedX + center.x, rotatedY + center.y}; 
}

SDL_FPoint C4_UI_MirrorPoint(SDL_FPoint point, SDL_FPoint center, C4_UI_Mirror mirror) {
    if (mirror == C4_UI_Mirror_None) return point;

    float localX = point.x - center.x;
    float localY = point.y - center.y;

    if (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) {
        localX = -localX;
    }
    if (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) {
        localY = -localY;
    }

    return (SDL_FPoint){center.x + localX, center.y + localY};
}

void C4_ColorToFColor(SDL_Color* color, SDL_FColor* fColor) {
    // FColor uses a 0.0 - 1.0 range rather than 0 - 255
    fColor->r = color->r / 255.f;
    fColor->g = color->g / 255.f;
    fColor->b = color->b / 255.f;
    fColor->a = color->a / 255.f;
}

void C4_InitVertices(SDL_Vertex* vertices, size_t vertexCount, SDL_FColor fColor) {
    if (!vertices) {
        SDL_Log("Unable init shape vertices. Vertices pointer is NULL");
        return;
    }
    for (size_t i = 0; i < vertexCount; i++) {
        vertices[i].color = fColor;
        vertices[i].tex_coord = (SDL_FPoint){0.f, 0.f};
    }
}

SDL_FPoint C4_GetRectCenter(SDL_FRect rect) {
    return (SDL_FPoint){rect.x + rect.w / 2.f, rect.y + rect.h / 2.f};
}

float C4_UI_GetMirroredRotation(float degrees, C4_UI_Mirror mirror) {
    if (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_Y) {
        return -degrees;
    }
    return degrees;
}