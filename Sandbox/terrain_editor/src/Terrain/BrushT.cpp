#include <cmath>

#include "BrushT.h"

// s::Brush::Brush(float size, float falloff, FalloffFunctionType falloff_function)
//	:falloff_function(falloff_function), size(size), falloff(falloff)
//{}
//
// float s::Brush::getFalloffFactor(float distance) const
//{
//	return falloff_function(falloff, distance);
// }
//
// void s::Brush::setFalloffFunction(FalloffFunctionType new_falloff_function)
//{
//	falloff_function = new_falloff_function;
// }

namespace te
{
    Brush::Brush(float size, float falloff, FalloffFuction falloffFunction)
      : falloffFunction(falloffFunction),
        size(size),
        falloff(falloff)
    {
    }

    void Brush::SetFalloffFunction(FalloffFuction falloffFunction)
    {
        this->falloffFunction = falloffFunction;
    }

    float Brush::GetFalloffFactor(float distance) const
    {
        return falloffFunction(falloff, distance, size);
    }

    float Brush::BaseFalloff(float falloff, float distance, float max_distance) { return 1; }

    float Brush::LinearFalloff(float falloff, float distance, float max_distance)
    {
        float min_distance = max_distance - max_distance * falloff;

        if (distance < min_distance)
            return 1;
        if (distance > max_distance)
            return 0;

        return 1 - (distance - min_distance) / (max_distance - min_distance);
    }

    float Brush::SmoothFalloff(float falloff, float distance, float max_distance)
    {
        float min_distance = max_distance - max_distance * falloff;

        if (distance < min_distance)
            return 1;
        if (distance > max_distance)
            return 0;

        float distance_factor = (distance - min_distance) / (max_distance - min_distance);

        float radians = distance_factor * (1.45f);

        // ��� ���������� �������, ������� - ��� ������ ��������� � ������ ���������,
        // ���� ������ �������, �� �������� � ��������, ��� ��� cos(x^2) ������ 2.5,
        // � ������ ��� �������� �������� 1.78 (�������� ��� cos(x) - 3.14..)
        float factor = (std::cos(radians * radians * radians) + 1) / 2;

        return factor;
    }
}
