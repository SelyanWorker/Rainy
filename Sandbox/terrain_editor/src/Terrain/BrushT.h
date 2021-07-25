#pragma once

#include "math/Math.h"

using Rainy::Vector3f;

// to delete in future

//
namespace te {

	class Brush
	{
	public:

		using FalloffFuction = float(*)(float, float, float);

		static float BaseFalloff(float falloff, float distance, float max_distance);
		static float SmoothFalloff(float falloff, float distance, float max_distance);
		static float LinearFalloff(float falloff, float distance, float max_distance);

	public:
		explicit Brush(float size, float falloff,
			FalloffFuction falloffFunction = &BaseFalloff);

		~Brush() = default;

		float GetFalloffFactor(float distance) const;

		void SetFalloffFunction(FalloffFuction fallofffunctor);

	public:
		float size{ 1 };
		float falloff{ 0.5f };

	private:
		FalloffFuction falloffFunction;
	};

}
