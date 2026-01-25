#include "Connect4/ui/draw/ellipse.h"
#include "Connect4/ui/draw/utils.h"
#include <assert.h>

// Im ngl this entire file is vibecoded. I was NOT in the mood to figure out the math for drawing circles myself.
// It doesnt look like it wouldve been too difficult but idk. I wanna move on to actually working on this game.

#define MAX_ELLIPSE_SEGMENTS 512
#define MAX_VERTICES (MAX_ELLIPSE_SEGMENTS + 2) // +1 center, +1 to close loop
#define MAX_INDICES  (MAX_ELLIPSE_SEGMENTS * 3)

static SDL_Vertex vertexCache[MAX_VERTICES];
static int indexCache[MAX_INDICES];

static int GetEllipseSegments(float radiusX, float radiusY) {
    float maxRadius = fmaxf(radiusX, radiusY);
    return (int)fmaxf(24.0f, fminf(512.0f, maxRadius * 2.0f));
}

void C4_UI_DrawEllipse(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer) {
    assert(shape && styleState && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);

    float radiusX = rect.w / 2.0f;
    float radiusY = rect.h / 2.0f;
    float centerX = rect.x + radiusX;
    float centerY = rect.y + radiusY;

    int segments = GetEllipseSegments(radiusX, radiusY);

    if (segments > MAX_ELLIPSE_SEGMENTS) {
        segments = MAX_ELLIPSE_SEGMENTS;
    }

    int vertexCount = segments + 1;
    int indexCount = segments * 3;

    SDL_FColor color;
    C4_ColorToFColor(&styleState->background, &color);

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);

    // Center Vertex
    vertexCache[0] = (SDL_Vertex){
        .position = {centerX, centerY},
        .color = color,
        .tex_coord = {0.0f, 0.0f}
    };

    for (int i = 0; i < segments; i++) {
        const float angle = (float)i / (float)segments * 2.0f * (float)M_PI;
        
        float vertexX = centerX + radiusX * SDL_cosf(angle);
        float vertexY = centerY + radiusY * SDL_sinf(angle);

        vertexCache[i + 1] = (SDL_Vertex){
            .position = {vertexX, vertexY},
            .color = color,
            .tex_coord = {0.0f, 0.0f}
        };

        vertexCache[i + 1].position = C4_UI_RotatePoint(
            vertexCache[i + 1].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        int current = i + 1;
        int next = (i + 1) % segments + 1;
        
        indexCache[i * 3 + 0] = 0;
        indexCache[i * 3 + 1] = current;
        indexCache[i * 3 + 2] = next;
    }

    SDL_RenderGeometry(renderer, NULL, vertexCache, vertexCount, indexCache, indexCount);
}

#define MAX_BORDER_VERTICES ((MAX_ELLIPSE_SEGMENTS + 1) * 2) 
#define MAX_BORDER_INDICES  (MAX_ELLIPSE_SEGMENTS * 6)

static SDL_Vertex borderVertexCache[MAX_BORDER_VERTICES];
static int borderIndexCache[MAX_BORDER_INDICES];

void C4_UI_DrawEllipseBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirror mirror, SDL_Renderer* renderer, float uiScale) {
    assert(shape && renderer);
    assert(mirror >= C4_UI_Mirror_None && mirror < C4_UI_Mirror_Count);
    
    if (shape->borderWidth <= 0.0f) {
        return;
    }

    int scaledBorderWidth = (int)(shape->borderWidth * uiScale);
    if (scaledBorderWidth <= 0) {
        scaledBorderWidth = 1; 
    }

    float radiusX = rect.w / 2.0f;
    float radiusY = rect.h / 2.0f;
    float centerX = rect.x + radiusX;
    float centerY = rect.y + radiusY;
    float halfWidth = scaledBorderWidth / 2.0f;

    int segments = GetEllipseSegments(radiusX, radiusY);
    if (segments > MAX_ELLIPSE_SEGMENTS) {
        segments = MAX_ELLIPSE_SEGMENTS;
    }
    
    int vertexCount = (segments + 1) * 2; 
    int indexCount = segments * 6;

    SDL_FColor color;
    C4_ColorToFColor(&styleState->border, &color);

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);

    for (int i = 0; i <= segments; i++) {
        float angle = (float)(i % segments) / (float)segments * 2.0f * (float)M_PI;
        
        float angleCos = SDL_cosf(angle);
        float angleSin = SDL_sinf(angle);

        float pointX = centerX + radiusX * angleCos;
        float pointY = centerY + radiusY * angleSin;

        float normalX = radiusY * angleCos;
        float normalY = radiusX * angleSin;
        float length = sqrtf(normalX * normalX + normalY * normalY);
        
        if (length > 0.0f) {
            normalX /= length;
            normalY /= length;
        }

        borderVertexCache[i * 2 + 0] = (SDL_Vertex){
            .position = {pointX + normalX * halfWidth, pointY + normalY * halfWidth},
            .color = color
        };
        borderVertexCache[i * 2 + 1] = (SDL_Vertex){
            .position = {pointX - normalX * halfWidth, pointY - normalY * halfWidth},
            .color = color
        };

        borderVertexCache[i * 2 + 0].position = C4_UI_RotatePoint(
            borderVertexCache[i * 2 + 0].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        borderVertexCache[i * 2 + 1].position = C4_UI_RotatePoint(
            borderVertexCache[i * 2 + 1].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        if (i < segments) {
            int base = i * 2;
            // First Triangle
            borderIndexCache[i * 6 + 0] = base + 0;
            borderIndexCache[i * 6 + 1] = base + 1;
            borderIndexCache[i * 6 + 2] = base + 2;
            
            // Second Triangle
            borderIndexCache[i * 6 + 3] = base + 1;
            borderIndexCache[i * 6 + 4] = base + 3;
            borderIndexCache[i * 6 + 5] = base + 2;
        }
    }

    SDL_RenderGeometry(renderer, NULL, borderVertexCache, vertexCount, borderIndexCache, indexCount);
}