#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

using Rainy::Vector3f;


// to delete in future
constexpr float STRENGTH = 0.05f; 
//

using FalloffFuctionType = float (*)(float, float, float);

float baseFalloffFunction(float falloff, float distance, float max_distance);
float smoothFalloffFunction(float falloff, float distance, float max_distance);
float linearFalloffFunction(float falloff, float distance, float max_distance);

class Brush {
private:
	FalloffFuctionType falloffFunction;

public:
	float size{ 1 };
	float falloff{ 0.5f };

public:
	explicit Brush(float size, float falloff, 
		FalloffFuctionType falloffFunction = &baseFalloffFunction);

	~Brush() = default;

	float getFalloffFactor(float distance) const;

	void setFalloffFunctor(FalloffFuctionType fallofffunctor);
};


