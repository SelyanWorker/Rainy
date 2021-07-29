#pragma once

#include "core/core.h"
#include "event/event.h"

namespace Rainy
{
    class RAINY_API Layer
    {
    public:
        virtual void onEvent(Event &e) = 0;

        virtual void onUpdate() = 0;

        virtual void onImGuiRender() = 0;
    };

}
