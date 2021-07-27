#pragma once

#include "core/core.h"
#include "layers/layer.h"
#include "event/event.h"
#include "event/key_events.h"
#include "event/mouse_event.h"
#include "event/window_events.h"
#include "core/window.h"

namespace Rainy{

	class RAINY_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnEvent(Event& e) override;

		void OnUpdate() override;

		void OnImGuiRender() override;

		void MenuBar();

		void Init();

		void Begin();

		void End();

	};

}
