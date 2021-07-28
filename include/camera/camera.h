#pragma once

#include "core/core.h"
#include "math/math.h"
#include "core/input.h"

namespace Rainy {

	class RAINY_API Camera
	{
	public:
		virtual void OnUpdate() = 0;

		virtual Matrix4f GetViewMatrix() const = 0;

		virtual Matrix4f GetProjectionMatrix() const = 0;

		virtual void SetPosition(Vector3f position) = 0;

		virtual void SetRotation(Vector3f rotation) = 0;

		virtual Vector3f GetPosition() const = 0;

		virtual Vector3f GetRotation() const = 0;

	};

}
