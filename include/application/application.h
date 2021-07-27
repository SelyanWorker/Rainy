#pragma once

#include "core/core.h"
#include "core/window.h"
#include "core/key_codes.h"
#include "event/event.h"
#include "event/window_events.h"
#include "event/key_events.h"
#include "event/mouse_event.h"
#include "layers/layer_stack.h"
#include "imgui_layer/imgui_layer.h"
#include "core/time.h"

namespace Rainy {

	class RAINY_API Application
	{
	private:
		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);
		/*
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnKeyRelease(KeyReleaseEvent& e);
		*/
		void ShutDown();

	public:
		Application();

		virtual ~Application();

		void PushLayer(Layer* layer);

		void PopLayer();

		void Run();

		static Application* Get();

		Window* GetWindow();

		float GetFrameTime() const;

		void EnableCursor();

		void DisableCursor();

	private:
		bool m_run;
		Window* m_window;
		float m_frameTime;
		LayerStack m_layerStack;
		static Application* m_application;
	};

	Application* CreateApplication();

}
