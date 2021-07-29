#include "WindowsInput.h"
#include "WindowsWindow.h"
#include "application/application.h"
#include "core/input.h"

namespace Rainy
{
    Input *Input::m_input = new WindowsInput();

    bool WindowsInput::isKeyPressedImpl(uint32_t KeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::get()->getWindow()->getNativeWindow());
        return glfwGetKey(glfwWindow, KeyCode) == GLFW_PRESS;
    }

    bool WindowsInput::isKeyReleaseImpl(uint32_t KeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::get()->getWindow()->getNativeWindow());
        return glfwGetKey(glfwWindow, KeyCode) == GLFW_RELEASE;
    }

    bool WindowsInput::isMouseKeyPressedImpl(uint32_t mouseKeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::get()->getWindow()->getNativeWindow());
        return glfwGetMouseButton(glfwWindow, mouseKeyCode) == GLFW_PRESS;
    }

    bool WindowsInput::isMouseKeyReleasedImpl(uint32_t mouseKeyCode)
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::get()->getWindow()->getNativeWindow());
        return glfwGetMouseButton(glfwWindow, mouseKeyCode) == GLFW_RELEASE;
    }

    std::pair<float, float> WindowsInput::getCursorPositionImpl()
    {
        GLFWwindow *glfwWindow =
            static_cast<GLFWwindow *>(Application::get()->getWindow()->getNativeWindow());
        double xPos, yPos;
        glfwGetCursorPos(glfwWindow, &xPos, &yPos);
        int winXPos, winYPos;
        glfwGetWindowPos(glfwWindow, &winXPos, &winYPos);
        return { xPos + winXPos, yPos + winYPos };
    }

}
