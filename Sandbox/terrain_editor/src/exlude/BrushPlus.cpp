#include <cmath>

#include "BrushPlus.h"

//s::Brush::Brush(float size, float falloff, FalloffFunctionType falloff_function)
//	:falloff_function(falloff_function), size(size), falloff(falloff)
//{}
//
//float s::Brush::getFalloffFactor(float distance) const
//{
//	return falloff_function(falloff, distance);
//}
//
//void s::Brush::setFalloffFunction(FalloffFunctionType new_falloff_function)
//{
//	falloff_function = new_falloff_function;
//}

namespace ls {

	Brush::Brush(float size, float falloff, FalloffFuctionType falloffFunction)
		:falloffFunction(falloffFunction), size(size), falloff(falloff) {}

	void Brush::SetFalloffFunction(FalloffFuctionType falloffFunction)
	{
		this->falloffFunction = falloffFunction;
	}

	float Brush::GetFalloffFactor(float distance) const
	{
		return falloffFunction(falloff, distance, size);
	}

	float BaseFalloff(float falloff, float distance, float max_distance)
	{
		return 1;
	}

	float LinearFalloff(float falloff, float distance, float max_distance)
	{
		float min_distance = max_distance - max_distance * falloff;

		if (distance < min_distance)
			return 1;
		if (distance > max_distance)
			return 0;

		return 1 - (distance - min_distance) / (max_distance - min_distance);
	}

	float SmoothFalloff(float falloff, float distance, float max_distance)
	{
		float min_distance = max_distance - max_distance * falloff;

		if (distance < min_distance)
			return 1;
		if (distance > max_distance)
			return 0;

		float distance_factor = (distance - min_distance) / (max_distance - min_distance);

		float radians = distance_factor * (1.45f);

		// тут используем функцию, радианы - это фактор дистанции в нужном диапозоне,
		// если меняем функцию, то меняется и диапозон, так для cos(x^2) период 2.5,
		// а нужный нам диапозон примерно 1.78 (диапозон для cos(x) - 3.14..) 
		float factor = (std::cosf(radians * radians * radians) + 1) / 2;

		return factor;
	}
}