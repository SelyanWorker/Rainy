#pragma once

#include "event.h"

namespace Rainy {

	class RAINY_API KeyPressEvent : public Event
	{
	public:
		KeyPressEvent(unsigned KeyId)
			:m_KeyId(KeyId)
		{}
		~KeyPressEvent() = default;
		EventType GetType() { return EventType::KeyPressed; }
		static EventType GetStaticType() { return EventType::KeyPressed; }
		unsigned GetKeyCode() const { return m_KeyId; }
	private:
		unsigned m_KeyId;
	};

	class RAINY_API KeyReleaseEvent : public Event
	{
	public:
		KeyReleaseEvent(unsigned KeyId)
			:m_KeyId(KeyId)
		{}
		~KeyReleaseEvent() = default;
		EventType GetType() { return EventType::KeyRelease; }
		static EventType GetStaticType() { return EventType::KeyRelease; }
		unsigned GetKeyCode() const { return m_KeyId; }
	private:
		unsigned m_KeyId;
	};

	class RAINY_API CharInputEvent : public Event
	{
	public:
		CharInputEvent(unsigned symbol)
			:m_symbol(symbol)
		{}
		~CharInputEvent() = default;
		EventType GetType() { return EventType::CharInput; }
		static EventType GetStaticType() { return EventType::CharInput; }
		unsigned GetCharCode() const { return m_symbol; }
	private:
		unsigned m_symbol;

	};

	/*class RAINY_API KeyRepeatEvent : public Event
	{
	public:
		KeyRepeatEvent(unsigned KeyId, unsigned count)
			:m_KeyId(KeyId), m_count(count)
		{}
		~KeyRepeatEvent() = default;
		EventType GetType() { return EventType::KeyRepeat; }
		static EventType GetStaticType() { return EventType::KeyRepeat; }
	private:
		unsigned m_KeyId;
		unsigned m_count;
	};*/
}
