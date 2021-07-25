#pragma once

#include "core/Core.h"
#include "layers/layer.h"
#include "event/Event.h"
#include "event/KeyEvents.h"
#include "event/MouseEvent.h"
#include "event/WindowEvents.h"
#include "core/Window.h"

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
