#include "Connect4/ui/draw/ellipse.h"
#include "Connect4/ui/draw/utils.h"

// Im ngl this entire file is vibecoded. I was NOT in the mood to figure out the math for drawing circles myself.
// It doesnt look like it wouldve been too difficult but idk. I wanna move on to actually working on this game.

static int GetEllipseSegments(float radiusX, float radiusY) {
    float maxRadius = fmaxf(radiusX, radiusY);
    return (int)fmaxf(24.0f, fminf(512.0f, maxRadius * 2.0f));
}

void C4_UI_DrawEllipse(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!shape || !renderer) {
        return;
    }

    float radiusX = rect.w / 2.0f;
    float radiusY = rect.h / 2.0f;
    float centerX = rect.x + radiusX;
    float centerY = rect.y + radiusY;

    int segments = GetEllipseSegments(radiusX, radiusY);

    int vertexCount = segments + 1;
    int indexCount = segments * 3;
    
    SDL_Vertex *vertices = (SDL_Vertex*)SDL_calloc(vertexCount, sizeof(SDL_Vertex));
    int *indices = (int*)SDL_calloc(indexCount, sizeof(int));
    
    if (!vertices || !indices) {
        SDL_free(vertices);
        SDL_free(indices);
        return;
    }

    SDL_FColor color;
    C4_ColorToFColor(&styleState->background, &color);

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);

    // Center Vertex
    vertices[0] = (SDL_Vertex){
        .position = {centerX, centerY},
        .color = color,
        .tex_coord = {0.0f, 0.0f}
    };

    for (int i = 0; i < segments; i++) {
        const float angle = (float)i / (float)segments * 2.0f * (float)M_PI;
        
        float vertexX = centerX + radiusX * cosf(angle);
        float vertexY = centerY + radiusY * sinf(angle);

        vertices[i + 1] = (SDL_Vertex){
            .position = {vertexX, vertexY},
            .color = color,
            .tex_coord = {0.0f, 0.0f}
        };

        vertices[i + 1].position = C4_UI_RotatePoint(
            vertices[i + 1].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        int current = i + 1;
        int next = (i + 1) % segments + 1;
        
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = current;
        indices[i * 3 + 2] = next;
    }

    SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);

    SDL_free(vertices);
    SDL_free(indices);
    return;
}

void C4_UI_DrawEllipseBorders(SDL_FRect rect, C4_UI_Data_Shape* shape, C4_UI_StyleState* styleState, C4_UI_Mirroring mirror, SDL_Renderer* renderer) {
    if (!renderer || !shape || shape->borderWidth <= 0.0f) {
        return;
    }

    float radiusX = rect.w / 2.0f;
    float radiusY = rect.h / 2.0f;
    float centerX = rect.x + radiusX;
    float centerY = rect.y + radiusY;
    float halfWidth = shape->borderWidth / 2.0f;

    int segments = GetEllipseSegments(radiusX, radiusY);
    
    int vertexCount = (segments + 1) * 2; 
    int indexCount = segments * 6;

    SDL_Vertex *vertices = (SDL_Vertex*)SDL_calloc(vertexCount, sizeof(SDL_Vertex));
    int *indices = (int*)SDL_calloc(indexCount, sizeof(int));

    if (!vertices || !indices) {
        SDL_free(vertices);
        SDL_free(indices);
        return;
    }

    SDL_FColor color;
    C4_ColorToFColor(&styleState->border, &color);

    float effectiveRotation = C4_UI_GetMirroredRotation((float)shape->rotationDegrees, mirror);

    for (int i = 0; i <= segments; i++) {
        float angle = (float)(i % segments) / (float)segments * 2.0f * (float)M_PI;
        
        float angleCos = cosf(angle);
        float angleSin = sinf(angle);

        float pointX = centerX + radiusX * angleCos;
        float pointY = centerY + radiusY * angleSin;

        float normalX = radiusY * angleCos;
        float normalY = radiusX * angleSin;
        float length = sqrtf(normalX * normalX + normalY * normalY);
        
        if (length > 0.0f) {
            normalX /= length;
            normalY /= length;
        }

        vertices[i * 2 + 0] = (SDL_Vertex){
            .position = {pointX + normalX * halfWidth, pointY + normalY * halfWidth},
            .color = color
        };
        vertices[i * 2 + 1] = (SDL_Vertex){
            .position = {pointX - normalX * halfWidth, pointY - normalY * halfWidth},
            .color = color
        };

        vertices[i * 2 + 0].position = C4_UI_RotatePoint(
            vertices[i * 2 + 0].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        vertices[i * 2 + 1].position = C4_UI_RotatePoint(
            vertices[i * 2 + 1].position,
            (SDL_FPoint){centerX, centerY},
            effectiveRotation
        );

        if (i < segments) {
            int base = i * 2;
            // First Triangle
            indices[i * 6 + 0] = base + 0;
            indices[i * 6 + 1] = base + 1;
            indices[i * 6 + 2] = base + 2;
            
            // Second Triangle
            indices[i * 6 + 3] = base + 1;
            indices[i * 6 + 4] = base + 3;
            indices[i * 6 + 5] = base + 2;
        }
    }

    SDL_RenderGeometry(renderer, NULL, vertices, vertexCount, indices, indexCount);

    SDL_free(vertices);
    SDL_free(indices);
    return;
}