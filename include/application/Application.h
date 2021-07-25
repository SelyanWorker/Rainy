#pragma once

#include "core/Core.h"
#include "core/Window.h"
#include "core/KeyCodes.h"
#include "event/Event.h"
#include "event/WindowEvents.h"
#include "event/KeyEvents.h"
#include "event/MouseEvent.h"
#include "layers/LayerStack.h"
#include "imgui_layer/ImGuiLayer.h"
#include "core/Time.h"

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
