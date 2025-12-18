#include "Connect4/ui/elements/symbol.h"
#include <math.h>
#include <stdlib.h>

bool C4_UI_Symbol_InitProperties(C4_UI_Symbol* symbol, C4_UI_SymbolType type, SDL_FRect destination, int rotationDegrees) {
    if (!symbol) {
        SDL_Log("Unable to init symbol properties. Pointer is NULL");
        return false;
    }
    if (type < C4_UI_SymbolType_None || type >= C4_UI_SymbolType_Length) {
        SDL_Log("Unable to init symbol properties. Invalid symbol type");
        return false;
    }
    symbol->type = type;
    symbol->destination = destination;
    symbol->rotationDegrees = rotationDegrees;
    return true;
}

C4_UI_Symbol* C4_UI_Symbol_Create(C4_UI_SymbolType type, SDL_FRect destination, int rotationDegrees) {
    C4_UI_Symbol* symbol = calloc(1, sizeof(C4_UI_Symbol));
    if (!symbol) {
        SDL_Log("Unable to allocate memory for symbol element");
        return NULL;
    }
    if (!C4_UI_Symbol_InitProperties(symbol, type, destination, rotationDegrees)) {
        C4_UI_Symbol_Destroy(symbol);
        return NULL;
    }
    return symbol;
}

void C4_UI_Symbol_Destroy(C4_UI_Symbol* symbol) {
    if (!symbol) {
        return;
    }
    free(symbol);
}

static SDL_FPoint C4_UI_RotatePoint(SDL_FPoint point, SDL_FPoint center, float degrees) {
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

static void C4_UI_Symbol_DrawTriangle(C4_UI_Symbol* symbol, const SDL_Color color, SDL_Renderer* renderer) {
    if (!symbol) {
        return;
    }
    SDL_Vertex vertices[3];
    // FColor uses a 0.0 - 1.0 range rather than 0 - 255
    SDL_FColor fColor = {
        color.r / 255.f,
        color.g / 255.f,
        color.b / 255.f,
        color.a / 255.f
    };
    for (int i = 0; i < 3; i++) {
        vertices[i].color = fColor;
        vertices[i].tex_coord = (SDL_FPoint){0.f, 0.f};
    }
    float x = symbol->destination.x;
    float y = symbol->destination.y;
    float w = symbol->destination.w;
    float h = symbol->destination.h;

    SDL_FPoint rawPoints[3];
    rawPoints[0] = (SDL_FPoint){x + w / 2.f, y};
    rawPoints[1] = (SDL_FPoint){x, y + h};
    rawPoints[2] = (SDL_FPoint){x + w, y + h};

    SDL_FPoint center = {x + w / 2.f, y + h / 2.f};

    for (int i = 0; i < 3; i++) {
        vertices[i].position = C4_UI_RotatePoint(rawPoints[i], center, (float)symbol->rotationDegrees);
    }

    SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);
}

void C4_UI_Symbol_Draw(C4_UI_Symbol* symbol, const SDL_Color color, SDL_Renderer* renderer) {
    if (!symbol) {
        SDL_Log("Tried to draw NULL symbol element. Ignoring");
        return;
    }
    if (!renderer) {
        SDL_Log("Unable to draw symbol element. SDL Renderer is NULL");
        return;
    }
    switch (symbol->type) {
        case C4_UI_SymbolType_Triangle: {
            C4_UI_Symbol_DrawTriangle(symbol, color, renderer);
        }; break;
        default: break;
    }
}