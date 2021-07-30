#pragma once

#include "event.h"

namespace Rainy
{
    class RAINY_API WindowCloseEvent : public Event
    {
    public:
        EventType getType() override { return EventType::WindowClose; }
        static EventType getStaticType() { return EventType::WindowClose; }
    };

    class RAINY_API WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(uint32_t newWidth, uint32_t newHeight)
          : m_newWidth(newWidth),
            m_newHeight(newHeight)
        {
        }
        EventType getType() override { return EventType::WindowResize; }
        static EventType getStaticType() { return EventType::WindowResize; }

    private:
        uint32_t m_newWidth;
        uint32_t m_newHeight;
    };

    class RAINY_API WindowMoveEvent : public Event
    {
    public:
        WindowMoveEvent(int32_t x, int32_t y) : m_newX(x), m_newY(y) {}
        EventType getType() override { return EventType::WindowMove; }
        static EventType getStaticType() { return EventType::WindowMove; }

    private:
        int32_t m_newX;
        int32_t m_newY;
    };

}
