#include "core/Time.h"

#include "GLFW/glfw3.h"

namespace Rainy {

	TimeStep TimeStep::GetTime()
	{
		return TimeStep(glfwGetTime());
	}

}