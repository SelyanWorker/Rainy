#pragma once

#include "core/core.h"


namespace Rainy
{
    enum EventCategory
    {
        MouseEvent,
        KeyEvent,
        ApplicationEvent
    };

    enum EventType
    {
        None = 0,
        ApplicationUpdate = 1,
        ApplicationRender = 2,
        ApplicationClose = 3,
        MouseMoved = 11,
        MouseKeyPressed = 12,
        MouseKeyRelease = 13,
        MouseScroll = 14,
        KeyPressed = 21,
        KeyRelease = 22,
        KeyRepeat = 23,
        CharInput = 24,
        WindowClose = 31,
        WindowResize = 32,
        WindowMove = 33
    };

    class RAINY_API Event
    {
    public:
        Event() : Handled(false) {}
        ~Event() = default;

        virtual EventType getType() { return EventType::None; }

        static EventType getStaticType() { return EventType::None; }

    public:
        bool Handled;
    };

    class RAINY_API EventDispatcher
    {
    public:
        EventDispatcher(Event &e) : e(e) {}
        ~EventDispatcher() = default;

        template<typename EventT, typename FunType>
        bool dispatch(FunType const &fun)
        {
            auto test_0 = e.getType();
            auto test_1 = EventT::getStaticType();
            if (e.getType() == EventT::getStaticType())
            {
                e.Handled = fun(static_cast<EventT &>(e));
                return true;
            }
            else
                return false;
        }

    private:
        Event &e;
    };

}
