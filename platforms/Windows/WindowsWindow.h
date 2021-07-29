#pragma once

#include "GLFW/glfw3.h"

#include "core/window.h"

namespace Rainy
{
    class WindowsWindow : public Window
    {
    public:
        WindowsWindow(WindowProperties &prop);
        ~WindowsWindow();

        uint32_t getWidth() const override;

        uint32_t getHeight() const override;

        void *getNativeWindow() const override;

        std::pair<int32_t, int32_t> getWindowsPosition() const override;

        void setEventFunction(EventFunType eventFun) override;

        void onUpdate() override;

        void swapBuffers() override;

        void enableCursor() override;

        void disableCursor() override;

        void Init();

        void Shutdown();

    private:
        struct WindowsWindowProp
        {
            char const *m_name;
            uint32_t m_width;
            uint32_t m_height;
            int32_t m_x;
            int32_t m_y;
            EventFunType m_eventFun;
        };
        WindowsWindowProp m_prop;
        GLFWwindow *m_window;
    };
}
