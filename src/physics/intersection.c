#include "Connect4/physics/intersection.h"
#include "Connect4/ui/draw/utils.h"
#include <math.h>

bool C4_IsPointInsideRectangle(SDL_FPoint point, SDL_FRect rectangleBounds, float rotationDegrees, C4_UI_Mirror mirror) {    
    float centerX = rectangleBounds.x + rectangleBounds.w / 2.f;
    float centerY = rectangleBounds.y + rectangleBounds.h / 2.f;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    // Rotate the point backwards to be able to just check basic rectangle collision
    float effectiveRotation = C4_UI_GetMirroredRotation(rotationDegrees, mirror);
    float angleRadians = -effectiveRotation * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    /*
    Is you ever add something that can make rectangles non symmetrical
    if (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) {
        localX = -localX;
    }
    if (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) {
        localY = -localY;
    }
    */

    // The rectangles local center is (0, 0), so i just check if the point is within half its size in every direction
    return (
        -rectangleBounds.w / 2.f <= localX &&
        rectangleBounds.w / 2.f >= localX &&
        -rectangleBounds.h / 2.f <= localY &&
        rectangleBounds.h / 2.f >= localY
    );
}

bool C4_IsPointInsideTriangle(SDL_FPoint point, SDL_FRect triangleBounds, float rotationDegrees, C4_UI_Mirror mirror) {
    if (triangleBounds.w == 0 || triangleBounds.h == 0) {
        return false;
    }

    float centerX = triangleBounds.x + triangleBounds.w / 2.f;
    float centerY = triangleBounds.y + triangleBounds.h / 2.f;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    float effectiveRotation = C4_UI_GetMirroredRotation(rotationDegrees, mirror);
    float angleRadians = -effectiveRotation * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    if (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) {
        localX = -localX;
    }
    if (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) {
        localY = -localY;
    }

    // Shifting coordinates to the corner
    float checkX = localX + (triangleBounds.w / 2.f);
    float checkY = (triangleBounds.h / 2.f) - localY;

    return (
        checkX >= 0 &&
        checkY >= 0 &&
        (checkX / triangleBounds.w) + (checkY / triangleBounds.h) <= 1.f
    );
}

bool C4_IsPointInsideEllipse(SDL_FPoint point, SDL_FRect circleBounds, float rotationDegrees, C4_UI_Mirror mirror) {
    float radiiX = circleBounds.w / 2.f;
    float radiiY = circleBounds.h / 2.f;

    if (radiiX == 0 || radiiY == 0) {
        return false;
    }

    float centerX = circleBounds.x + radiiX;
    float centerY = circleBounds.y + radiiY;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    float effectiveRotation = C4_UI_GetMirroredRotation(rotationDegrees, mirror);
    float angleRadians = -effectiveRotation * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    /*
    Same here as rectangle
    if (mirror == C4_UI_Mirror_X || mirror == C4_UI_Mirror_XY) {
        localX = -localX;
    }
    if (mirror == C4_UI_Mirror_Y || mirror == C4_UI_Mirror_XY) {
        localY = -localY;
    }
    */

    float result = (localX / radiiX) * (localX / radiiX) + (localY / radiiY) * (localY / radiiY);

    return result <= 1;
}

bool C4_IsPointInsideShape(C4_UI_ShapeType shapeType, SDL_FPoint point, SDL_FRect shapeBounds, float rotationDegrees, C4_UI_Mirror mirror) {
    switch (shapeType) {
        case C4_UI_Shape_Rectangle: return C4_IsPointInsideRectangle(point, shapeBounds, rotationDegrees, mirror);
        case C4_UI_Shape_Triangle: return C4_IsPointInsideTriangle(point, shapeBounds, rotationDegrees, mirror);
        case C4_UI_Shape_Ellipse: return C4_IsPointInsideEllipse(point, shapeBounds, rotationDegrees, mirror);
        default: return false;
    }
}