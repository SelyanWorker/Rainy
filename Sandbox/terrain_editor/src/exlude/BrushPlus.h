#pragma once

#include "Rainy/Rainy.h"
#include "Rainy/Core/Math.h"

using Rainy::Vector3f;


// to delete in future

//
namespace ls {

	using FalloffFuctionType = float(*)(float, float, float);

	float BaseFalloff(float falloff, float distance, float max_distance);
	float SmoothFalloff(float falloff, float distance, float max_distance);
	float LinearFalloff(float falloff, float distance, float max_distance);

	class Brush {
	private:
		FalloffFuctionType falloffFunction;

	public:
		float size{ 1 };
		float falloff{ 0.5f };

	public:
		explicit Brush(float size, float falloff,
			FalloffFuctionType falloffFunction = &BaseFalloff);

		~Brush() = default;

		float GetFalloffFactor(float distance) const;

		void SetFalloffFunction(FalloffFuctionType fallofffunctor);
	};

}
