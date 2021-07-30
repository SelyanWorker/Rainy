#pragma once

#include "event.h"

namespace Rainy
{
    class RAINY_API KeyPressEvent : public Event
    {
    public:
        KeyPressEvent(unsigned keyId) : m_keyId(keyId) {}
        ~KeyPressEvent() = default;
        EventType getType() { return EventType::KeyPressed; }
        static EventType getStaticType() { return EventType::KeyPressed; }
        uint32_t getKeyCode() const { return m_keyId; }

    private:
        uint32_t m_keyId;
    };

    class RAINY_API KeyReleaseEvent : public Event
    {
    public:
        KeyReleaseEvent(unsigned KeyId) : m_KeyId(KeyId) {}
        ~KeyReleaseEvent() = default;
        EventType getType() { return EventType::KeyRelease; }
        static EventType getStaticType() { return EventType::KeyRelease; }
        uint32_t getKeyCode() const { return m_KeyId; }

    private:
        uint32_t m_KeyId;
    };

    class RAINY_API CharInputEvent : public Event
    {
    public:
        CharInputEvent(unsigned symbol) : m_symbol(symbol) {}
        ~CharInputEvent() = default;
        EventType getType() { return EventType::CharInput; }
        static EventType getStaticType() { return EventType::CharInput; }
        uint32_t getCharCode() const { return m_symbol; }

    private:
        uint32_t m_symbol;
    };

    /*class RAINY_API KeyRepeatEvent : public Event
    {
    public:
        KeyRepeatEvent(unsigned KeyId, unsigned count)
            :m_keyId(KeyId), m_count(count)
        {}
        ~KeyRepeatEvent() = default;
        EventType getType() { return EventType::KeyRepeat; }
        static EventType getStaticType() { return EventType::KeyRepeat; }
    private:
        unsigned m_keyId;
        unsigned m_count;
    };*/
}
