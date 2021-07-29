#pragma once

#include "core/core.h"
#include "core/window.h"
#include "event/event.h"
#include "event/key_events.h"
#include "event/mouse_event.h"
#include "event/window_events.h"
#include "layers/layer.h"

namespace Rainy
{
    class RAINY_API ImGuiLayer : public Layer
    {
    public:
        ImGuiLayer();
        ~ImGuiLayer();

        void onEvent(Event &e) override;

        void onUpdate() override;

        void onImGuiRender() override;

        void menuBar();

        void init();

        void begin();

        void end();
    };

}
