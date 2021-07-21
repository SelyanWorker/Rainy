#include "rnpch.h"

#include "GLFW/glfw3.h"
#include "Rainy/Core/Time.h"

namespace Rainy {

	TimeStep TimeStep::GetTime()
	{
		return TimeStep(glfwGetTime());
	}

}