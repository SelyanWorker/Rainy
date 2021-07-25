#pragma once

#include "core/Core.h"
#include "math/Math.h"

namespace Rainy {

	struct RAINY_API Vertex2D
	{
		Vector3f position;
		Vector2f textCoord;
		Vector2f normal;
	};

	struct RAINY_API Vertex3D
	{
		Vector3f position;
		Vector2f textCoord;
		Vector3f normal;
	};

}
