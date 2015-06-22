#include "MapElements.h"

#include <cmath>
#include <numeric>

#include "assert.h"
#include "Util.h"

using std::shared_ptr;

// SpawnComponentPoint

float SpawnComponentPoint::getArea() const {
    return 1.0;
}

Vector2D SpawnComponentPoint::getRandomPoint() const {
    return _point;
}

// SpawnComponentLine

float SpawnComponentLine::getArea() const {
    return (_point2 - _point1).length();
}

Vector2D SpawnComponentLine::getRandomPoint() const {
    float a = random_uniform_float(0.0f, 1.0f);
    return (a * _point1) + ((1.0f - a) * _point2);
}

// SpawnComponentRectangle

float SpawnComponentRectangle::getArea() const {
    return std::abs((_point1.x - _point2.x) * (_point1.y - _point2.y));
}

Vector2D SpawnComponentRectangle::getRandomPoint() const {
    return Vector2D(random_uniform_float(_point1.x, _point2.x),
                    random_uniform_float(_point1.y, _point2.y));
}

// SpawnComponentTriangle

float SpawnComponentTriangle::getArea() const {
    return 0.5f * std::abs(_point1.x * _point2.y + _point2.x * _point3.y +
                           _point3.x * _point1.y - _point1.x * _point3.y -
                           _point2.x * _point1.y - _point3.x * _point2.y);
}

Vector2D SpawnComponentTriangle::getRandomPoint() const {
    float a = random_uniform_float(0.0f, 1.0f);
    float b = random_uniform_float(0.0f, 1.0f);
    if (a + b > 1.0f) {
        a = 1.0f - a;
        b = 1.0f - b;
    }
    return ((1.0f - a - b) * _point1) + (a * _point2) + (b * _point3);
}

// Arc slice

float SpawnComponentSlice::getArea() const {
    const float theta = _theta2 - _theta1;
    return 0.5f * (theta - std::sin(theta)) * _radius * _radius;
}

Vector2D SpawnComponentSlice::getRandomPoint() const {
    const float theta = _theta2 - _theta1;
    float x, y;
    float sinThetaOver2 = std::sin(theta / 2.0f);
    float cosThetaOver2 = std::cos(theta / 2.0f);
    do {
        x = random_uniform_float(cosThetaOver2, 1.0f);
        if (theta >= M_PI) {
            y = random_uniform_float(-1.0, 1.0);
        } else {
            y = random_uniform_float(-sinThetaOver2, sinThetaOver2);
        }
    } while (x * x + y * y > 1.0f);
    return (Vector2D(x, y).getRotation(0.5f * (_theta1 + _theta2)) * _radius) +
           _center;
}

// Sector

float SpawnComponentSector::getArea() const {
    const float theta = _theta2 - _theta1;
    return 0.5f * theta * _radius * _radius;
}

Vector2D SpawnComponentSector::getRandomPoint() const {
    const float theta = random_uniform_float(_theta1, _theta2);
    const float r = std::sqrt(random_uniform_float(0.0f, 1.0f));
    return _center + (r * _radius * Vector2D::getUnitVector(theta));
}

// SpawnDescriptor

Vector2D SpawnDescriptor::getRandomPoint() const {
    assert(_components.size() > 0);

    const float totalArea = std::accumulate(
        _components.begin(), _components.end(), 0.0f,
        [](const float acc, shared_ptr<SpawnComponent> const &sc) {
            return acc + sc->getArea();
        });

    float r = random_uniform_float(0.0, totalArea);
    int index = -1;
    for (shared_ptr<SpawnComponent> const &sc : _components) {
        index++;
        r -= sc->getArea();
        if (r < 0) {
            break;
        }
    }

    return _components[index]->getRandomPoint();
}
