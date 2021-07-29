#pragma once

#include "event.h"

namespace Rainy
{
    class RAINY_API MouseKeyReleaseEvent : public Event
    {
    public:
        MouseKeyReleaseEvent(uint32_t buttonCode) : m_buttonCode(buttonCode) {}
        ~MouseKeyReleaseEvent() = default;

        static EventType GetStaticType() { return EventType::MouseKeyRelease; }
        EventType GetType() { return EventType::MouseKeyRelease; }
        uint32_t GetKeyCode() const { return m_buttonCode; }

    private:
        uint32_t m_buttonCode;
    };

    class RAINY_API MouseKeyPressEvent : public Event
    {
    public:
        MouseKeyPressEvent(uint32_t buttonCode) : m_buttonCode(buttonCode) {}
        ~MouseKeyPressEvent() = default;

        static EventType GetStaticType() { return EventType::MouseKeyPressed; }
        EventType GetType() { return EventType::MouseKeyPressed; }
        uint32_t GetKeyCode() const { return m_buttonCode; }

    private:
        uint32_t m_buttonCode;
    };

    class RAINY_API MouseMoveEvent : public Event
    {
    public:
        MouseMoveEvent(float xPos, float yPos) : m_xPos(xPos), m_yPos(yPos) {}
        ~MouseMoveEvent() = default;

        static EventType GetStaticType() { return EventType::MouseMoved; }
        EventType GetType() { return EventType::MouseMoved; }

        float GetX() const { return m_xPos; }
        float GetY() const { return m_yPos; }

    private:
        float m_xPos;
        float m_yPos;
    };

    class RAINY_API MouseScrollEvent : public Event
    {
    public:
        MouseScrollEvent(float xPos, float yPos) : m_xOffset(xPos), m_yOffset(yPos) {}
        ~MouseScrollEvent() = default;

        static EventType GetStaticType() { return EventType::MouseScroll; }
        EventType GetType() { return EventType::MouseScroll; }

        float GetX() const { return m_xOffset; }
        float GeTy() const { return m_yOffset; }

    private:
        float m_xOffset;
        float m_yOffset;
    };
}
