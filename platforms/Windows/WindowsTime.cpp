#include "core/time.h"

#include "GLFW/glfw3.h"

namespace Rainy
{
    TimeStep TimeStep::getTime() { return TimeStep(glfwGetTime()); }

}
