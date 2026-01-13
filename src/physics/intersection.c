#include "Connect4/physics/intersection.h"
#include <math.h>

bool C4_IsPointInsideRectangle(SDL_FPoint point, SDL_FRect rectangleBounds, float rotationDegrees) {
    float centerX = rectangleBounds.x + rectangleBounds.w / 2.f;
    float centerY = rectangleBounds.y + rectangleBounds.h / 2.f;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    // Rotate the point backwards to be able to just check basic rectangle collision
    float angleRadians = -rotationDegrees * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    // The rectangles local center is (0, 0), so i just check if the point is within half its size in every direction
    return (
        -rectangleBounds.w / 2.f <= localX &&
        rectangleBounds.w / 2.f >= localX &&
        -rectangleBounds.h / 2.f <= localY &&
        rectangleBounds.h / 2.f >= localY
    );
}

bool C4_IsPointInsideTriangle(SDL_FPoint point, SDL_FRect triangleBounds, float rotationDegrees) {
    if (triangleBounds.w == 0 || triangleBounds.h == 0) {
        return false;
    }

    float centerX = triangleBounds.x + triangleBounds.w / 2.f;
    float centerY = triangleBounds.y + triangleBounds.h / 2.f;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    float angleRadians = -rotationDegrees * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    // Shifting coordinates to the corner
    float checkX = localX + (triangleBounds.w / 2.f);
    float checkY = (triangleBounds.h / 2.f) - localY;

    return (
        checkX >= 0 &&
        checkY >= 0 &&
        (checkX / triangleBounds.w) + (checkY / triangleBounds.h) <= 1.f
    );
}

bool C4_IsPointInsideEllipse(SDL_FPoint point, SDL_FRect circleBounds, float rotationDegrees) {
    float radiiX = circleBounds.w / 2.f;
    float radiiY = circleBounds.h / 2.f;

    if (radiiX == 0 || radiiY == 0) {
        return false;
    }

    float centerX = circleBounds.x + radiiX;
    float centerY = circleBounds.y + radiiY;

    float relativeX = point.x - centerX;
    float relativeY = point.y - centerY;

    float angleRadians = -rotationDegrees * ((float)M_PI / 180.f);
    float localX = relativeX * cosf(angleRadians) - relativeY * sinf(angleRadians);
    float localY = relativeX * sinf(angleRadians) + relativeY * cosf(angleRadians);

    float result = (localX / radiiX) * (localX / radiiX) + (localY / radiiY) * (localY / radiiY);

    return result <= 1;
}

bool C4_IsPointInsideShape(C4_UI_ShapeType shapeType, SDL_FPoint point, SDL_FRect shapeBounds, float rotationDegrees) {
    switch (shapeType) {
        case C4_UI_Shape_Rectangle: return C4_IsPointInsideRectangle(point, shapeBounds, rotationDegrees);
        case C4_UI_Shape_Triangle: return C4_IsPointInsideTriangle(point, shapeBounds, rotationDegrees);
        case C4_UI_Shape_Ellipse: return C4_IsPointInsideEllipse(point, shapeBounds, rotationDegrees);
        default: return false;
    }
}