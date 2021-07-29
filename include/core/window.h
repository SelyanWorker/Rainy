#pragma once

#include "core.h"
#include "event/event.h"

namespace Rainy
{
    using EventFunType = std::function<void(Event &e)>;

    struct WindowProperties
    {
        char const *m_name;
        uint32_t m_width;
        uint32_t m_height;
        WindowProperties(char const *name, uint32_t width, uint32_t height)
          : m_name(name),
            m_width(width),
            m_height(height)
        {
        }
    };

    // Rainy window interface
    class RAINY_API Window
    {
    public:
        static Window *create(WindowProperties properties);

        virtual uint32_t getWidth() const = 0;

        virtual uint32_t getHeight() const = 0;

        virtual void *getNativeWindow() const = 0;

        virtual std::pair<int32_t, int32_t> getWindowsPosition() const = 0;

        virtual void setEventFunction(EventFunType eventFun) = 0;

        virtual void onUpdate() = 0;

        virtual void swapBuffers() = 0;

        virtual void enableCursor() = 0;

        virtual void disableCursor() = 0;
    };

}
