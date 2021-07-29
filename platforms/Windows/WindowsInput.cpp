#include "WindowsInput.h"
#include "WindowsWindow.h"
#include "application/application.h"
#include "core/input.h"

namespace Rainy
{
    Input *Input::m_input = new WindowsInput();

    bool WindowsInput::IsKeyPressedImpl(uint32_t KeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow());
        return glfwGetKey(glfwWindow, KeyCode) == GLFW_PRESS;
    }

    bool WindowsInput::IsKeyReleaseImpl(uint32_t KeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow());
        return glfwGetKey(glfwWindow, KeyCode) == GLFW_RELEASE;
    }

    bool WindowsInput::IsMouseKeyPressedImpl(uint32_t mouseKeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow());
        return glfwGetMouseButton(glfwWindow, mouseKeyCode) == GLFW_PRESS;
    }

    bool WindowsInput::IsMouseKeyReleasedImpl(uint32_t mouseKeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow());
        return glfwGetMouseButton(glfwWindow, mouseKeyCode) == GLFW_RELEASE;
    }

    std::pair<float, float> WindowsInput::GetCursorPositionImpl()
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::Get()->GetWindow()->GetNativeWindow());
        double xpos;
        double ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);
        int winxpos;
        int winypos;
        glfwGetWindowPos(glfwWindow, &winxpos, &winypos);
        return { xpos + winxpos, ypos + winypos };
    }

}
