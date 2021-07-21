#pragma once

#include <functional>
#include <cstdint>

#include "Rainy/Core/Core.h"
#include "Rainy/Event/WindowEvents.h"

namespace Rainy {
	using EventFunType = std::function<void(Event& e)>;

	struct WindowProperties 
	{
		char const * m_name;
		uint32_t m_width;
		uint32_t m_height;
		WindowProperties(char const * name, uint32_t width, uint32_t height) 
			: m_name(name), m_width(width), m_height(height) {}
	};

	// Rainy window interface
	class RAINY_API Window
	{
	public:
		static Window * Create(WindowProperties properties);

		virtual uint32_t GetWidth() const = 0;
		
		virtual uint32_t GetHeight() const = 0;
		
		virtual void * GetNativeWindow() const = 0;

		virtual std::pair<int32_t, int32_t> GetWindowsPosition() const = 0;

		virtual void SetEventFunction(EventFunType eventFun) = 0;

		virtual void OnUpdate() = 0;
		
		virtual void SwapBuffers() = 0;

		virtual void EnableCursor() = 0;

		virtual void DisableCursor() = 0;
	};

}