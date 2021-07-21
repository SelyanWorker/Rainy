#pragma once

#include "Core.h"
#include "Window.h"
#include "KeyCodes.h"
#include "Rainy/Event/Event.h"
#include "Rainy/Event/WindowEvents.h"
#include "Rainy/Event/KeyEvents.h"
#include "Rainy/Event/MouseEvent.h"
#include "Rainy/Layers/LayerStack.h"
#include "Rainy/ImGui/ImGuiLayer.h"
#include "Time.h"

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