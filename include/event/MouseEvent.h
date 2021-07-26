#pragma once

#include "Event.h"

namespace  Rainy {
	
	class RAINY_API MouseKeyReleaseEvent : public Event
	{
	public:
		MouseKeyReleaseEvent(uint32_t buttonCode)
			:m_buttonCode(buttonCode)
		{}
		~MouseKeyReleaseEvent() = default;

		static EventType GetStaticType() { return EventType::MouseKeyRelease; }
		EventType GetType() { return EventType::MouseKeyRelease; }
		uint32_t GetKeyCode() const { return m_buttonCode; }
	private:
		uint32_t m_buttonCode;
	};

	class RAINY_API MouseKeyPressEvent : public Event
	{
	public:
		MouseKeyPressEvent(uint32_t buttonCode)
			:m_buttonCode(buttonCode)
		{}
		~MouseKeyPressEvent() = default;

		static EventType GetStaticType() { return EventType::MouseKeyPressed; }
		EventType GetType() { return EventType::MouseKeyPressed; }
		uint32_t GetKeyCode() const { return m_buttonCode; }
	private:
		uint32_t m_buttonCode;
	};

	class RAINY_API MouseMoveEvent : public Event
	{
	public:
		MouseMoveEvent(float xpos, float ypos)
			:m_xpos(xpos), m_ypos(ypos)
		{}
		~MouseMoveEvent() = default;

		static EventType GetStaticType() { return EventType::MouseMoved; }
		EventType GetType() { return EventType::MouseMoved; }

		float GetX() const { return m_xpos; }
		float GetY() const { return m_ypos; }

	private:
		float m_xpos;
		float m_ypos;
	};

	class RAINY_API MouseScrollEvent : public Event
	{
	public:
		MouseScrollEvent(float xpos, float ypos)
			:m_xoffset(xpos), m_yoffset(ypos)
		{}
		~MouseScrollEvent() = default;

		static EventType GetStaticType() { return EventType::MouseScroll; }
		EventType GetType() { return EventType::MouseScroll; }

		float GetX() const { return m_xoffset; }
		float GeTy() const { return m_yoffset; }

	private:
		float m_xoffset;
		float m_yoffset;
	};
}